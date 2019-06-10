/*
  ActivityReport.cpp

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

#include "ActivityReport.h"
#include "ApplicationCore.h"
#include "DateEntrySyncer.h"
#include "Data.h"
#include "SelectTaskDialog.h"

#include "Core/Configuration.h"
#include "Core/Dates.h"

#include <QCalendarWidget>
#include <QDomDocument>
#include <QDomElement>
#include <QFile>
#include <QPushButton>
#include <QTimer>
#include <QtAlgorithms>
#include <QUrl>

#include "ui_ActivityReportConfigurationDialog.h"

ActivityReportConfigurationDialog::ActivityReportConfigurationDialog(QWidget *parent)
    : ReportConfigurationDialog(parent)
    , m_ui(new Ui::ActivityReportConfigurationDialog)
{
    setWindowTitle(tr("Activity Report"));

    m_ui->setupUi(this);
    m_ui->dateEditEnd->calendarWidget()->setFirstDayOfWeek(Qt::Monday);
    m_ui->dateEditEnd->calendarWidget()->setVerticalHeaderFormat(QCalendarWidget::ISOWeekNumbers);
    m_ui->dateEditStart->calendarWidget()->setFirstDayOfWeek(Qt::Monday);
    m_ui->dateEditStart->calendarWidget()->setVerticalHeaderFormat(QCalendarWidget::ISOWeekNumbers);
    m_ui->tabWidget->setCurrentIndex(0);
    connect(m_ui->buttonBox, &QDialogButtonBox::accepted,
            this, &ActivityReportConfigurationDialog::accept);
    connect(m_ui->buttonBox, &QDialogButtonBox::rejected,
            this, &ActivityReportConfigurationDialog::reject);
    connect(m_ui->comboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &ActivityReportConfigurationDialog::slotTimeSpanSelected );
    connect(m_ui->addExcludeTaskButton, &QToolButton::clicked,
            this, &ActivityReportConfigurationDialog::slotExcludeTask);
    connect(m_ui->removeExcludeTaskButton, &QToolButton::clicked,
            this, &ActivityReportConfigurationDialog::slotRemoveExcludedTask);
    connect(m_ui->addIncludeTaskButton,  &QToolButton::clicked,
            this, &ActivityReportConfigurationDialog::slotSelectTask);
    connect(m_ui->removeIncludeTaskButton, &QToolButton::clicked,
             this, &ActivityReportConfigurationDialog::slotRemoveIncludeTask);
    connect(m_ui->checkBoxGroupTasks, &QCheckBox::clicked,
            this, &ActivityReportConfigurationDialog::slotGroupTasks);
    connect(m_ui->checkBoxGroupTasksComments, &QCheckBox::clicked,
            this, &ActivityReportConfigurationDialog::slotGroupTasksComments);

    new DateEntrySyncer(m_ui->spinBoxStartWeek, m_ui->spinBoxStartYear, m_ui->dateEditStart, 1,
                        this);
    new DateEntrySyncer(m_ui->spinBoxEndWeek, m_ui->spinBoxEndYear, m_ui->dateEditEnd, 7, this);

    QTimer::singleShot(0, this, SLOT(slotDelayedInitialization()));
}

ActivityReportConfigurationDialog::~ActivityReportConfigurationDialog()
{
}

void ActivityReportConfigurationDialog::slotDelayedInitialization()
{
    slotStandardTimeSpansChanged();
    connect(ApplicationCore::instance().dateChangeWatcher(),
            &DateChangeWatcher::dateChanged,
            this,
            &ActivityReportConfigurationDialog::slotStandardTimeSpansChanged);
    // FIXME load settings
}

void ActivityReportConfigurationDialog::slotStandardTimeSpansChanged()
{
    const TimeSpans timeSpans;
    m_timespans = timeSpans.standardTimeSpans();
    NamedTimeSpan customRange = {
        tr("Select Range"),
        timeSpans.thisWeek().timespan,
        Range
    };
    m_timespans << customRange;
    m_ui->comboBox->clear();
    for (int i = 0; i < m_timespans.size(); ++i)
        m_ui->comboBox->addItem(m_timespans[i].name);
}

void ActivityReportConfigurationDialog::slotTimeSpanSelected(int index)
{
    if (m_ui->comboBox->count() == 0 || index == -1) return;
    Q_ASSERT(m_ui->comboBox->count() > index);
    if (index == m_timespans.size() - 1) {   // manual selection
        m_ui->groupBox->setEnabled(true);
    } else {
        m_ui->spinBoxStartYear->setValue(m_timespans[index].timespan.first.year());
        m_ui->spinBoxStartWeek->setValue(m_timespans[index].timespan.first.weekNumber());
        m_ui->spinBoxEndYear->setValue(m_timespans[index].timespan.second.year());
        m_ui->spinBoxEndWeek->setValue(m_timespans[index].timespan.second.weekNumber());
        m_ui->dateEditStart->setDate(m_timespans[index].timespan.first);
        m_ui->dateEditEnd->setDate(m_timespans[index].timespan.second);
        m_ui->groupBox->setEnabled(false);
    }
}

void ActivityReportConfigurationDialog::slotSelectTask()
{
    TaskId taskId;
    if (selectTask(taskId) && !m_properties.rootTasks.contains(taskId)) {
        addListItem(taskId, m_ui->listWidgetIncludeTask);
        m_properties.rootTasks << taskId;
    }
    m_ui->removeIncludeTaskButton->setEnabled(!m_properties.rootTasks.isEmpty());
    m_ui->listWidgetIncludeTask->setEnabled(!m_properties.rootTasks.isEmpty());
}

void ActivityReportConfigurationDialog::slotExcludeTask()
{
    TaskId taskId;
    if (selectTask(taskId) && !m_properties.rootExcludeTasks.contains(taskId)) {
        addListItem(taskId, m_ui->listWidgetExclude);
        m_properties.rootExcludeTasks << taskId;
    }
    m_ui->removeExcludeTaskButton->setEnabled(!m_properties.rootExcludeTasks.isEmpty());
    m_ui->listWidgetExclude->setEnabled(!m_properties.rootExcludeTasks.isEmpty());
}

void ActivityReportConfigurationDialog::slotRemoveExcludedTask()
{
    QListWidgetItem *item = m_ui->listWidgetExclude->currentItem();
    if (item) {
        m_properties.rootExcludeTasks.remove(item->data(Qt::UserRole).toInt());
        delete item;
    }
    m_ui->removeExcludeTaskButton->setEnabled(!m_properties.rootExcludeTasks.isEmpty());
    m_ui->listWidgetExclude->setEnabled(!m_properties.rootExcludeTasks.isEmpty());
}

void ActivityReportConfigurationDialog::slotRemoveIncludeTask()
{
    QListWidgetItem *item = m_ui->listWidgetIncludeTask->currentItem();
    if (item) {
        m_properties.rootTasks.remove(item->data(Qt::UserRole).toInt());
        delete item;
    }
    m_ui->removeIncludeTaskButton->setEnabled(!m_properties.rootTasks.isEmpty());
    m_ui->listWidgetIncludeTask->setEnabled(!m_properties.rootTasks.isEmpty());
}

void ActivityReportConfigurationDialog::slotGroupTasks(bool checked)
{
    m_ui->checkBoxGroupTasksComments->setEnabled(!checked);
}

void ActivityReportConfigurationDialog::slotGroupTasksComments(bool checked)
{
    m_ui->checkBoxGroupTasks->setEnabled(!checked);
}

bool ActivityReportConfigurationDialog::selectTask(TaskId &task)
{
    SelectTaskDialog dialog(this);
    dialog.setNonTrackableSelectable();
    dialog.setNonValidSelectable();
    const bool taskSelected = dialog.exec();
    if (taskSelected)
        task = dialog.selectedTask();
    return taskSelected;
}

QListWidgetItem *ActivityReportConfigurationDialog::addListItem(TaskId taskId,
                                                                QListWidget *list) const
{
    const TaskTreeItem &item = DATAMODEL->taskTreeItem(taskId);
    QListWidgetItem *listItem = new QListWidgetItem(DATAMODEL->smartTaskName(item.task()),
                                                    list);
    listItem->setToolTip(DATAMODEL->fullTaskName(item.task()));
    listItem->setData(Qt::UserRole, taskId);
    return listItem;
}

void ActivityReportConfigurationDialog::accept()
{
    // FIXME save settings
    QDialog::accept();
}

void ActivityReportConfigurationDialog::reject()
{
    QDialog::reject();
}

void ActivityReportConfigurationDialog::showReportPreviewDialog()
{
    const int index = m_ui->comboBox->currentIndex();
    m_properties.timeSpanSelection = m_timespans[index];
    if (index == m_timespans.size() - 1) {   //Range
        m_properties.start = m_ui->dateEditStart->date();
        m_properties.end = m_ui->dateEditEnd->date().addDays(1);
    } else {
        m_properties.start = m_timespans[index].timespan.first;
        m_properties.end = m_timespans[index].timespan.second;
    }
    m_properties.showFullDescription = m_ui->checkBoxFullDescription->isChecked();
    m_properties.groupByTaskId = m_ui->checkBoxGroupTasks->isChecked();
    m_properties.groupByTaskIdAndComments = m_ui->checkBoxGroupTasksComments->isChecked();

    auto report = new ActivityReport();
    report->setReportProperties(m_properties);
    report->show();
}

ActivityReport::ActivityReport(QWidget *parent)
    : ReportPreviewWindow(parent)
{
    saveToXmlButton()->hide();
    saveToTextButton()->hide();
    uploadButton()->hide();
    connect(this, &ReportPreviewWindow::nextClicked,
            this, [this]() {
                updateRange(1);
            });
    connect(this, &ReportPreviewWindow::previousClicked,
            this, [this]() {
                updateRange(-1);
            });
}

ActivityReport::~ActivityReport()
{
}

void ActivityReport::setReportProperties(
    const ActivityReportConfigurationDialog::Properties &properties)
{
    m_properties = properties;
    slotUpdate();
}

void ActivityReport::slotUpdate()
{
    // retrieve matching events:
    EventIdList matchingEvents = DATAMODEL->eventsThatStartInTimeFrame(m_properties.start,
                                                                       m_properties.end);

    if (!m_properties.rootTasks.isEmpty()) {
        QSet<EventId> filteredEvents;
        Q_FOREACH (TaskId include, m_properties.rootTasks)
            filteredEvents |= Charm::filteredBySubtree(matchingEvents, include).toSet();
        matchingEvents = filteredEvents.toList();
    }

    if (m_properties.groupByTaskId) {
        matchingEvents = Charm::eventIdsSortedBy(matchingEvents,
                                                 Charm::SortOrderList() << Charm::SortOrder::TaskId
                                                                        << Charm::SortOrder::StartTime);
    } else if (m_properties.groupByTaskIdAndComments) {
        matchingEvents = Charm::eventIdsSortedBy(matchingEvents,
                                                 Charm::SortOrderList() << Charm::SortOrder::TaskId
                                                                        << Charm::SortOrder::Comment
                                                                        << Charm::SortOrder::StartTime);
    } else {
        matchingEvents = Charm::eventIdsSortedBy(matchingEvents,
                                                 Charm::SortOrderList()
                                                 << Charm::SortOrder::StartTime);
    }

    // filter unproductive events:
    Q_FOREACH (TaskId exclude, m_properties.rootExcludeTasks)
        matchingEvents = Charm::filteredBySubtree(matchingEvents, exclude, true);

    // calculate total:
    int totalSeconds = 0;
    Q_FOREACH (EventId id, matchingEvents) {
        const Event &event = DATAMODEL->eventForId(id);
        Q_ASSERT(event.isValid());
        totalSeconds += event.duration();
    }

    // which TimeSpan type
    QString timeSpanTypeName;
    switch (m_properties.timeSpanSelection.timeSpanType) {
    case Day:
        timeSpanTypeName = tr("Day");
        break;
    case Week:
        timeSpanTypeName = tr("Week");
        break;
    case Month:
        timeSpanTypeName = tr("Month");
        break;
    case Year:
        timeSpanTypeName = tr("Year");
        break;
    case Range:
        timeSpanTypeName = tr("Range");
        break;
    default:
        Q_ASSERT(false);   // should not happen
    }

    setTimeSpanTypeName(timeSpanTypeName);
    auto report = new QTextDocument(this);
    QDomDocument doc = createReportTemplate();
    QDomElement root = doc.documentElement();
    QDomElement body = root.firstChildElement(QStringLiteral("body"));

    // create the caption:
    {
        QDomElement headline = doc.createElement(QStringLiteral("h1"));
        QDomText text = doc.createTextNode(tr("Activity Report"));
        headline.appendChild(text);
        body.appendChild(headline);
    }
    {
        QDomElement headline = doc.createElement(QStringLiteral("h3"));
        QString content = tr("Report for %1, from %2 to %3")
                          .arg(CONFIGURATION.user.name(),
                               m_properties.start.toString(Qt::TextDate),
                               m_properties.end.addDays(-1).toString(Qt::TextDate));
        QDomText text = doc.createTextNode(content);
        headline.appendChild(text);
        body.appendChild(headline);
        {
            QDomElement paragraph = doc.createElement(QStringLiteral("h4"));
            QString totalsText = tr("Total: %1").arg(hoursAndMinutes(totalSeconds));
            QDomText totalsElement = doc.createTextNode(totalsText);
            paragraph.appendChild(totalsElement);
            body.appendChild(paragraph);
        }
        if (!m_properties.rootTasks.isEmpty()) {
            QDomElement paragraph = doc.createElement(QStringLiteral("p"));
            QString rootTaskText = tr("Activity under tasks:");

            Q_FOREACH (TaskId taskId, m_properties.rootTasks) {
                const Task &task = DATAMODEL->getTask(taskId);
                rootTaskText.append(QStringLiteral(" ( %1 ),").arg(DATAMODEL->fullTaskName(task)));
            }
            rootTaskText = rootTaskText.mid(0, rootTaskText.length() - 1);
            QDomText rootText = doc.createTextNode(rootTaskText);
            paragraph.appendChild(rootText);
            body.appendChild(paragraph);
        }

        QDomElement paragraph = doc.createElement(QStringLiteral("br"));
        body.appendChild(paragraph);
    }
    {
        const QString Headlines[] = {
            tr("Date and Time, Task, Description")
        };
        const int NumberOfColumns = sizeof Headlines / sizeof Headlines[0];

        // now for a table
        QDomElement table = doc.createElement(QStringLiteral("table"));
        table.setAttribute(QStringLiteral("width"), QStringLiteral("100%"));
        table.setAttribute(QStringLiteral("align"), QStringLiteral("left"));
        table.setAttribute(QStringLiteral("cellpadding"), QStringLiteral("3"));
        table.setAttribute(QStringLiteral("cellspacing"), QStringLiteral("0"));
        body.appendChild(table);
        // table header
        QDomElement tableHead = doc.createElement(QStringLiteral("thead"));
        table.appendChild(tableHead);
        QDomElement headerRow = doc.createElement(QStringLiteral("tr"));
        headerRow.setAttribute(QStringLiteral("class"), QStringLiteral("header_row"));
        tableHead.appendChild(headerRow);
        // column headers
        for (int i = 0; i < NumberOfColumns; ++i) {
            QDomElement header = doc.createElement(QStringLiteral("th"));
            QDomText text = doc.createTextNode(Headlines[i]);
            header.appendChild(text);
            headerRow.appendChild(header);
        }
        QDomElement tableBody = doc.createElement(QStringLiteral("tbody"));
        table.appendChild(tableBody);
        // rows
        const bool groupTasks = m_properties.groupByTaskId || m_properties.groupByTaskIdAndComments;
        int groupTotalSeconds = 0;
        for (auto it = matchingEvents.constBegin(), end = matchingEvents.constEnd(); it != end;
             ++it) {
            const EventId id(*it);
            const Event &event = DATAMODEL->eventForId(id);
            Q_ASSERT(event.isValid());
            bool nextMatch = false;

            if (groupTasks) {
                const auto next(it + 1);
                const EventId nextId(next != end ? *next : 0);
                const Event &nextEvent(DATAMODEL->eventForId(nextId));

                nextMatch = event.taskId() == nextEvent.taskId();

                if (nextMatch && m_properties.groupByTaskIdAndComments)
                    nextMatch = Charm::collatorCompare(event.comment(), nextEvent.comment()) == 0;

                groupTotalSeconds += event.duration();

                if (nextMatch)
                    continue;
            }

            const TaskTreeItem &item = DATAMODEL->taskTreeItem(event.taskId());
            const Task &task = item.task();
            Q_ASSERT(task.isValid());

            const auto paddedId = QStringLiteral("%1").arg(QString::number(
                                                               task.id()).trimmed(),
                                                           Configuration::instance().taskPaddingLength,
                                                           QLatin1Char('0'));

            QStringList row1Texts;

            if (groupTasks) {
                row1Texts = QStringList {
                    tr("%1 -- [%2] %3")
                    .arg(hoursAndMinutes(groupTotalSeconds),
                         paddedId,
                         m_properties.showFullDescription ? DATAMODEL->fullTaskName(
                             task) : task.name().trimmed())
                };
            } else {
                row1Texts = QStringList {
                    tr("%1 %2-%3 (%4) -- [%5] %6")
                    .arg(event.startDateTime().date().toString(Qt::SystemLocaleShortDate).trimmed(),
                         event.startDateTime().time().toString(Qt::SystemLocaleShortDate).trimmed(),
                         event.endDateTime().time().toString(Qt::SystemLocaleShortDate).trimmed(),
                         hoursAndMinutes(event.duration()),
                         paddedId,
                         m_properties.showFullDescription ? DATAMODEL->fullTaskName(
                             task) : task.name().trimmed())
                };
            }

            QDomElement row1 = doc.createElement(QStringLiteral("tr"));
            row1.setAttribute(QStringLiteral("class"), QStringLiteral("event_attributes_row"));
            QDomElement row2 = doc.createElement(QStringLiteral("tr"));
            for (int index = 0; index < NumberOfColumns; ++index) {
                QDomElement cell = doc.createElement(QStringLiteral("td"));
                cell.setAttribute(QStringLiteral("class"), QStringLiteral("event_attributes"));
                QDomText text = doc.createTextNode(row1Texts[index]);
                cell.appendChild(text);
                row1.appendChild(cell);
            }
            QDomElement cell2 = doc.createElement(QStringLiteral("td"));
            cell2.setAttribute(QStringLiteral("class"), QStringLiteral("event_description"));
            cell2.setAttribute(QStringLiteral("align"), QStringLiteral("left"));
            QDomElement preElement = doc.createElement(QStringLiteral("pre"));
            QDomText preText = doc.createTextNode(
                m_properties.groupByTaskId ? QString() : event.comment());
            preElement.appendChild(preText);
            cell2.appendChild(preElement);
            row2.appendChild(cell2);

            tableBody.appendChild(row1);
            tableBody.appendChild(row2);

            if (groupTasks) {
                if (!nextMatch)
                    groupTotalSeconds = 0;
            }
        }
    }

    // NOTE: seems like the style sheet has to be set before the html
    // code is pushed into the QTextDocument
    report->setDefaultStyleSheet(Charm::reportStylesheet(palette()));

    report->setHtml(doc.toString());
    setDocument(report);
}

void ActivityReport::updateRange(int direction)
{
    switch (m_properties.timeSpanSelection.timeSpanType) {
    case Day:
        m_properties.start = m_properties.start.addDays(1 * direction);
        m_properties.end = m_properties.end.addDays(1 * direction);
        break;
    case Week:
        m_properties.start = m_properties.start.addDays(7 * direction);
        m_properties.end = m_properties.end.addDays(7 * direction);
        break;
    case Month:
        m_properties.start = m_properties.start.addMonths(1 * direction);
        m_properties.end = m_properties.end.addMonths(1 * direction);
        break;
    case Year:
        m_properties.start = m_properties.start.addYears(1 * direction);
        m_properties.end = m_properties.end.addYears(1 * direction);
        break;
    case Range:
    {
        const int spanRange = m_properties.start.daysTo(m_properties.end);
        m_properties.start = m_properties.start.addDays(spanRange * direction);
        m_properties.end = m_properties.end.addDays(spanRange * direction);
        break;
    }
    default:
        Q_ASSERT(false);   // should not happen
    }
    setReportProperties(m_properties);
}
