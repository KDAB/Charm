#include <QtDebug>

#include "Core/CharmConstants.h"
#include "ModelConnector.h"
#include "Application.h"
#include "ViewHelpers.h"
#include "CharmReport.h"
#include "Core/CharmDataModel.h"
#include "ReportPreviewWindow.h"

CharmReport::CharmReport( QObject* parent )
    : QObject( parent )
{
}

CharmReport::~CharmReport()
{
}

bool CharmReport::prepare()
{
    return true;
}

void CharmReport::makeReportPreviewWindow()
{
    if ( prepare() && create() ) {
        qDebug() << "CharmReport::makeReportPreviewWindow: making it";
        ReportPreviewWindow* window = new ReportPreviewWindow( &VIEW );
        // FIXME TEMP_REM
        // window->setDocument( report() );
        window->show();
    } else {
        qDebug() << "CharmReport::makeReportPreviewWindow: prepare or create failed" ;
    }
}

int pointSize( double relSize, float scale )
{
    return static_cast<int>( relSize * scale );
}

bool StartsEarlier( EventId firstId, EventId secondId )
{
    const Event& first = DATAMODEL->eventForId( firstId );
    const Event& second = DATAMODEL->eventForId( secondId );
    return first.startDateTime() < second.startDateTime();
}

bool isIndirectParent( const Task& p, const Task& c )
{
    if ( c.parent() == p.id() ) return true;
    if ( c.parent() == 0 ) return false;
    if ( p.parent() == 0 ) return false;

    const TaskTreeItem& item = DATAMODEL->taskTreeItem( p.parent() );
    return isIndirectParent( item.task(), p );
}

#include "CharmReport.moc"
