#include <QtDebug>
#include <QSettings>

#include "Configuration.h"
#include "CharmConstants.h"

#ifdef NDEBUG
#define DEFAULT_CONFIG_GROUP "default"
#else
#define DEFAULT_CONFIG_GROUP "debug"
#endif

Configuration& Configuration::instance()
{
    static Configuration configuration;
    return configuration;
}

Configuration::Configuration()
    : oneEventAtATime( true )
    , taskPrefilteringMode( TaskPrefilter_ShowAll )
    , taskTrackerFontSize( TaskTrackerFont_Regular )
    , always24hEditing( false )
    , durationFormat( Minutes )
    , toolButtonStyle( Qt::ToolButtonFollowStyle )
    , showStatusBar( true )
    , detectIdling( true )
    , animatedTrayIcon( true )
    , configurationName( DEFAULT_CONFIG_GROUP )
    , installationId( 0 )
    , newDatabase( false )
    , failure( false )
    , taskPaddingLength( 6 ) // arbitrary
{
}

Configuration::Configuration( bool _eventsInLeafsOnly, bool _oneEventAtATime, User _user,
                              TaskPrefilteringMode _taskPrefilteringMode,
                              TaskTrackerFontSize _taskTrackerFontSize,
                              bool _always24hEditing, DurationFormat _durationFormat, bool _detectIdling,
                              Qt::ToolButtonStyle _buttonstyle,
                              bool _showStatusBar, bool _animatedTrayIcon )
    : oneEventAtATime( _oneEventAtATime )
    , taskPrefilteringMode( _taskPrefilteringMode )
    , taskTrackerFontSize( _taskTrackerFontSize )
    , always24hEditing( _always24hEditing )
    , durationFormat( _durationFormat )
    , toolButtonStyle( _buttonstyle )
    , showStatusBar( _showStatusBar )
    , detectIdling ( _detectIdling )
    , animatedTrayIcon( _animatedTrayIcon )
    , configurationName( DEFAULT_CONFIG_GROUP )
    , installationId( 0 )
    , newDatabase( false )
    , failure( false )
    , taskPaddingLength( 6 ) // arbitrary
{
}

bool Configuration::operator==( const Configuration& other ) const
{
    return oneEventAtATime == other.oneEventAtATime &&
        user == other.user &&
        taskPrefilteringMode == other.taskPrefilteringMode &&
        taskTrackerFontSize == other.taskTrackerFontSize &&
        always24hEditing == other.always24hEditing &&
        durationFormat == other.durationFormat &&
        detectIdling == other.detectIdling &&
        animatedTrayIcon == other.animatedTrayIcon &&
        toolButtonStyle == other.toolButtonStyle &&
        showStatusBar == other.showStatusBar &&
        configurationName == other.configurationName &&
        installationId == other.installationId &&
        localStorageType == other.localStorageType &&
        localStorageDatabase == other.localStorageDatabase;
}

void Configuration::writeTo( QSettings& settings )
{
    settings.setValue( MetaKey_Key_InstallationId, installationId );
    settings.setValue( MetaKey_Key_UserId, user.id() );
    settings.setValue( MetaKey_Key_LocalStorageType, localStorageType );
    settings.setValue( MetaKey_Key_LocalStorageDatabase, localStorageDatabase );
    dump( "(Configuration::writeTo stored configuration)" );
}

bool Configuration::readFrom( QSettings& settings )
{
    bool complete = true;
    if ( settings.contains( MetaKey_Key_InstallationId ) ) {
        installationId = settings.value( MetaKey_Key_InstallationId ).toInt();
    } else {
        complete = false;
    }
    if ( settings.contains( MetaKey_Key_UserId ) ) {
        user.setId( settings.value( MetaKey_Key_UserId ).toInt() );
    } else {
        complete = false;
    }
    if ( settings.contains( MetaKey_Key_LocalStorageType ) ) {
        localStorageType = settings.value( MetaKey_Key_LocalStorageType ).toString();
    } else {
        complete = false;
    }
    if ( settings.contains( MetaKey_Key_LocalStorageDatabase ) ) {
        localStorageDatabase = settings.value( MetaKey_Key_LocalStorageDatabase ).toString();
    } else {
        complete = false;
    }
    dump( "(Configuration::readFrom loaded configuration)" );
    return complete;
}

void Configuration::dump( const QString& why )
{
    // dump configuration:
    return; // disable debug output
    qDebug() << "Configuration: configuration:"
             << ( why.isEmpty() ? QString() : why )
             << endl
             << "--> installation id:        " << installationId << endl
             << "--> userid:                 " << user.id() << endl
             << "--> local storage type:     " << localStorageType << endl
             << "--> local storage database: " << localStorageDatabase << endl
             << "--> one event at a time:       " << oneEventAtATime << endl
             << "--> task prefiltering mode:   " << taskPrefilteringMode << endl
             << "--> task tracker font size: " << taskTrackerFontSize << endl
             << "--> 24h time editing:       " << always24hEditing << endl
             << "--> duration format:        " << durationFormat << endl
             << "--> Idle Detection:         " << detectIdling << endl
             << "--> toolButtonStyle:        " << toolButtonStyle << endl
             << "--> showStatusBar:          " << showStatusBar << endl
             << "--> animatedTrayIcon:       " << animatedTrayIcon;
}

