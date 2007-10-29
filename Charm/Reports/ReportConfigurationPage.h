#ifndef REPORTCONFIGURATIONPAGE_H
#define REPORTCONFIGURATIONPAGE_H

#include <QWidget>

#include "ReportDialog.h"

class QDialog;

/** ReportConfigurationPage is the base class for report configuration
    pages.
    The report dialog offers the user on the first page to select the
    report type. When the user hits "next", the respective
    ReportConfigurationPage will be shown. All settings made by the
    user will be saved in the ReportConfigurationPage, and it will
    generate a report preview page according to the settings on
    request. */
class ReportConfigurationPage : public QWidget
{
    Q_OBJECT

public:
    explicit ReportConfigurationPage( ReportDialog* parent = 0 );

    /** makeReportPreviewDialog will generate a report preview dialgo
        that follows the  settings made by the user. The dialog is
        supposed to destroy-on-close and  */
    virtual QDialog* makeReportPreviewDialog( QWidget* parent = 0 ) = 0;
    virtual QString name() = 0;
    virtual QString description() = 0;

signals:
    /** accept() is supposed to tell the report dialog to close and
        accept. It is connected to the report dialogs accept slot in
        the constructor. */
    void accept();
    /** back() tells the report dialog to go back to the start
        page. It is connected to the report dialogs back button in the
        constructor. */
    void back();
};

#endif
