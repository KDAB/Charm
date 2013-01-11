#include "DateEntrySyncer.h"
#include <Core/Dates.h>

#include <QDateEdit>
#include <QSpinBox>

#include <Core/Dates.h>

DateEntrySyncer::DateEntrySyncer( QSpinBox* week, QSpinBox* year, QDateEdit* date, int weekDay, QObject* parent )
    : QObject( parent )
    , m_week( week )
    , m_year( year )
    , m_date( date )
    , m_weekDay( weekDay )
{
    connect( m_week, SIGNAL(valueChanged(int)), this, SLOT(dateSelectionChanged()) );
    connect( m_year, SIGNAL(valueChanged(int)), this, SLOT(dateSelectionChanged()) );
    if ( m_date )
        connect( m_date, SIGNAL(dateChanged(QDate)), this, SLOT(dateSelectionChanged()) );
}

// number of weeks per year differs between 52 and 53, so we need to set the maximum value accordingly, and fix the value if the user flips through years
static void fixWeek( QSpinBox* yearSb, QSpinBox* weekSb ) {
    const int year = yearSb->value();
    const int week = weekSb->value();
    const int maxWeek = Charm::numberOfWeeksInYear( year );
    Q_ASSERT( maxWeek >= 52 );
    const int newWeek = qMin( maxWeek, week );
    weekSb->blockSignals( true );
    weekSb->setMaximum( maxWeek );
    weekSb->setValue( newWeek );
    weekSb->blockSignals( false );
}

void DateEntrySyncer::dateSelectionChanged()
{
    if ( sender() == m_week || sender() == m_year ) {
        //spinboxes changed, update date edit
        fixWeek( m_year, m_week );
        const int week = m_week->value();
        const int year = m_year->value();
        if ( m_date ) {
            m_date->blockSignals( true );
            m_date->setDate( Charm::dateByWeekNumberAndWeekDay( year, week, m_weekDay ) );
            m_date->blockSignals( false );
        }
    } else {
        Q_ASSERT( m_date );
        //date edit changed, update spinboxes
        const QDate date = m_date->date();
        int year = 0;
        const int week = date.weekNumber( &year );
        m_year->blockSignals( true );
        m_week->blockSignals( true );
        m_year->setValue( year );
        m_week->setValue( week );
        m_week->blockSignals( false );
        m_year->blockSignals( false );
    }
}

#include "DateEntrySyncer.moc"
