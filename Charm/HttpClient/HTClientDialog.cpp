#include "HTClientDialog.h"

#include <QCloseEvent>
#include <QTimer>

#include "HTClient.h"
#include "ui_HTClientDialog.h"

#define TIMEOUT 2000

HTClientDialog::HTClientDialog(HTClient &client)
    : QDialog(0)
    , m_client( client )
    , m_ui( new Ui::HTClientDialog )
    , m_finished( false )
{
    m_ui->setupUi( this );
    connect( m_ui->pbAbort, SIGNAL( clicked() ), &client, SLOT( abort() ) );

    connect( &client, SIGNAL( success() ), SLOT( slotSuccess() ) );
    connect( &client, SIGNAL( failure() ), SLOT( slotFailure() ) );
}

HTClientDialog::~HTClientDialog()
{
    delete m_ui;
}

void HTClientDialog::slotSuccess()
{
    m_finished = true;
    m_ui->pbAbort->setEnabled(false);
    setStatus(tr("Success!"));
    QTimer::singleShot(TIMEOUT, this, SLOT( accept() ) );
}

void HTClientDialog::slotFailure()
{
    m_finished = true;
    m_ui->pbAbort->setEnabled(false);
    setStatus(tr("Failure!"));
    QTimer::singleShot(TIMEOUT, this, SLOT( reject() ) );
}

void HTClientDialog::setStatus(const QString &str)
{
    m_ui->lStatus->setText(str);
}

void HTClientDialog::showEvent( QShowEvent* e )
{
    m_ui->pbAbort->setEnabled(true);
    QDialog::showEvent( e );
    m_client.next();
}

void HTClientDialog::closeEvent( QCloseEvent* e )
{
    e->ignore();

    if (!m_finished)
        m_client.abort();
}

#include "HTClientDialog.moc"
