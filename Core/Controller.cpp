/*
  Controller.cpp

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2007-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Mirko Boehm <mirko.boehm@kdab.com>
  Author: Frank Osterfeld <frank.osterfeld@kdab.com>
  Author: Olivier JG <olivier.de.gaalon@kdab.com>
  Author: Guillermo A. Amaral <gamaral@kdab.com>

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

#include "Controller.h"
#include "CharmCommand.h"
#include "CharmConstants.h"
#include "CharmExceptions.h"
#include "Configuration.h"
#include "Event.h"
#include "SqLiteStorage.h"
#include "SqlRaiiTransactor.h"
#include "StorageInterface.h"
#include "Task.h"

#include <QtDebug>

Controller::Controller( QObject* parent_ )
    : QObject( parent_ )
    , ControllerInterface()
    , m_storage( nullptr )
{
}

Controller::~Controller()
{
}

Event Controller::makeEvent( const Task& task )
{
    Event event = m_storage->makeEvent();
    Q_ASSERT( event.isValid() );

    event.setTaskId( task.id() );
    if ( m_storage->modifyEvent( event ) ) {
        emit eventAdded( event );
    } else {
        event = Event();
    }
    return event;
}

Event Controller::cloneEvent(const Event &e)
{
    Event event = m_storage->makeEvent();
    Q_ASSERT( event.isValid() );

    int id = event.id();
    event = e;
    event.setId(id);
    if ( m_storage->modifyEvent( event ) ) {
        emit eventAdded( event );
    } else {
        event = Event();
    }
    return event;
}

bool Controller::modifyEvent( const Event& e )
{
    if ( m_storage->modifyEvent( e ) )
    {
        emit eventModified( e );
        return true;
    } else {
        return false;
    }
}

bool Controller::deleteEvent( const Event& e )
{
    if ( m_storage->deleteEvent( e ) ) {
        emit eventDeleted( e );
        return true;
    } else {
        return false;
    }
}

bool Controller::addTask( const Task& task )
{
    qDebug() << Q_FUNC_INFO << "adding task" << task.id()
             << "to parent" << task.parent();
    if ( m_storage->addTask( task ) ) {
        updateSubscriptionForTask( task );
        emit taskAdded( task );

        return true;
    } else {
        Q_ASSERT( false ); // impossible
        return false;
    }
}

bool Controller::modifyTask( const Task& task )
{
    // find it
    qDebug() << Q_FUNC_INFO << "committing changes to task"
             << task.id();
    // modify the task itself:
    bool result = m_storage->modifyTask( task );
    Q_ASSERT( result );
    if ( ! result ) {
        qDebug() << Q_FUNC_INFO << "modifyTask failed!";
        return result;
    }

    updateSubscriptionForTask( task );
    emit( taskUpdated( task ) );

    return true;
}

bool Controller::deleteTask( const Task& task )
{
    qDebug() << Q_FUNC_INFO << "deleting task" << task.id();
    if ( m_storage->deleteTask( task ) ) {
        m_storage->deleteSubscription( CONFIGURATION.user, task );
        emit taskDeleted( task );
        return true;
    } else {
        Q_ASSERT( false ); // impossible
        return false;
    }
}

bool Controller::setAllTasks( const TaskList& tasks )
{
    qDebug() << Q_FUNC_INFO << "replacing all tasks";
    if ( m_storage->setAllTasks( CONFIGURATION.user, tasks ) ) {
        const TaskList newTasks = m_storage->getAllTasks();
        // tell the view about the existing tasks;
        emit definedTasks( newTasks );
        return true;
    } else {
        return false;
    }
}

void Controller::updateSubscriptionForTask( const Task& task )
{
    if ( task.subscribed() ) {
        bool result = m_storage->addSubscription( CONFIGURATION.user, task );
        Q_ASSERT( result ); Q_UNUSED( result );
    } else {
        bool result = m_storage->deleteSubscription( CONFIGURATION.user, task );
        Q_ASSERT( result ); Q_UNUSED( result );
    }
}

void Controller::stateChanged( State previous, State next )
{
    Q_UNUSED( previous );

    switch( next ) {
    case Connected:
    {   // yes, it is that simple:
        TaskList tasks = m_storage->getAllTasks();
        // tell the view about the existing tasks;
        if ( ! Task::checkForUniqueTaskIds( tasks ) ) {
            throw CharmException( tr( "The Charm database is corrupted, it contains duplicate task ids. "
                                      "Please have it looked after by a professional." ) );
        }
        if ( ! Task::checkForTreeness( tasks ) ) {
            throw CharmException( tr( "The Charm database is corrupted, the tasks do not form a tree. "
                                      "Please have it looked after by a professional." ) );
        }
        emit definedTasks( tasks );
        EventList events = m_storage->getAllEvents();
        emit allEvents( events );
    }
    break;
    case Disconnecting:
    {
        emit readyToQuit();
        if ( m_storage ) {
// this will still leave Qt complaining about a repeated connection
//         qDebug() << "Application::enterConnectingState: closing existing storage interface";
            m_storage->disconnect();
            delete m_storage;
            m_storage = nullptr;
        }
    }
    break;
    default:
        break;
    };

    if ( m_storage ) {
        emit currentBackendStatus( m_storage->description() );
        m_storage->stateChanged( previous );
    }
}

struct Setting {
    QString key;
    QString value;
};

void Controller::persistMetaData( Configuration& configuration )
{
    Q_ASSERT_X( m_storage != nullptr, Q_FUNC_INFO, "No storage interface available" );
    Setting settings[] = {
        { MetaKey_Key_UserName,
          configuration.user.name() },
        { MetaKey_Key_SubscribedTasksOnly,
          QString().setNum( configuration.taskPrefilteringMode ) },
        { MetaKey_Key_TimeTrackerFontSize,
          QString().setNum( configuration.timeTrackerFontSize ) },
        { MetaKey_Key_DurationFormat,
          QString::number( configuration.durationFormat ) },
        { MetaKey_Key_IdleDetection,
          stringForBool( configuration.detectIdling ) },
        { MetaKey_Key_WarnUnuploadedTimesheets,
          stringForBool( configuration.warnUnuploadedTimesheets ) },
        { MetaKey_Key_RequestEventComment,
          stringForBool( configuration.requestEventComment ) },
        { MetaKey_Key_ToolButtonStyle,
          QString().setNum( configuration.toolButtonStyle ) },
        { MetaKey_Key_ShowStatusBar,
          stringForBool( configuration.showStatusBar ) },
        { MetaKey_Key_EnableCommandInterface,
          stringForBool( configuration.enableCommandInterface ) }
    };
    int NumberOfSettings = sizeof settings / sizeof settings[0];

    bool good = true;
    for ( int i = 0; i < NumberOfSettings; ++i ) {
        good = good && m_storage->setMetaData( settings[i].key, settings[i].value );
    }
    Q_ASSERT_X( good, Q_FUNC_INFO, "Controller assumes write "
                "permissions in meta data table if persistMetaData is called" );
    CONFIGURATION.dump();
}

template<class T>
void Controller::loadConfigValue( const QString& key, T& configValue ) const
{
    const QString storedValue = m_storage->getMetaData( key );
    if ( storedValue.isNull() )
        return;
    configValue = strToT<T>( storedValue );
}

void Controller::provideMetaData( Configuration& configuration)
{
    Q_ASSERT_X( m_storage != nullptr, Q_FUNC_INFO, "No storage interface available" );
    configuration.user.setName( m_storage->getMetaData( MetaKey_Key_UserName ) );

    loadConfigValue( MetaKey_Key_TimeTrackerFontSize, configuration.timeTrackerFontSize );
    loadConfigValue( MetaKey_Key_DurationFormat, configuration.durationFormat );
    loadConfigValue( MetaKey_Key_SubscribedTasksOnly, configuration.taskPrefilteringMode );
    loadConfigValue( MetaKey_Key_IdleDetection, configuration.detectIdling );
    loadConfigValue( MetaKey_Key_WarnUnuploadedTimesheets, configuration.warnUnuploadedTimesheets );
    loadConfigValue( MetaKey_Key_RequestEventComment, configuration.requestEventComment );
    loadConfigValue( MetaKey_Key_ToolButtonStyle, configuration.toolButtonStyle );
    loadConfigValue( MetaKey_Key_ShowStatusBar, configuration.showStatusBar );
    loadConfigValue( MetaKey_Key_EnableCommandInterface, configuration.enableCommandInterface );

    CONFIGURATION.dump();
}

bool Controller::initializeBackEnd( const QString& name )
{
    // make storage interface according to configuration
    // this is our local storage backend factory and may have to be
    // factored out into a factory method (now that is some serious
    // refucktoring):
    if ( name == CHARM_SQLITE_BACKEND_DESCRIPTOR )
    {
        m_storage = new SqLiteStorage;
        return true;
    } else {
        Q_ASSERT_X( false, Q_FUNC_INFO, "Unknown local storage backend type" );
        return false;
    }
}

bool Controller::connectToBackend()
{
    bool result = m_storage->connect( CONFIGURATION );

    // the user id in the database, and the installation id, do not
    // have to be 1 and 1, as we have guessed --> persist configuration
    if ( result && ! CONFIGURATION.newDatabase ) {
        provideMetaData( CONFIGURATION );
    }

    return result;
}

bool Controller::disconnectFromBackend()
{
    return m_storage->disconnect();
}

void Controller::executeCommand( CharmCommand* command )
{
    command->execute( this );
    // send it back to the view:
    emit commandCompleted( command );
}

void Controller::rollbackCommand( CharmCommand* command )
{
    command->rollback( this );
    // send it back to the view:
    emit commandCompleted( command );
}


StorageInterface* Controller::storage()
{
    return m_storage;
}

const QString MetaDataElement ( "metadata" );
const QString ExportRootElement( "charmdatabase" );
const QString VersionElement( "version" );
const QString TasksElement( "tasks" );
const QString EventsElement( "events" );

QDomDocument Controller::exportDatabasetoXml() const
{
    QDomDocument document( "charmdatabase" );
    // root element:
    QDomElement root = document.createElement( ExportRootElement );
    root.setAttribute( VersionElement, CHARM_DATABASE_VERSION );
    document.appendChild( root );
    // metadata:
    QDomElement metadata = document.createElement( MetaDataElement );
    // I am not so sure what kind of metadata needs to be stored
    root.appendChild( metadata );
    // tasks element:
    QDomElement tasksElement = document.createElement( TasksElement );
    // FIXME there are generic methods for that now, in Task.h
    TaskList tasks = m_storage->getAllTasks();
    Q_FOREACH( Task task, tasks ) {
        QDomElement element = task.toXml( document );
        tasksElement.appendChild( element );
    }
    root.appendChild( tasksElement );
    // events element:
    QDomElement eventsElement = document.createElement( EventsElement );
    EventList events = m_storage->getAllEvents();
    Q_FOREACH( Event event, events ) {
        QDomElement element = event.toXml( document );
        eventsElement.appendChild( element );
    }
    root.appendChild( eventsElement );
    // qDebug() << document.toString( 4 );
    return document;
}

class MakeSureTheModelIsUpdated {
public:
    explicit MakeSureTheModelIsUpdated( Controller* controller )
        : m_controller( controller )
    {}

    ~MakeSureTheModelIsUpdated()
    {
        // now tell the data model that things have changed:
        m_controller->updateModelEventsAndTasks();
    }

private:
    Controller* m_controller;
};

QString Controller::importDatabaseFromXml( const QDomDocument& document )
{
    MakeSureTheModelIsUpdated m( this );

    // first, parse the XML document, and break if there is an error
    // (not touching the DB contents):
    TaskList importedTasks;
    EventList importedEvents;
    int databaseSchemaVersion;

    // FIXME test for the file to be a database export, not (for example) a task definitions export
    try {
        QDomElement rootElement = document.documentElement();
        bool ok;
        databaseSchemaVersion = rootElement.attribute( "version" ).toInt( &ok );
        if ( !ok ) throw XmlSerializationException( QObject::tr( "Syntax error, no version attribute found." ) );

        QDomElement metadataElement = rootElement.firstChildElement( MetaDataElement );
        QDomElement tasksElement = rootElement.firstChildElement( TasksElement );
        for ( QDomElement element = tasksElement.firstChildElement( Task::tagName() );
              !element.isNull(); element = element.nextSiblingElement( Task::tagName() ) ) {
            Task task = Task::fromXml( element, databaseSchemaVersion );
            if ( ! task.isValid() ) {
                qDebug() << "The following task is invalid and will not be added:";
                task.dump();
                // return tr( "The Export file contains at least one invalid task." );
            } else {
                importedTasks.append( task );
            }
        }
        QDomElement eventsElement = rootElement.firstChildElement( EventsElement );
        for ( QDomElement element = eventsElement.firstChildElement( Event::tagName() );
              !element.isNull(); element = element.nextSiblingElement( Event::tagName() ) ) {
            Event event = Event::fromXml( element, databaseSchemaVersion );
            if ( ! event.isValid() ) {
                qDebug() << "The following event is invalid and will not be added:";
                event.dump();
                // return tr( "The Export file contains at least one invalid event." );
            } else {
                importedEvents.append( event );
            }
        }
    } catch ( const XmlSerializationException& e ) {
        qDebug() << "Controller::importDatabaseFromXml: things fucked up:" << e.what();
        return tr( "The export file is invalid: %1" ).arg( e.what() );
    }

    const QString error = m_storage->setAllTasksAndEvents( CONFIGURATION.user, importedTasks, importedEvents );
    if( !error.isEmpty() ) {
        // the database should be unchanged, and the model will update on return
        return tr( "Error importing tasks and events from the file:<br />%1" )
                .arg( error );
    }

    // FIXME needed?

    // tell the model that the tasks and events have vanished:
    emit allEvents( EventList() );
    emit definedTasks( TaskList() );

    return QString();
}

void Controller::updateModelEventsAndTasks()
{
    TaskList tasks = m_storage->getAllTasks();
    // tell the view about the existing tasks;
    emit definedTasks( tasks );
    EventList events = m_storage->getAllEvents();
    emit allEvents( events );
}

#include "moc_Controller.cpp"
