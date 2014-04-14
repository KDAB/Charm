#include "TimesheetInfo.h"

#include "Core/CharmDataModel.h"

TimeSheetInfo::TimeSheetInfo(int segments)
  : indentation( 0 )
  , seconds( segments )
  , taskId( 0 )
  , aggregated( false )
{
    seconds.fill( 0 );
}

int TimeSheetInfo::total() const
{
    int value = 0;
    for ( int i = 0; i < seconds.size(); ++i )
        value += seconds[i];
    return value;
}

void TimeSheetInfo::dump()
{
    qDebug() << "TimeSheetInfo: (" << indentation << ")" << formattedTaskIdAndName( 6 ) << ":" << seconds << "-" << total() << "total";
}

QString TimeSheetInfo::formattedTaskIdAndName( int taskPaddingLength ) const
{
    const QString formattedId = QString::fromLatin1( "%1" ).arg( taskId, taskPaddingLength, 10, QChar( '0' ) );
    return QString::fromLatin1("%1: %2").arg( formattedId, taskName );
}

// make the list, aggregate the seconds in the subtask:
TimeSheetInfoList TimeSheetInfo::taskWithSubTasks( const CharmDataModel* dataModel, int segments, TaskId id,
    const SecondsMap& secondsMap,
    TimeSheetInfo* addTo )
{
    TimeSheetInfoList result;
    TimeSheetInfoList children;

    TimeSheetInfo myInformation(segments);
    const TaskTreeItem& item = dataModel->taskTreeItem( id );
    // real task or virtual root item
    Q_ASSERT( item.task().isValid() || id == 0 );

    if ( id != 0 ) {
        // add totals for task itself:
        if ( secondsMap.contains( id ) ) {
            myInformation.seconds = secondsMap.value(id);
        }
        // add name and id:
        myInformation.taskId = item.task().id();
        myInformation.taskName = item.task().name();

        if ( addTo != 0 ) {
            myInformation.indentation = addTo->indentation + 1;
        }
        myInformation.taskId = id;
    } else {
        myInformation.indentation = -1;
    }

    TaskIdList childIds = item.childIds();
    // sort by task id
    qSort( childIds );
    // recursively add those to myself:
    Q_FOREACH ( const TaskId i, childIds ) {
        children << taskWithSubTasks( dataModel, segments, i, secondsMap, &myInformation );
    }

    // add to parent:
    if ( addTo != 0 ) {
        for ( int i = 0; i < segments; ++i )
        {
            addTo->seconds[i] += myInformation.seconds[i];
        }
        addTo->aggregated = true;
    }

    result << myInformation << children;

    return result;
}

// retrieve events that match the settings (active, ...):
TimeSheetInfoList TimeSheetInfo::filteredTaskWithSubTasks(
    TimeSheetInfoList timeSheetInfo,
    bool activeTasksOnly )
{
    if ( activeTasksOnly ) {
        TimeSheetInfoList nonZero;
        // FIXME use algorithm (I just hate to lug the fat book around)
        for ( int i = 0; i < timeSheetInfo.size(); ++i )
        {
            if ( timeSheetInfo[i].total() > 0 ) {
                nonZero << timeSheetInfo[i];
            }
        }
        timeSheetInfo = nonZero;
    }

    return timeSheetInfo;
}
