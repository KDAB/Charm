#ifndef REPORTCONFIGURATIONDIALOG_H
#define REPORTCONFIGURATIONDIALOG_H

#include <QDialog>

/** Base class for report configuration dialogs. */
class ReportConfigurationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ReportConfigurationDialog( QWidget* parent = 0 );

    /** generates a report preview dialog that follows the
        settings made by the user. The dialog is supposed to destroy-on-close and non-modal.

        @param parent parent widget for the preview dialog
     */
    virtual QDialog* makeReportPreviewDialog( QWidget* parent ) = 0;
};

#endif
