#include <QCheckBox>

#include "Core/Configuration.h"
#include "Application.h"
#include "CharmPreferences.h"

CharmPreferences::CharmPreferences( const Configuration& config,
                                    QWidget* parent_ )
    : QDialog( parent_ )
{
    m_ui.setupUi( this );
    m_ui.cbOneEventAtATime->setChecked( config.oneEventAtATime );
    m_ui.cbEventsInLeavesOnly->setChecked( config.eventsInLeafsOnly );
    m_ui.cb24hEditing->setChecked( config.always24hEditing );
    m_ui.cbIdleDetection->setEnabled( Application::instance().idleDetector() != 0 );
    m_ui.cbIdleDetection->setChecked( config.detectIdling );
    m_ui.cbAnimatedTrayIcon->setChecked( config.animatedTrayIcon );

    // this would not need a switch, but i hate casting enums to int:
    switch( config.taskTrackerFontSize ) {
    case Configuration::TaskTrackerFont_Small:
        m_ui.cbTaskTrackerFontSize->setCurrentIndex( 0 );
        break;
    case Configuration::TaskTrackerFont_Regular:
        m_ui.cbTaskTrackerFontSize->setCurrentIndex( 1 );
        break;
    case Configuration::TaskTrackerFont_Large:
        m_ui.cbTaskTrackerFontSize->setCurrentIndex( 2 );
        break;
    default:
        Q_ASSERT( false ); // somebody added an option
    };

    switch ( config.durationFormat ) {
    case Configuration::Minutes:
        m_ui.cbDurationFormat->setCurrentIndex( 0 );
        break;
    case Configuration::Decimal:
        m_ui.cbDurationFormat->setCurrentIndex( 1 );
        break;
    default:
        Q_ASSERT( !"unhandled DurationFormat enum value" );
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
    default:
        Q_ASSERT( false ); // somebody added an option
    };
    // resize( minimumSize() );
}

CharmPreferences::~CharmPreferences()
{
}

bool CharmPreferences::oneEventAtATime() const
{
    return m_ui.cbOneEventAtATime->isChecked();
}

bool CharmPreferences::eventsInLeafsOnly() const
{
    return m_ui.cbEventsInLeavesOnly->isChecked();
}

bool CharmPreferences::always24hEditing() const
{
    return m_ui.cb24hEditing->isChecked();
}

bool CharmPreferences::detectIdling() const
{
    return m_ui.cbIdleDetection->isChecked();
}

bool CharmPreferences::animatedTrayIcon() const
{
    return m_ui.cbAnimatedTrayIcon->isChecked();
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

Configuration::TaskTrackerFontSize CharmPreferences::taskTrackerFontSize() const
{
    switch( m_ui.cbTaskTrackerFontSize->currentIndex() ) {
    case 0:
        return Configuration::TaskTrackerFont_Small;
        break;
    case 1:
        return Configuration::TaskTrackerFont_Regular;
        break;
    case 2:
        return Configuration::TaskTrackerFont_Large;
        break;
    default:
        Q_ASSERT( false ); // somebody added an item
    }
    // always return something, to avoid compiler warning:
    return Configuration::TaskTrackerFont_Regular;
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

#include "CharmPreferences.moc"
