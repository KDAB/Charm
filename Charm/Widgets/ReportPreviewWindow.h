#ifndef REPORTPREVIEWWINDOW_H
#define REPORTPREVIEWWINDOW_H

#include <QDialog>
#include <QDomDocument>
#include <QScopedPointer>
#include <QTextDocument>

namespace Ui {
    class ReportPreviewWindow;
}

class QPushButton;

class ReportPreviewWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ReportPreviewWindow( QWidget* parent = 0 );
    virtual ~ReportPreviewWindow();

protected:
    void setDocument( const QTextDocument* document );
    QDomDocument createReportTemplate() const;
    QPushButton* saveToXmlButton() const;
    QPushButton* saveToTextButton() const;
    QPushButton* uploadButton() const;

private slots:
    virtual void slotSaveToXml();
    virtual void slotSaveToText();
    virtual void slotPrint();
    virtual void slotUpdate();
    virtual void slotClose();

private:
    QScopedPointer<Ui::ReportPreviewWindow> m_ui;
    QScopedPointer<QTextDocument> m_document;
};

#endif
