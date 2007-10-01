#include <QtDebug>
#include <QSettings>

#include "Configuration.h"

// QSETTINGS KEYS

const char Settings_Key_InstallationId[] = "InstallationId";
const char Settings_Key_UserId[] = "UserId";
const char Settings_Key_LocalStorageDatabase[] = "LocalStorageDatabase";
const char Settings_Key_LocalStorageType[] = "LocalStorageType";
const char Settings_Key_SubscribedTasksOnly[] = "SubscribedTasksOnly";
const char Settings_Key_TaskTrackerFontSize[] = "TaskTrackerFontSize";
const char Settings_Key_24hEditing[] = "Key24hEditing";

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
    : eventsInLeafsOnly( true )
    , oneEventAtATime( true )
    , configurationName( DEFAULT_CONFIG_GROUP )
    , installationId( 0 )
    , newDatabase( false )
    , failure( false )
    , taskPaddingLength( 6 ) // arbitrary
    , showOnlySubscribedTasks( false )
    , taskTrackerFontSize( TaskTrackerFont_Regular )
    , always24hEditing( false )
{
}

void Configuration::writeTo( QSettings& settings )
{
    settings.setValue( Settings_Key_InstallationId, installationId );
    settings.setValue( Settings_Key_UserId, user.id() );
    settings.setValue( Settings_Key_LocalStorageType, localStorageType );
    settings.setValue( Settings_Key_LocalStorageDatabase, localStorageDatabase );
    settings.setValue( Settings_Key_SubscribedTasksOnly, showOnlySubscribedTasks );
    settings.setValue( Settings_Key_TaskTrackerFontSize, static_cast<int>( taskTrackerFontSize ) );
    settings.setValue( Settings_Key_24hEditing, always24hEditing );
    dump( "(Configuration::writeTo stored configuration)" );
}

bool Configuration::readFrom( QSettings& settings )
{
    bool complete = true;
    if ( settings.contains( Settings_Key_InstallationId ) ) {
        installationId = settings.value( Settings_Key_InstallationId ).toInt();
    } else {
        complete = false;
    }
    if ( settings.contains( Settings_Key_UserId ) ) {
        user.setId( settings.value( Settings_Key_UserId ).toInt() );
    } else {
        complete = false;
    }
    if ( settings.contains( Settings_Key_LocalStorageType ) ) {
        localStorageType = settings.value( Settings_Key_LocalStorageType ).toString();
    } else {
        complete = false;
    }
    if ( settings.contains( Settings_Key_LocalStorageDatabase ) ) {
        localStorageDatabase = settings.value( Settings_Key_LocalStorageDatabase ).toString();
    } else {
        complete = false;
    }
    // ----- optional settings:
    if ( settings.contains( Settings_Key_SubscribedTasksOnly ) ) {
        showOnlySubscribedTasks = settings.value( Settings_Key_SubscribedTasksOnly ).value<bool>();
    }
    if ( settings.contains( Settings_Key_TaskTrackerFontSize ) ) {
        int setting = settings.value( Settings_Key_TaskTrackerFontSize ).value<int>();
        taskTrackerFontSize = static_cast<TaskTrackerFontSize>( setting );
    }
    if ( settings.contains( Settings_Key_24hEditing ) ) {
        always24hEditing = settings.value( Settings_Key_24hEditing ).value<bool>();
    }

    dump( "(Configuration::readFrom loaded configuration)" );
    return complete;
}

void Configuration::dump( const QString& why )
{
    // dump configuration:
    return; // disable debug output
    qDebug() << "Application::enterStartingUpState: configuration:"
             << ( why.isEmpty() ? QString() : why )
             << endl
             << "--> installation id:        " << installationId << endl
             << "--> userid:                 " << user.id() << endl
             << "--> local storage type:     " << localStorageType << endl
             << "--> local storage database: " << localStorageDatabase << endl
             << "--> subscribed tasks only:  " << showOnlySubscribedTasks << endl
             << "--> task tracker font size: " << taskTrackerFontSize
             << "--> 24h time editing:       " << always24hEditing;
}

