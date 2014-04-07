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
