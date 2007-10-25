#include <QtDebug>

#include "Task.h"
#include "Event.h"
#include "CharmConstants.h"
#include "Controller.h"
#include "SqLiteStorage.h"
#include "StorageInterface.h"
#include "CharmCommand.h"

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
        emit definedTasks( tasks );
        EventList events = m_storage->getAllEvents();
        emit allEvents( events );
    }
    break;
    case Disconnecting:
    {   // end all events:

// FIXME retry that?
//         while ( !m_activeEvents.isEmpty() )
//             endEvent( m_activeEvents.last().task() );
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

void Controller::persistMetaData( Configuration& configuration )
{
    Q_ASSERT_X( m_storage != 0, "Controller::persistMetaData",
                "No storage interface available" );
    const QString& events( stringForBool( configuration.eventsInLeafsOnly ) );
    const QString& one( stringForBool( configuration.oneEventAtATime ) );
    bool result =
        m_storage->setMetaData( MetaKey_EventsInLeafsOnly, events )
        && m_storage->setMetaData( MetaKey_OneEventAtATime, one );
    Q_ASSERT_X( result, "Controller::persistMetaData", "Controller assumes write "
                "permissions in meta data table if persistMetaData is called" );
}

void Controller::provideMetaData( Configuration& configuration)
{
    Q_ASSERT_X( m_storage != 0, "Controller::provideMetaData",
                "No storage interface available" );
    configuration.oneEventAtATime = boolForString(
        m_storage->getMetaData( MetaKey_OneEventAtATime ) );
    configuration.eventsInLeafsOnly = boolForString(
        m_storage->getMetaData( MetaKey_EventsInLeafsOnly ) );
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
    if ( result ) {
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

StorageInterface* Controller::storage()
{
    return m_storage;
}

#include "Controller.moc"
