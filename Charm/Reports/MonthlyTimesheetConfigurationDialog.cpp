#include "MonthlyTimesheetConfigurationDialog.h"

#include <QCalendarWidget>
#include <QSettings>

#include "DateEntrySyncer.h"
#include "SelectTaskDialog.h"
#include "ViewHelpers.h"

#include "CharmCMake.h"

#include "MonthlyTimesheet.h"

#include "ui_MonthlyTimesheetConfigurationDialog.h"

static const int MAX_MONTH = 12;
static const int MIN_YEAR = 1990;

MonthlyTimesheetConfigurationDialog::MonthlyTimesheetConfigurationDialog( QWidget* parent )
    : ReportConfigurationDialog( parent )
    , m_ui( new Ui::MonthlyTimesheetConfigurationDialog )
{
    setWindowTitle( tr( "Monthly Timesheet" ) );

    m_ui->setupUi( this );
    connect( m_ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()) );
    connect( m_ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()) );

    connect( m_ui->comboBoxMonth, SIGNAL( currentIndexChanged( int ) ),
             SLOT( slotMonthComboItemSelected( int ) ) );
    connect( m_ui->toolButtonSelectTask, SIGNAL( clicked() ),
             SLOT( slotSelectTask() ) );
    connect( m_ui->checkBoxSubTasksOnly, SIGNAL( toggled( bool ) ),
             SLOT( slotCheckboxSubtasksOnlyChecked( bool ) ) );
    m_ui->comboBoxMonth->setCurrentIndex( 1 );
    slotCheckboxSubtasksOnlyChecked( m_ui->checkBoxSubTasksOnly->isChecked() );

    slotStandardTimeSpansChanged();
    connect( Application::instance().dateChangeWatcher(),
             SIGNAL( dateChanged() ),
             SLOT( slotStandardTimeSpansChanged() ) );

    // set current month and year:
    m_ui->spinBoxMonth->setValue(QDate::currentDate().month());
    m_ui->spinBoxYear->setValue(QDate::currentDate().year());

    // load settings:
    QSettings settings;
    if ( settings.contains( MetaKey_TimesheetActiveOnly ) ) {
        m_ui->checkBoxActiveOnly->setChecked( settings.value( MetaKey_TimesheetActiveOnly ).toBool() );
    } else {
        m_ui->checkBoxActiveOnly->setChecked( true );
    }
}

MonthlyTimesheetConfigurationDialog::~MonthlyTimesheetConfigurationDialog()
{
    delete m_ui; m_ui = 0;
}

void MonthlyTimesheetConfigurationDialog::setDefaultMonth(int yearOfMonth, int month)
{
    m_ui->spinBoxMonth->setValue(month);
    m_ui->spinBoxYear->setValue(yearOfMonth);
    m_ui->comboBoxMonth->setCurrentIndex(4);
}

void MonthlyTimesheetConfigurationDialog::accept()
{
    // save settings:
    QSettings settings;
    settings.setValue( MetaKey_TimesheetActiveOnly,
                       m_ui->checkBoxActiveOnly->isChecked() );
    settings.setValue( MetaKey_TimesheetRootTask,
                       m_rootTask );

    QDialog::accept();
}

void MonthlyTimesheetConfigurationDialog::showReportPreviewDialog( QWidget* parent )
{
    QDate start, end;
    int index = m_ui->comboBoxMonth->currentIndex();
    if ( index == m_monthInfo.size() -1 ) {
        // manual selection
        start = QDate(m_ui->spinBoxYear->value(), m_ui->spinBoxMonth->value(), 1);
        end = start.addMonths(1);
    } else {
        start = m_monthInfo[index].timespan.first;
        end = m_monthInfo[index].timespan.second;
    }
    bool activeOnly = m_ui->checkBoxActiveOnly->isChecked();
    MonthlyTimeSheetReport* report = new MonthlyTimeSheetReport( parent );
    report->setReportProperties( start, end, m_rootTask, activeOnly );
    report->show();
}

void MonthlyTimesheetConfigurationDialog::showEvent( QShowEvent* )
{
    QSettings settings;

    // we only want to do this once a backend is loaded, and we ignore
    // the saved root task if it does not exist anymore
    if ( settings.contains( MetaKey_TimesheetRootTask ) ) {
        TaskId root = settings.value( MetaKey_TimesheetRootTask ).toInt();
        const TaskTreeItem& item = DATAMODEL->taskTreeItem( root );
        if ( item.isValid() ) {
            m_rootTask = root;
            m_ui->labelTaskName->setText( DATAMODEL->fullTaskName( item.task() ) );
            m_ui->checkBoxSubTasksOnly->setChecked( true );
        }
    }
}

void MonthlyTimesheetConfigurationDialog::slotCheckboxSubtasksOnlyChecked( bool checked )
{
    if ( checked && m_rootTask == 0 ) {
        slotSelectTask();
    }

    if ( ! checked ) {
        m_rootTask = 0;
        m_ui->labelTaskName->setText( tr( "(All Tasks)" ) );
    }
}

void MonthlyTimesheetConfigurationDialog::slotStandardTimeSpansChanged()
{
    const TimeSpans timeSpans;
    m_monthInfo = timeSpans.last4Months();
    NamedTimeSpan custom = {
        tr( "Manual Selection" ),
        timeSpans.thisMonth().timespan
    };
    m_monthInfo << custom;
    m_ui->comboBoxMonth->clear();
    for ( int i = 0; i < m_monthInfo.size(); ++i )
    {
        m_ui->comboBoxMonth->addItem( m_monthInfo[i].name );
    }
    // Set current index to "Last Month" as that's what you'll usually want
    m_ui->comboBoxMonth->setCurrentIndex( 1 );
}

void MonthlyTimesheetConfigurationDialog::slotMonthComboItemSelected( int index )
{
    // wait for the next update, in this case:
    if ( m_ui->comboBoxMonth->count() == 0 || index == -1 ) return;
    Q_ASSERT( m_ui->comboBoxMonth->count() > index );

    if ( index == m_monthInfo.size() - 1 ) { // manual selection
        m_ui->groupBox->setEnabled( true );
    } else {
        m_ui->groupBox->setEnabled( false );
    }
}

void MonthlyTimesheetConfigurationDialog::slotSelectTask()
{
    SelectTaskDialog dialog( this );
    if ( dialog.exec() ) {
        m_rootTask = dialog.selectedTask();
        const TaskTreeItem& item = DATAMODEL->taskTreeItem( m_rootTask );
        m_ui->labelTaskName->setText( DATAMODEL->fullTaskName( item.task() ) );
    } else {
        if ( m_rootTask == 0 )
            m_ui->checkBoxSubTasksOnly->setChecked( false );
    }
}

