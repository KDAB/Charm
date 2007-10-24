#ifndef REPORTPREVIEWWINDOW_H
#define REPORTPREVIEWWINDOW_H

#include <QDialog>
#include <QTextDocument>

#include "ui_ReportPreviewWindow.h"

class ReportPreviewWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ReportPreviewWindow( QWidget* parent = 0 );
    ~ReportPreviewWindow();

    void setDocument( const QTextDocument* document );

public slots:
    void on_pushButtonClose_clicked();
    void on_pushButtonPrint_clicked();

private:
    Ui::ReportPreviewWindow m_ui;
    QTextDocument* m_document;
};

#endif
