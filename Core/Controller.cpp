#include <QtDebug>

#include "CharmExceptions.h"
#include "SqlRaiiTransactor.h"
#include "Task.h"
#include "Event.h"
#include "CharmConstants.h"
#include "Controller.h"
#include "SqLiteStorage.h"
#include "StorageInterface.h"
#include "CharmCommand.h"
#include "Configuration.h"

Controller::Controller( QObject* parent_ )
    : QObject( parent_ )
    , ControllerInterface()
    , m_storage( 0 )
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
    qDebug() << "Controller::addTask: adding task" << task.id()
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
    qDebug() << "Controller::modifyTask: committing changes to task"
             << task.id();
    // modify the task itself:
    bool result = m_storage->modifyTask( task );
    Q_ASSERT( result ); Q_UNUSED( result );
    if ( ! result ) {
        qDebug() << "Controller::modifyTask: modifyTask failed!";
        return result;
    }

    updateSubscriptionForTask( task );

    if ( result ) {
        emit( taskUpdated( task ) );
    } else {
        qDebug() << "Controller::modifyTask: storing  subscription state failed.";
    }

    return result;
}

bool Controller::deleteTask( const Task& task )
{
    qDebug() << "Controller::deleteTask: deleting task" << task.id();
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
    qDebug() << "Controller::setAllTasks: replacing all tasks";
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
            m_storage = 0;
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
    Q_ASSERT_X( m_storage != 0, "Controller::persistMetaData",
                "No storage interface available" );
    Setting settings[] = {
        { MetaKey_Key_UserName,
          configuration.user.name() },
        { MetaKey_Key_SubscribedTasksOnly,
          QString().setNum( configuration.taskPrefilteringMode ) },
        { MetaKey_Key_TaskTrackerFontSize,
          QString().setNum( configuration.taskTrackerFontSize ) },
        { MetaKey_Key_DurationFormat,
          QString::number( configuration.durationFormat ) },
        { MetaKey_Key_IdleDetection,
          stringForBool( configuration.detectIdling ) },
        { MetaKey_Key_AnimatedTrayIcon,
          stringForBool( configuration.animatedTrayIcon ) },
        { MetaKey_Key_ToolButtonStyle,
          QString().setNum( configuration.toolButtonStyle ) },
        { MetaKey_Key_ShowStatusBar,
          stringForBool( configuration.showStatusBar ) }
    };
    int NumberOfSettings = sizeof settings / sizeof settings[0];

    bool good = true;
    for ( int i = 0; i < NumberOfSettings; ++i ) {
        good = good && m_storage->setMetaData( settings[i].key, settings[i].value );
    }
    Q_ASSERT_X( good, "Controller::persistMetaData", "Controller assumes write "
                "permissions in meta data table if persistMetaData is called" );
    CONFIGURATION.dump();
}

void Controller::provideMetaData( Configuration& configuration)
{
    Q_ASSERT_X( m_storage != 0, "Controller::provideMetaData",
                "No storage interface available" );
    configuration.user.setName( m_storage->getMetaData( MetaKey_Key_UserName ) );

    bool ok;
    const int fontSize = m_storage->getMetaData( MetaKey_Key_TaskTrackerFontSize ).toInt( &ok );
    if ( !ok ) {
        configuration.taskTrackerFontSize = Configuration::TaskTrackerFont_Regular;
    } else {
        switch( fontSize ) {
        case Configuration::TaskTrackerFont_Small:
            configuration.taskTrackerFontSize = Configuration::TaskTrackerFont_Small;
            break;
        case Configuration::TaskTrackerFont_Large:
            configuration.taskTrackerFontSize = Configuration::TaskTrackerFont_Large;
            break;
        default:
        case Configuration::TaskTrackerFont_Regular:
            configuration.taskTrackerFontSize = Configuration::TaskTrackerFont_Regular;
        }
    }

    const int durationFormatValue = m_storage->getMetaData( MetaKey_Key_DurationFormat ).toInt( &ok );
    if ( ok ) {
        switch ( durationFormatValue ) {
        case Configuration::Minutes:
            configuration.durationFormat = Configuration::Minutes;
            break;
        case Configuration::Decimal:
            configuration.durationFormat = Configuration::Decimal;
            break;
        }
    }

    const int taskPrefilteringModeValue = m_storage->getMetaData( MetaKey_Key_SubscribedTasksOnly ).toInt( &ok );
    if ( ok ) {
        switch( taskPrefilteringModeValue ) {
        case Configuration::TaskPrefilter_ShowAll:
            configuration.taskPrefilteringMode = Configuration::TaskPrefilter_ShowAll;
            break;
        case Configuration::TaskPrefilter_CurrentOnly:
            configuration.taskPrefilteringMode = Configuration::TaskPrefilter_CurrentOnly;
            break;
        case Configuration::TaskPrefilter_SubscribedOnly:
            configuration.taskPrefilteringMode = Configuration::TaskPrefilter_SubscribedOnly;
            break;
        case Configuration::TaskPrefilter_SubscribedAndCurrentOnly:
            configuration.taskPrefilteringMode = Configuration::TaskPrefilter_SubscribedAndCurrentOnly;
            break;
        }
    }

    configuration.detectIdling = boolForString(
        m_storage->getMetaData( MetaKey_Key_IdleDetection ) );
    configuration.animatedTrayIcon = boolForString(
        m_storage->getMetaData( MetaKey_Key_AnimatedTrayIcon ) );

    int buttonStyleValue = m_storage->getMetaData( MetaKey_Key_ToolButtonStyle ).toInt( &ok );
    if( ok ) {
        Qt::ToolButtonStyle buttonStyle = static_cast<Qt::ToolButtonStyle> ( buttonStyleValue );
        configuration.toolButtonStyle = buttonStyle;
    } else {
        configuration.toolButtonStyle = Qt::ToolButtonIconOnly;
    }

    configuration.showStatusBar = boolForString(
            m_storage->getMetaData( MetaKey_Key_ShowStatusBar ) );
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
        Q_ASSERT_X( false, "Controller::initializeBackEnd",
                    "Unknown local storage backend type" );
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
    } catch ( XmlSerializationException& e ) {
        qDebug() << "Controller::importDatabaseFromXml: things fucked up:" << e.what();
        return tr( "The Export file is invalid." );
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

#include "Controller.moc"
