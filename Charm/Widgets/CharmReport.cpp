#include <QtDebug>

#include "Core/CharmConstants.h"
#include "ModelConnector.h"
#include "ApplicationCore.h"
#include "ViewHelpers.h"
#include "CharmReport.h"
#include "Core/CharmDataModel.h"
#include "Widgets/ReportPreviewWindow.h"

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

#include "moc_CharmReport.cpp"
