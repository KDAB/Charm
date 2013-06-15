#include <QCheckBox>
#include <QMessageBox>

#include "Core/Configuration.h"
#include "Application.h"
#include "CharmPreferences.h"
#include "MessageBox.h"

CharmPreferences::CharmPreferences( const Configuration& config,
                                    QWidget* parent_ )
    : QDialog( parent_ )
{
    m_ui.setupUi( this );
    m_ui.cbIdleDetection->setEnabled( Application::instance().idleDetector() != 0 );
    m_ui.cbIdleDetection->setChecked( config.detectIdling );
    m_ui.cbWarnUnuploadedTimesheets->setChecked( config.warnUnuploadedTimesheets );

    connect(m_ui.cbWarnUnuploadedTimesheets, SIGNAL(toggled(bool)),
            SLOT(slotWarnUnuploadedChanged(bool)));

    // this would not need a switch, but i hate casting enums to int:
    switch( config.timeTrackerFontSize ) {
    case Configuration::TimeTrackerFont_Small:
        m_ui.cbTimeTrackerFontSize->setCurrentIndex( 0 );
        break;
    case Configuration::TimeTrackerFont_Regular:
        m_ui.cbTimeTrackerFontSize->setCurrentIndex( 1 );
        break;
    case Configuration::TimeTrackerFont_Large:
        m_ui.cbTimeTrackerFontSize->setCurrentIndex( 2 );
        break;
    };

    switch ( config.durationFormat ) {
    case Configuration::Minutes:
        m_ui.cbDurationFormat->setCurrentIndex( 0 );
        break;
    case Configuration::Decimal:
        m_ui.cbDurationFormat->setCurrentIndex( 1 );
        break;
    }

    switch( config.toolButtonStyle ) {
    case Qt::ToolButtonIconOnly:
        m_ui.cbToolButtonStyle->setCurrentIndex( 0 );
        break;
    case Qt::ToolButtonTextOnly:
        m_ui.cbToolButtonStyle->setCurrentIndex( 1 );
        break;
    case Qt::ToolButtonTextUnderIcon:
        m_ui.cbToolButtonStyle->setCurrentIndex( 2 );
        break;
    case Qt::ToolButtonTextBesideIcon:
        m_ui.cbToolButtonStyle->setCurrentIndex( 3 );
        break;
    case Qt::ToolButtonFollowStyle:
        m_ui.cbToolButtonStyle->setCurrentIndex( 4 );
        break;
    };
    // resize( minimumSize() );
}

CharmPreferences::~CharmPreferences()
{
}

bool CharmPreferences::detectIdling() const
{
    return m_ui.cbIdleDetection->isChecked();
}

bool CharmPreferences::warnUnuploadedTimesheets() const
{
    return m_ui.cbWarnUnuploadedTimesheets->isChecked();
}

Configuration::DurationFormat CharmPreferences::durationFormat() const
{
    switch (m_ui.cbDurationFormat->currentIndex() ) {
    case 0:
        return Configuration::Minutes;
    case 1:
        return Configuration::Decimal;
    default:
        Q_ASSERT( !"Unexpected combobox item for DurationFormat" );
    }
    return Configuration::Minutes;
}

Configuration::TimeTrackerFontSize CharmPreferences::timeTrackerFontSize() const
{
    switch( m_ui.cbTimeTrackerFontSize->currentIndex() ) {
    case 0:
        return Configuration::TimeTrackerFont_Small;
        break;
    case 1:
        return Configuration::TimeTrackerFont_Regular;
        break;
    case 2:
        return Configuration::TimeTrackerFont_Large;
        break;
    default:
        Q_ASSERT( false ); // somebody added an item
    }
    // always return something, to avoid compiler warning:
    return Configuration::TimeTrackerFont_Regular;
}

Qt::ToolButtonStyle CharmPreferences::toolButtonStyle() const
{
    switch( m_ui.cbToolButtonStyle->currentIndex() ) {
    case 0:
        return Qt::ToolButtonIconOnly;
        break;
    case 1:
        return Qt::ToolButtonTextOnly;
        break;
    case 2:
        return Qt::ToolButtonTextUnderIcon;
        break;
    case 3:
        return Qt::ToolButtonTextBesideIcon;
        break;
    case 4:
        return Qt::ToolButtonFollowStyle;
        break;
    default:
        Q_ASSERT( false ); // somebody added an item
    }
    // always return something, to avoid compiler warning:
    return Qt::ToolButtonIconOnly;
}

void CharmPreferences::slotWarnUnuploadedChanged( bool enabled )
{
    if (!enabled)
    {
        const int response = MessageBox::question(this,
                                                  tr("Bill is sad :(."),
                                                  tr("Bill has always been misunderstood. All he really wants is your reports, and even when he doesn't get them you only have to evade him once per hour. I'm sure you want to keep Bill's gentle reminders?"),
                                                  tr("Mmmmkay"),
                                                  tr("No, Stop Bill"),
                                                  QMessageBox::Yes);
        if (response == QMessageBox::Yes)
            m_ui.cbWarnUnuploadedTimesheets->setCheckState(Qt::Checked);
    }
}

#include "CharmPreferences.moc"
