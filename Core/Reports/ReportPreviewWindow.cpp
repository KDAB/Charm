#include <QtDebug>
#include <QPrinter>
#include <QPrintDialog>
#include <QTextBrowser>

#include "ReportPreviewWindow.h"

ReportPreviewWindow::ReportPreviewWindow( QWidget* parent )
    : QDialog( parent )
    , m_document( 0 )
{
    m_ui.setupUi( this );
    setAttribute( Qt::WA_DeleteOnClose );
}

ReportPreviewWindow::~ReportPreviewWindow()
{
    delete m_document;
    m_document = 0;
}

void ReportPreviewWindow::setDocument( const QTextDocument* document )
{
    if ( document != 0 ) {
        // we keep a copy, to be able to show different versions of the same document
        m_document = document->clone();
        m_ui.textBrowser->setDocument( m_document );
    } else {
        m_ui.textBrowser->setDocument( 0 );
        delete m_document;
        m_document = 0;
    }
}

void ReportPreviewWindow::on_pushButtonClose_clicked()
{
    close();
}

void ReportPreviewWindow::on_pushButtonPrint_clicked()
{
    QPrinter printer;
    QPrintDialog dialog( &printer, this );

    if ( dialog.exec() ) {
        m_document->print( &printer );
    }
}

#include "ReportPreviewWindow.moc"
