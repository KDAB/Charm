#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QObject>

#include "User.h"

class QSettings;

class Configuration
{
public:
    // only append to that, to no break old configurations:
    enum TaskTrackerFontSize {
        TaskTrackerFont_Small,
        TaskTrackerFont_Regular,
        TaskTrackerFont_Large
    };

    enum TaskPrefilteringMode {
        TaskPrefilter_ShowAll,
        TaskPrefilter_CurrentOnly,
        TaskPrefilter_SubscribedOnly,
        TaskPrefilter_SubscribedAndCurrentOnly,
        TaskPrefilter_NumberOfModes
    };

    enum DurationFormat {
        Minutes=0,
        Decimal
    };

    bool operator== ( const Configuration& other ) const;

    static Configuration& instance();

    void writeTo( QSettings& );
    // read the configuration from the settings object
    // this will not modify the settings group etc but just use it
    // returns true if all individual settings have been found (the
    // configuration is complete)
    bool readFrom( QSettings& );

    // helper method
    void dump( const QString& why = QString::null );

    User user;  // this user's id
    TaskPrefilteringMode taskPrefilteringMode;
    TaskTrackerFontSize taskTrackerFontSize;
    DurationFormat durationFormat;
    Qt::ToolButtonStyle toolButtonStyle;
    bool showStatusBar;
    bool detectIdling;
    bool animatedTrayIcon;

    // these are stored in QSettings, since we need this information to locate and open the database:
    QString configurationName;
    int installationId;
    QString localStorageType; // SqLite, MySql, ...
    QString localStorageDatabase; // database name (path, with sqlite)
    bool newDatabase; // true if the configuration has just been created
    bool failure; // used to reconfigure on failures
    QString failureMessage; // a message to show the user if something is wrong with the configuration

    // appearance properties
    int taskPaddingLength; // auto-determined
private:
    // allow test classes to create configuration objects (tests are
    // the only  application that can have (test) multiple
    // configurations):
    friend class SqLiteStorageTests;
    friend class ControllerTests;
    // these are all the persisted metadata settings, and the constructor is only used during test runs:
    Configuration( bool eventsInLeafsOnly, bool oneEventAtATime, User user,
                   TaskPrefilteringMode taskPrefilteringMode, TaskTrackerFontSize,
                   DurationFormat durationFormat, bool detectIdling,
                   Qt::ToolButtonStyle buttonstyle, bool showStatusBar, bool animatedTrayIcon );
    Configuration();
};


#endif
