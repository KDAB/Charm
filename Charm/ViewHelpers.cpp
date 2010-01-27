#include <QtAlgorithms>

#include "ViewHelpers.h"

void connectControllerAndView( Controller* controller, CharmWindow* view )
{
    // connect view and controller:
    // make controller process commands send by the view:
    QObject::connect( view, SIGNAL( emitCommand( CharmCommand* ) ),
                      controller, SLOT( executeCommand( CharmCommand* ) ) );
    // make view receive done commands from the controller:
    QObject::connect( controller, SIGNAL( commandCompleted( CharmCommand* ) ),
                      view, SLOT( commitCommand( CharmCommand* ) ) );
}

bool startsEarlier(const EventId& leftId, const EventId& rightId )
{
    const Event& left = DATAMODEL->eventForId( leftId );
    const Event& right = DATAMODEL->eventForId( rightId );
    return left.startDateTime() < right.startDateTime();
}

EventIdList eventIdsSortedByStartTime( EventIdList ids )
{
    qStableSort( ids.begin(), ids.end(), startsEarlier );
    return ids;
}

EventIdList filteredBySubtree( EventIdList ids, TaskId parent, bool exclude )
{
    EventIdList result;
    bool isParent = false;
    Q_FOREACH( EventId id, ids ) {
        const Event& event = DATAMODEL->eventForId( id );
        isParent = ( parent == event.taskId() || DATAMODEL->isParentOf( parent, event.taskId() ) );
        if ( isParent != exclude ) {
            result << id;
        }
    }
    return result;
}

