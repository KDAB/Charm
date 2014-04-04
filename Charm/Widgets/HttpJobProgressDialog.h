#ifndef HTTPJOBPROGRESSDIALOG_H
#define HTTPJOBPROGRESSDIALOG_H

#include <QProgressDialog>
#include <QPointer>
#include "HttpClient/HttpJob.h"

class HttpJobProgressDialog : public QProgressDialog
{
    Q_OBJECT
public:
    explicit HttpJobProgressDialog( HttpJob* job, QWidget* parent = 0 );

private Q_SLOTS:
    void jobFinished( HttpJob* );
    void jobTransferStarted();
    void jobPasswordRequested();

private:
    QPointer<HttpJob> m_job;
};

#endif
