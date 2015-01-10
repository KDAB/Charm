/*
  Configuration.cpp

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2007-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Mirko Boehm <mirko.boehm@kdab.com>
  Author: Frank Osterfeld <frank.osterfeld@kdab.com>
  Author: Mike McQuaid <mike.mcquaid@kdab.com>

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

#include "Configuration.h"
#include "CharmConstants.h"

#include <QtDebug>
#include <QSettings>

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
    : taskPrefilteringMode( TaskPrefilter_ShowAll )
    , timeTrackerFontSize( TimeTrackerFont_Regular )
    , durationFormat( Minutes )
    , toolButtonStyle( Qt::ToolButtonFollowStyle )
    , showStatusBar( true )
    , detectIdling( true )
    , warnUnuploadedTimesheets( true )
    , requestEventComment( false )
    , enableCommandInterface( false )
    , configurationName( DEFAULT_CONFIG_GROUP )
    , installationId( 0 )
    , newDatabase( false )
    , failure( false )
    , taskPaddingLength( 6 ) // arbitrary
{
}

Configuration::Configuration( bool _eventsInLeafsOnly, bool _oneEventAtATime, User _user,
                              TaskPrefilteringMode _taskPrefilteringMode,
                              TimeTrackerFontSize _timeTrackerFontSize,
                              DurationFormat _durationFormat, bool _detectIdling,
                              Qt::ToolButtonStyle _buttonstyle, bool _showStatusBar,
                              bool _warnUnuploadedTimesheets, bool _requestEventComment,
                              bool _enableCommandInterface )
    : taskPrefilteringMode( _taskPrefilteringMode )
    , timeTrackerFontSize( _timeTrackerFontSize )
    , durationFormat( _durationFormat )
    , toolButtonStyle( _buttonstyle )
    , showStatusBar( _showStatusBar )
    , detectIdling ( _detectIdling )
    , warnUnuploadedTimesheets( _warnUnuploadedTimesheets )
    , requestEventComment( _requestEventComment )
    , enableCommandInterface( _enableCommandInterface )
    , configurationName( DEFAULT_CONFIG_GROUP )
    , installationId( 0 )
    , newDatabase( false )
    , failure( false )
    , taskPaddingLength( 6 ) // arbitrary
{
}

bool Configuration::operator==( const Configuration& other ) const
{
    return user == other.user &&
        taskPrefilteringMode == other.taskPrefilteringMode &&
        timeTrackerFontSize == other.timeTrackerFontSize &&
        durationFormat == other.durationFormat &&
        detectIdling == other.detectIdling &&
        warnUnuploadedTimesheets == other.warnUnuploadedTimesheets &&
        requestEventComment == other.requestEventComment &&
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
             << "--> installation id:          " << installationId << endl
             << "--> userid:                   " << user.id() << endl
             << "--> local storage type:       " << localStorageType << endl
             << "--> local storage database:   " << localStorageDatabase << endl
             << "--> task prefiltering mode:   " << taskPrefilteringMode << endl
             << "--> task tracker font size:   " << timeTrackerFontSize << endl
             << "--> duration format:          " << durationFormat << endl
             << "--> Idle Detection:           " << detectIdling << endl
             << "--> toolButtonStyle:          " << toolButtonStyle << endl
             << "--> showStatusBar:            " << showStatusBar << endl
             << "--> warnUnuploadedTimesheets: " << warnUnuploadedTimesheets << endl
             << "--> requestEventComment:      " << requestEventComment << endl
             << "--> enableCommandInterface:   " << enableCommandInterface;
}
