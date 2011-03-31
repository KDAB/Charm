#ifndef REPORTPREVIEWWINDOW_H
#define REPORTPREVIEWWINDOW_H

#include <QDialog>
#include <QDomDocument>
#include <QTextDocument>

namespace Ui {
    class ReportPreviewWindow;
}

class ReportPreviewWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ReportPreviewWindow( QWidget* parent = 0 );
    virtual ~ReportPreviewWindow();

protected:
    void setDocument( const QTextDocument* document );
    QDomDocument createReportTemplate();

private slots:
    virtual void slotSaveToXml();
    virtual void slotSaveToText();
    virtual void slotPrint();
    virtual void slotUpdate();
    virtual void slotClose();

private:
    Ui::ReportPreviewWindow* m_ui;
    QTextDocument* m_document;
};

#endif
