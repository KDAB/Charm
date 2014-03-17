#include "Timesheet.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>

#include "ViewHelpers.h"

#include "CharmCMake.h"

TimeSheetReport::TimeSheetReport( QWidget* parent )
    : ReportPreviewWindow( parent )
    , m_rootTask( 0 )
    , m_activeTasksOnly( false )
{
}

TimeSheetReport::~TimeSheetReport()
{
}

void TimeSheetReport::setReportProperties(
    const QDate& start, const QDate& end,
    TaskId rootTask, bool activeTasksOnly )
{
    m_start = start;
    m_end = end;
    m_rootTask = rootTask;
    m_activeTasksOnly = activeTasksOnly;
    update();
}

void TimeSheetReport::slotUpdate()
{
    update();
}

void TimeSheetReport::slotSaveToXml()
{
    qDebug() << "TimeSheet::slotSaveToXml: creating XML time sheet";
    // first, ask for a file name:
    QString filename = getFileName( tr("Charm reports (*.charmreport)") );
    if (filename.isEmpty())
        return;

    QFileInfo fileinfo( filename );
    if ( fileinfo.suffix().isEmpty() ) {
        filename += QLatin1String( ".charmreport" );
    }

    QByteArray payload = saveToXml();
    if (payload.isEmpty())
        return; // Error should have been already displayed by saveToXml()

    QFile file( filename );
    if ( file.open( QIODevice::WriteOnly ) ) {
        file.write( payload );
    } else {
        QMessageBox::critical( this, tr( "Error saving report" ),
                               tr( "Cannot write to selected location:\n%1" ).arg( file.errorString() ) );
    }
}

void TimeSheetReport::slotSaveToText()
{
    qDebug() << "TimeSheet::slotSaveToText: creating text file with totals";
    // first, ask for a file name:
    const QString filename = getFileName( "Text files (*.txt)" );
    if (filename.isEmpty())
        return;

    QFile file( filename );
    if ( !file.open( QIODevice::WriteOnly ) ) {
        QMessageBox::critical( this, tr( "Error saving report" ),
                               tr( "Cannot write to selected location:\n%1" )
                               .arg( file.errorString() ) );
        return;
    }
    file.write( saveToText() );
    file.close();
}

QString TimeSheetReport::getFileName( const QString& filter )
{
    QSettings settings;
    QString path;
    if ( settings.contains( MetaKey_ReportsRecentSavePath ) ) {
        path = settings.value( MetaKey_ReportsRecentSavePath ).toString();
        QDir dir( path );
        if ( !dir.exists() ) path = QString();
    }
    // suggest file name:
    path += QDir::separator() + suggestedFileName();
    // ask:
    QString filename = QFileDialog::getSaveFileName( this, tr( "Enter File Name" ), path, filter );
    if ( filename.isEmpty() )
        return QString();
    QFileInfo fileinfo( filename );
    path = fileinfo.absolutePath();
    if ( !path.isEmpty() ) {
        settings.setValue( MetaKey_ReportsRecentSavePath, path );
    }
    return filename;
}

/*********************************************************************** TimeSheetInfo */

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
    qDebug() << "TimeSheetInfo: (" << indentation << ")" << taskname << ":" << seconds << "-" << total() << "total";
}

// make the list, aggregate the seconds in the subtask:
TimeSheetInfoList TimeSheetInfo::taskWithSubTasks( int segments, TaskId id,
    const TimeSheetReport::SecondsMap& secondsMap,
    TimeSheetInfo* addTo )
{
    TimeSheetInfoList result;
    TimeSheetInfoList children;

    TimeSheetInfo myInformation(segments);
    const TaskTreeItem& item = DATAMODEL->taskTreeItem( id );
    // real task or virtual root item
    Q_ASSERT( item.task().isValid() || id == 0 );

    if ( id != 0 ) {
        // add totals for task itself:
        if ( secondsMap.contains( id ) ) {
            myInformation.seconds = secondsMap.value(id);
        }
        // add name:
        QTextStream stream( &myInformation.taskname );
        stream << QString("%1" ).arg(
            item.task().id(),
            CONFIGURATION.taskPaddingLength,
            10, QChar( '0' ) )
               << ": " << item.task().name();

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
    Q_FOREACH( const TaskId i, childIds ) {
        children << taskWithSubTasks( segments, i, secondsMap, &myInformation );
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
