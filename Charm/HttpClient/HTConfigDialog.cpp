#include "HTConfigDialog.h"

#include "HTClient.h"
#include "ui_HTConfigDialog.h"

HTConfigDialog::HTConfigDialog(HTClient &client)
    : QDialog(0)
    , m_client( client )
    , m_ui( new Ui::HTConfigDialog )
{
    m_ui->setupUi( this );
    m_ui->domainLineEdit->setText(m_client.domain());
    m_ui->passwordLineEdit->setText(m_client.password());
    m_ui->usernameLineEdit->setText(m_client.username());
}

HTConfigDialog::~HTConfigDialog()
{
    delete m_ui;
}

void
HTConfigDialog::accept()
{
	m_client.setDomain(m_ui->domainLineEdit->text());
	m_client.setUsername(m_ui->usernameLineEdit->text());
	m_client.setPassword(m_ui->passwordLineEdit->text());
	QDialog::accept();
}

#include "HTConfigDialog.moc"
