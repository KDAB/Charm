#include <QCheckBox>

#include "Core/Configuration.h"
#include "CharmPreferences.h"

CharmPreferences::CharmPreferences( const Configuration& config,
                                    QWidget* parent_ )
    : QDialog( parent_ )
{
    m_ui.setupUi( this );
    m_ui.cbOneEventAtATime->setChecked( config.oneEventAtATime );
    m_ui.cbEventsInLeavesOnly->setChecked( config.eventsInLeafsOnly );
    m_ui.cb24hEditing->setChecked( config.always24hEditing );

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
    default:
        Q_ASSERT( false ); // somebody added an item
    }
    // always return something, to avoid compiler warning:
    return Qt::ToolButtonIconOnly;
}

#include "CharmPreferences.moc"
