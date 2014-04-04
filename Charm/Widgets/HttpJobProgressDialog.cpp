#include "HttpJobProgressDialog.h"
#include <QInputDialog>
#include <QLineEdit>

HttpJobProgressDialog::HttpJobProgressDialog( HttpJob* job, QWidget* parent )
    : QProgressDialog(parent)
    , m_job( job )
{
    setLabelText( tr("Wait...") );

    Q_ASSERT(job);
    connect( job, SIGNAL(finished(HttpJob*)), this, SLOT(jobFinished(HttpJob*)) );
    connect( job, SIGNAL(transferStarted()), this, SLOT(jobTransferStarted()) );
    connect( job, SIGNAL(passwordRequested()), this, SLOT(jobPasswordRequested()) );
}

void HttpJobProgressDialog::jobTransferStarted()
{
    show();
}

void HttpJobProgressDialog::jobFinished( HttpJob* )
{
    deleteLater();
}

void HttpJobProgressDialog::jobPasswordRequested()
{
    bool ok;
    QPointer<QObject> that( this ); //guard against destruction while dialog is open
    const QString newpass = QInputDialog::getText( parentWidget(), tr("Password"), tr("Please enter your lotsofcake password"), QLineEdit::Password, m_job->password(), &ok );
    if ( !that )
        return;
    if ( ok ) {
        m_job->provideRequestedPassword( newpass );
    } else {
        m_job->passwordRequestCanceled();
    }
}
