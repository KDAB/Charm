/*
  WeeklyTimesheet.cpp

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2014-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Frank Osterfeld <frank.osterfeld@kdab.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "WeeklyTimesheet.h"
#include "Reports/WeeklyTimesheetXmlWriter.h"

#include <QCalendarWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QSettings>
#include <QUrl>

#include <Core/Dates.h>

#include "DateEntrySyncer.h"
#include "HttpClient/UploadTimesheetJob.h"
#include "Lotsofcake/Configuration.h"
#include "Widgets/HttpJobProgressDialog.h"

#include "SelectTaskDialog.h"
#include "ViewHelpers.h"

#include "ui_WeeklyTimesheetConfigurationDialog.h"

namespace {
static QString SETTING_GRP_TIMESHEETS = QStringLiteral("timesheets");
static QString SETTING_VAL_FIRSTYEAR = QStringLiteral("firstYear");
static QString SETTING_VAL_FIRSTWEEK = QStringLiteral("firstWeek");
static const int MAX_WEEK = 53;
static const int MIN_YEAR = 1990;
static const int DaysInWeek = 7;

enum TimeSheetTableColumns {
    Column_Task,
    Column_Monday,
    Column_Tuesday,
    Column_Wednesday,
    Column_Thursday,
    Column_Friday,
    Column_Saturday,
    Column_Sunday,
    Column_Total,
    NumberOfColumns
};
}

void addUploadedTimesheet(int year, int week)
{
    Q_ASSERT(year >= MIN_YEAR && week > 0 && week <= MAX_WEEK);
    QSettings settings;
    settings.beginGroup(SETTING_GRP_TIMESHEETS);
    QString yearStr = QString::number(year);
    QString weekStr = QString::number(week);
    QStringList existingSheets = settings.value(yearStr).toStringList();
    if (!existingSheets.contains(weekStr))
        settings.setValue(yearStr, existingSheets << weekStr);
    if (settings.value(SETTING_VAL_FIRSTYEAR, QString()).toString().isEmpty())
        settings.setValue(SETTING_VAL_FIRSTYEAR, yearStr);
    if (settings.value(SETTING_VAL_FIRSTWEEK, QString()).toString().isEmpty())
        settings.setValue(SETTING_VAL_FIRSTWEEK, weekStr);
}

WeeksByYear missingTimeSheets()
{
    WeeksByYear missing;
    QSettings settings;
    settings.beginGroup(SETTING_GRP_TIMESHEETS);
    int year = 0;
    int week = QDateTime::currentDateTime().date().weekNumber(&year);
    int firstYear = settings.value(SETTING_VAL_FIRSTYEAR, year).value<int>();
    int firstWeek = settings.value(SETTING_VAL_FIRSTWEEK, week).value<int>();
    for (int iYear = firstYear; iYear <= year; ++iYear) {
        QStringList uploaded = settings.value(QString::number(iYear)).toStringList();
        int firstWeekOfYear = iYear == firstYear ? firstWeek : 1;
        int lastWeekOfYear = iYear == year ? week - 1 : Charm::numberOfWeeksInYear(iYear);
        for (int iWeek = firstWeekOfYear; iWeek <= lastWeekOfYear; ++iWeek) {
            if (!uploaded.contains(QString::number(iWeek))) {
                Q_ASSERT(iYear >= MIN_YEAR && iWeek > 0 && iWeek <= MAX_WEEK);
                missing[iYear].append(iWeek);
            }
        }
    }
    return missing;
}

/************************************************** WeeklyTimesheetConfigurationDialog */

WeeklyTimesheetConfigurationDialog::WeeklyTimesheetConfigurationDialog(QWidget *parent)
    : ReportConfigurationDialog(parent)
    , m_ui(new Ui::WeeklyTimesheetConfigurationDialog)
{
    setWindowTitle(tr("Weekly Timesheet"));

    m_ui->setupUi(this);
    m_ui->dateEditDay->calendarWidget()->setFirstDayOfWeek(Qt::Monday);
    m_ui->dateEditDay->calendarWidget()->setVerticalHeaderFormat(QCalendarWidget::ISOWeekNumbers);
    connect(m_ui->buttonBox, &QDialogButtonBox::accepted,
            this, &WeeklyTimesheetConfigurationDialog::accept);
    connect(m_ui->buttonBox, &QDialogButtonBox::rejected,
            this, &WeeklyTimesheetConfigurationDialog::reject);

    connect(m_ui->comboBoxWeek, SIGNAL(currentIndexChanged(int)),
            SLOT(slotWeekComboItemSelected(int)));
    connect(m_ui->toolButtonSelectTask, &QToolButton::clicked,
            this, &WeeklyTimesheetConfigurationDialog::slotSelectTask);
    connect(m_ui->checkBoxSubTasksOnly, &QCheckBox::toggled,
            this, &WeeklyTimesheetConfigurationDialog::slotCheckboxSubtasksOnlyChecked);
    m_ui->comboBoxWeek->setCurrentIndex(1);
    slotCheckboxSubtasksOnlyChecked(m_ui->checkBoxSubTasksOnly->isChecked());
    new DateEntrySyncer(m_ui->spinBoxWeek, m_ui->spinBoxYear, m_ui->dateEditDay, 1, this);

    slotStandardTimeSpansChanged();
    connect(ApplicationCore::instance().dateChangeWatcher(), &DateChangeWatcher::dateChanged,
            this, &WeeklyTimesheetConfigurationDialog::slotStandardTimeSpansChanged);

    // load settings:
    QSettings settings;
    if (settings.contains(MetaKey_TimesheetActiveOnly)) {
        m_ui->checkBoxActiveOnly->setChecked(settings.value(MetaKey_TimesheetActiveOnly).toBool());
    } else {
        m_ui->checkBoxActiveOnly->setChecked(true);
    }
}

WeeklyTimesheetConfigurationDialog::~WeeklyTimesheetConfigurationDialog()
{
}

void WeeklyTimesheetConfigurationDialog::setDefaultWeek(int yearOfWeek, int week)
{
    m_ui->spinBoxWeek->setValue(week);
    m_ui->spinBoxYear->setValue(yearOfWeek);
    m_ui->comboBoxWeek->setCurrentIndex(4);
}

void WeeklyTimesheetConfigurationDialog::accept()
{
    // save settings:
    QSettings settings;
    settings.setValue(MetaKey_TimesheetActiveOnly,
                      m_ui->checkBoxActiveOnly->isChecked());
    settings.setValue(MetaKey_TimesheetRootTask,
                      m_rootTask);

    QDialog::accept();
}

void WeeklyTimesheetConfigurationDialog::showReportPreviewDialog()
{
    QDate start, end;
    int index = m_ui->comboBoxWeek->currentIndex();
    if (index == m_weekInfo.size() -1) {
        // manual selection
        QDate selectedDate = m_ui->dateEditDay->date();
        start = selectedDate.addDays(-selectedDate.dayOfWeek() + 1);
        end = start.addDays(DaysInWeek);
    } else {
        start = m_weekInfo[index].timespan.first;
        end = m_weekInfo[index].timespan.second;
    }
    bool activeOnly = m_ui->checkBoxActiveOnly->isChecked();
    auto report = new WeeklyTimeSheetReport();
    report->setReportProperties(start, end, m_rootTask, activeOnly);
    report->show();
}

void WeeklyTimesheetConfigurationDialog::showEvent(QShowEvent *)
{
    QSettings settings;

    // we only want to do this once a backend is loaded, and we ignore
    // the saved root task if it does not exist anymore
    if (settings.contains(MetaKey_TimesheetRootTask)) {
        TaskId root = settings.value(MetaKey_TimesheetRootTask).toInt();
        const TaskTreeItem &item = DATAMODEL->taskTreeItem(root);
        if (item.isValid()) {
            m_rootTask = root;
            m_ui->labelTaskName->setText(DATAMODEL->fullTaskName(item.task()));
            m_ui->checkBoxSubTasksOnly->setChecked(true);
        }
    }
}

void WeeklyTimesheetConfigurationDialog::slotCheckboxSubtasksOnlyChecked(bool checked)
{
    if (checked && m_rootTask == 0)
        slotSelectTask();

    if (!checked) {
        m_rootTask = 0;
        m_ui->labelTaskName->setText(tr("(All Tasks)"));
    }
}

void WeeklyTimesheetConfigurationDialog::slotStandardTimeSpansChanged()
{
    const TimeSpans timeSpans;
    m_weekInfo = timeSpans.last4Weeks();
    NamedTimeSpan custom = {
        tr("Manual Selection"),
        timeSpans.thisWeek().timespan,
        Range
    };
    m_weekInfo << custom;
    m_ui->comboBoxWeek->clear();
    for (int i = 0; i < m_weekInfo.size(); ++i)
        m_ui->comboBoxWeek->addItem(m_weekInfo[i].name);
    // Set current index to "Last Week" as that's what you'll usually want
    m_ui->comboBoxWeek->setCurrentIndex(1);
}

void WeeklyTimesheetConfigurationDialog::slotWeekComboItemSelected(int index)
{
    // wait for the next update, in this case:
    if (m_ui->comboBoxWeek->count() == 0 || index == -1) return;
    Q_ASSERT(m_ui->comboBoxWeek->count() > index);

    if (index == m_weekInfo.size() - 1) {   // manual selection
        m_ui->groupBox->setEnabled(true);
    } else {
        m_ui->dateEditDay->setDate(m_weekInfo[index].timespan.first);
        m_ui->groupBox->setEnabled(false);
    }
}

void WeeklyTimesheetConfigurationDialog::slotSelectTask()
{
    SelectTaskDialog dialog(this);
    dialog.setNonTrackableSelectable();
    dialog.setNonValidSelectable();
    if (dialog.exec()) {
        m_rootTask = dialog.selectedTask();
        const TaskTreeItem &item = DATAMODEL->taskTreeItem(m_rootTask);
        m_ui->labelTaskName->setText(DATAMODEL->fullTaskName(item.task()));
    } else {
        if (m_rootTask == 0)
            m_ui->checkBoxSubTasksOnly->setChecked(false);
    }
}

/*************************************************************** WeeklyTimeSheetReport */
// here begins ... the actual report:

WeeklyTimeSheetReport::WeeklyTimeSheetReport(QWidget *parent)
    : TimeSheetReport(parent)
{
    QPushButton *upload = uploadButton();
    connect(upload, &QPushButton::clicked, this, &WeeklyTimeSheetReport::slotUploadTimesheet);
    connect(this, &ReportPreviewWindow::nextClicked, this, [this] () {
        updateRange(7);
    });
    connect(this, &ReportPreviewWindow::previousClicked, this, [this] () {
        updateRange(-7);
    });

    if (!Lotsofcake::Configuration().isConfigured())
        upload->hide();
}

WeeklyTimeSheetReport::~WeeklyTimeSheetReport()
{
}

void WeeklyTimeSheetReport::setReportProperties(
    const QDate &start, const QDate &end, TaskId rootTask, bool activeTasksOnly)
{
    m_weekNumber = start.weekNumber(&m_yearOfWeek);
    TimeSheetReport::setReportProperties(start, end, rootTask, activeTasksOnly);
}

void WeeklyTimeSheetReport::slotUploadTimesheet()
{
    const Lotsofcake::Configuration configuration;
    auto client = new UploadTimesheetJob(this);
    client->setUsername(configuration.username());
    client->setUploadUrl(configuration.timesheetUploadUrl());
    auto dialog = new HttpJobProgressDialog(client, this);
    dialog->setWindowTitle(tr("Uploading"));
    connect(client, &HttpJob::finished, this, &WeeklyTimeSheetReport::slotTimesheetUploaded);
    client->setFileName(suggestedFileName());
    client->setPayload(saveToXml(ExcludeTaskList));
    client->start();
    uploadButton()->setEnabled(false);
}

void WeeklyTimeSheetReport::slotTimesheetUploaded(HttpJob *client)
{
    uploadButton()->setEnabled(true);
    switch (client->error()) {
    case HttpJob::NoError:
        addUploadedTimesheet(m_yearOfWeek, m_weekNumber);
        QMessageBox::information(this, tr("Timesheet Uploaded"),
                                 tr("Your timesheet was successfully uploaded."));
        break;
    case HttpJob::Canceled:
        break;
    case HttpJob::AuthenticationFailed:
        uploadButton()->setEnabled(false);
        slotUploadTimesheet();
        break;
    default:
        QMessageBox::critical(this, tr("Error"),
                              tr("Could not upload timesheet: %1").arg(client->errorString()));
    }
}

QString WeeklyTimeSheetReport::suggestedFileName() const
{
    return tr("WeeklyTimeSheet-%1-%2").arg(m_yearOfWeek).arg(m_weekNumber, 2, 10, QLatin1Char('0'));
}

void WeeklyTimeSheetReport::update()
{   // this creates the time sheet
    // retrieve matching events:
    const EventIdList matchingEvents
        = DATAMODEL->eventsThatStartInTimeFrame(startDate(), endDate());

    m_secondsMap.clear();

    // for every task, make a vector that includes a number of seconds
    // for every day of the week ( int seconds[7]), and store those in
    // a map by their task id
    Q_FOREACH (EventId id, matchingEvents) {
        const Event &event = DATAMODEL->eventForId(id);
        QVector<int> seconds(DaysInWeek);
        if (m_secondsMap.contains(event.taskId()))
            seconds = m_secondsMap.value(event.taskId());
        // what day in the week is the event (normalized to vector indexes):
        int dayOfWeek = event.startDateTime().date().dayOfWeek() - 1;
        Q_ASSERT(dayOfWeek >= 0 && dayOfWeek < DaysInWeek);
        seconds[dayOfWeek] += event.duration();
        // store in minute map:
        m_secondsMap[event.taskId()] = seconds;
    }

    setTimeSpanTypeName(tr("Week"));

    // now the reporting:
    // headline first:
    QTextDocument report;
    QDomDocument doc = createReportTemplate();
    QDomElement root = doc.documentElement();
    QDomElement body = root.firstChildElement(QStringLiteral("body"));

    // create the caption:
    {
        QDomElement headline = doc.createElement(QStringLiteral("h1"));
        QDomText text = doc.createTextNode(tr("Weekly Time Sheet"));
        headline.appendChild(text);
        body.appendChild(headline);
    }
    {
        QDomElement headline = doc.createElement(QStringLiteral("h3"));
        QString content = tr("Report for %1, Week %2 (%3 to %4)")
                          .arg(CONFIGURATION.user.name())
                          .arg(m_weekNumber, 2, 10, QLatin1Char('0'))
                          .arg(startDate().toString(Qt::TextDate))
                          .arg(endDate().addDays(-1).toString(Qt::TextDate));
        QDomText text = doc.createTextNode(content);
        headline.appendChild(text);
        body.appendChild(headline);

        QDomElement paragraph = doc.createElement(QStringLiteral("br"));
        body.appendChild(paragraph);
    }
    {
        // now for a table
        // retrieve the information for the report:
        // TimeSheetInfoList timeSheetInfo = taskWithSubTasks( rootTask(), secondsMap() );
        TimeSheetInfoList timeSheetInfo = TimeSheetInfo::filteredTaskWithSubTasks(
            TimeSheetInfo::taskWithSubTasks(DATAMODEL, DaysInWeek, rootTask(), secondsMap()),
            activeTasksOnly());

        QDomElement table = doc.createElement(QStringLiteral("table"));
        table.setAttribute(QStringLiteral("width"), QStringLiteral("100%"));
        table.setAttribute(QStringLiteral("align"), QStringLiteral("left"));
        table.setAttribute(QStringLiteral("cellpadding"), QStringLiteral("3"));
        table.setAttribute(QStringLiteral("cellspacing"), QStringLiteral("0"));
        body.appendChild(table);

        TimeSheetInfo totalsLine(DaysInWeek);
        if (!timeSheetInfo.isEmpty()) {
            totalsLine = timeSheetInfo.first();
            if (rootTask() == 0)
                timeSheetInfo.removeAt(0);   // there is always one, because there is always the root item
        }

        QDomElement headerRow = doc.createElement(QStringLiteral("tr"));
        headerRow.setAttribute(QStringLiteral("class"), QStringLiteral("header_row"));
        table.appendChild(headerRow);
        QDomElement headerDayRow = doc.createElement(QStringLiteral("tr"));
        headerDayRow.setAttribute(QStringLiteral("class"), QStringLiteral("header_row"));
        table.appendChild(headerDayRow);

        const QString Headlines[NumberOfColumns] = {
            tr("Task"),
            QDate::shortDayName(1),
            QDate::shortDayName(2),
            QDate::shortDayName(3),
            QDate::shortDayName(4),
            QDate::shortDayName(5),
            QDate::shortDayName(6),
            QDate::shortDayName(7),
            tr("Total")
        };
        const QString DayHeadlines[NumberOfColumns] = {
            QString(),
            tr("%1").arg(startDate().day(), 2, 10, QLatin1Char('0')),
            tr("%1").arg(startDate().addDays(1).day(), 2, 10, QLatin1Char('0')),
            tr("%1").arg(startDate().addDays(2).day(), 2, 10, QLatin1Char('0')),
            tr("%1").arg(startDate().addDays(3).day(), 2, 10, QLatin1Char('0')),
            tr("%1").arg(startDate().addDays(4).day(), 2, 10, QLatin1Char('0')),
            tr("%1").arg(startDate().addDays(5).day(), 2, 10, QLatin1Char('0')),
            tr("%1").arg(startDate().addDays(6).day(), 2, 10, QLatin1Char('0')),
            QString()
        };

        for (int i = 0; i < NumberOfColumns; ++i) {
            QDomElement header = doc.createElement(QStringLiteral("th"));
            QDomText text = doc.createTextNode(Headlines[i]);
            header.appendChild(text);
            headerRow.appendChild(header);
            QDomElement dayHeader = doc.createElement(QStringLiteral("th"));
            QDomText dayText = doc.createTextNode(DayHeadlines[i]);
            dayHeader.appendChild(dayText);
            headerDayRow.appendChild(dayHeader);
        }

        for (int i = 0; i < timeSheetInfo.size(); ++i) {
            QDomElement row = doc.createElement(QStringLiteral("tr"));
            if (i % 2)
                row.setAttribute(QStringLiteral("class"), QStringLiteral("alternate_row"));
            table.appendChild(row);

            QString texts[NumberOfColumns];
            texts[Column_Task] = timeSheetInfo[i].formattedTaskIdAndName(
                CONFIGURATION.taskPaddingLength);
            texts[Column_Monday] = hoursAndMinutes(timeSheetInfo[i].seconds[0]);
            texts[Column_Tuesday] = hoursAndMinutes(timeSheetInfo[i].seconds[1]);
            texts[Column_Wednesday] = hoursAndMinutes(timeSheetInfo[i].seconds[2]);
            texts[Column_Thursday] = hoursAndMinutes(timeSheetInfo[i].seconds[3]);
            texts[Column_Friday] = hoursAndMinutes(timeSheetInfo[i].seconds[4]);
            texts[Column_Saturday] = hoursAndMinutes(timeSheetInfo[i].seconds[5]);
            texts[Column_Sunday] = hoursAndMinutes(timeSheetInfo[i].seconds[6]);
            texts[Column_Total] = hoursAndMinutes(timeSheetInfo[i].total());

            for (int column = 0; column < NumberOfColumns; ++column) {
                QDomElement cell = doc.createElement(QStringLiteral("td"));
                cell.setAttribute(QStringLiteral("align"),
                                  column
                                  == Column_Task ? QStringLiteral("left") : QStringLiteral("center"));

                if (column == Column_Task) {
                    QString style = QStringLiteral("text-indent: %1px;")
                                    .arg(9 * timeSheetInfo[i].indentation);
                    cell.setAttribute(QStringLiteral("style"), style);
                }

                QDomText text = doc.createTextNode(texts[column]);
                cell.appendChild(text);
                row.appendChild(cell);
            }
        }
        // put the totals:
        QString TotalsTexts[NumberOfColumns] = {
            tr("Total:"),
            hoursAndMinutes(totalsLine.seconds[0]),
            hoursAndMinutes(totalsLine.seconds[1]),
            hoursAndMinutes(totalsLine.seconds[2]),
            hoursAndMinutes(totalsLine.seconds[3]),
            hoursAndMinutes(totalsLine.seconds[4]),
            hoursAndMinutes(totalsLine.seconds[5]),
            hoursAndMinutes(totalsLine.seconds[6]),
            hoursAndMinutes(totalsLine.total())
        };
        QDomElement totals = doc.createElement(QStringLiteral("tr"));
        totals.setAttribute(QStringLiteral("class"), QStringLiteral("header_row"));
        table.appendChild(totals);
        for (int i = 0; i < NumberOfColumns; ++i) {
            QDomElement cell = doc.createElement(QStringLiteral("th"));
            QDomText text = doc.createTextNode(TotalsTexts[i]);
            cell.appendChild(text);
            totals.appendChild(cell);
        }
    }

    // NOTE: seems like the style sheet has to be set before the html
    // code is pushed into the QTextDocument
    report.setDefaultStyleSheet(Charm::reportStylesheet(palette()));

    report.setHtml(doc.toString());
    setDocument(&report);
    uploadButton()->setEnabled(true);
}

QByteArray WeeklyTimeSheetReport::saveToXml(SaveToXmlMode mode)
{
    try {
        WeeklyTimesheetXmlWriter timesheet;
        timesheet.setDataModel(DATAMODEL);
        timesheet.setYear(m_yearOfWeek);
        timesheet.setWeekNumber(m_weekNumber);
        timesheet.setRootTask(rootTask());
        timesheet.setIncludeTaskList(mode == IncludeTaskList);
        const EventIdList matchingEventIds = DATAMODEL->eventsThatStartInTimeFrame(
            startDate(), endDate());
        EventList events;
        events.reserve(matchingEventIds.size());
        Q_FOREACH (const EventId &id, matchingEventIds)
            events.append(DATAMODEL->eventForId(id));
        timesheet.setEvents(events);

        return timesheet.saveToXml();
    } catch (const XmlSerializationException &e) {
        QMessageBox::critical(this, tr("Error exporting the report"), e.what());
    }

    return QByteArray();
}

QByteArray WeeklyTimeSheetReport::saveToText()
{
    QByteArray output;
    QTextStream stream(&output);
    QString content = tr("Report for %1, Week %2 (%3 to %4)")
                      .arg(CONFIGURATION.user.name())
                      .arg(m_weekNumber, 2, 10, QLatin1Char('0'))
                      .arg(startDate().toString(Qt::TextDate))
                      .arg(endDate().addDays(-1).toString(Qt::TextDate));
    stream << content << '\n';
    stream << '\n';
    TimeSheetInfoList timeSheetInfo = TimeSheetInfo::filteredTaskWithSubTasks(
        TimeSheetInfo::taskWithSubTasks(DATAMODEL, DaysInWeek, rootTask(), secondsMap()),
        activeTasksOnly());

    TimeSheetInfo totalsLine(DaysInWeek);
    if (!timeSheetInfo.isEmpty()) {
        totalsLine = timeSheetInfo.first();
        if (rootTask() == 0)
            timeSheetInfo.removeAt(0);   // there is always one, because there is always the root item
    }

    for (int i = 0; i < timeSheetInfo.size(); ++i)
        stream << timeSheetInfo[i].formattedTaskIdAndName(CONFIGURATION.taskPaddingLength)
               << "\t" << hoursAndMinutes(timeSheetInfo[i].total()) << '\n';
    stream << '\n';
    stream << "Week total: " << hoursAndMinutes(totalsLine.total()) << '\n';
    stream.flush();

    return output;
}

void WeeklyTimeSheetReport::updateRange(int deltaDays)
{
    QDate start = startDate().addDays(deltaDays);
    QDate end = endDate().addDays(deltaDays);
    setReportProperties(start, end, rootTask(), activeTasksOnly());
}
