#include <QtAlgorithms>

#include "ViewHelpers.h"

void connectControllerAndView( Controller* controller, CharmWindow* view )
{
    // connect view and controller:
    // make controller process commands send by the view:
    QObject::connect( view, SIGNAL( emitCommand( CharmCommand* ) ),
                      controller, SLOT( executeCommand( CharmCommand* ) ) );
    QObject::connect( view, SIGNAL( emitCommandRollback( CharmCommand* ) ),
                      controller, SLOT( rollbackCommand( CharmCommand* ) ) );
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

QString elidedTaskName( const QString& text, const QFont& font, int width )
{
    QFontMetrics metrics( font );
    const QString& projectCode =
            text.section( ' ', 0, 0, QString::SectionIncludeTrailingSep );
    const int projectCodeWidth = metrics.width( projectCode );
    if ( width > projectCodeWidth ) {
        const QString& taskName = text.section( ' ', 1 );
        const int taskNameWidth = width - projectCodeWidth;
        const QString& taskNameElided =
                metrics.elidedText( taskName, Qt::ElideLeft, taskNameWidth );
        return projectCode + taskNameElided;
    }

    return metrics.elidedText( text, Qt::ElideMiddle, width );
}
