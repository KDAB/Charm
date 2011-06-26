#include <QFileDialog>

#include "Core/CharmConstants.h"

#include "ConfigurationDialog.h"

ConfigurationDialog::ConfigurationDialog( const Configuration& config,
                                          QWidget* parent )
    : QDialog( parent )
    , m_config( config )
{
    m_ui.setupUi( this );
    m_ui.nameLineEdit->setText( config.user.name() );
    m_ui.databaseLocation->setText( config.localStorageDatabase );
    connect( m_ui.buttonBox, SIGNAL( rejected() ), SLOT( reject() ) );
    connect( m_ui.buttonBox, SIGNAL( accepted() ), SLOT( done() ) );
}

Configuration ConfigurationDialog::configuration() const
{
    return m_config;
}

void ConfigurationDialog::on_databaseLocation_textChanged( const QString& text )
{
    checkInput();
}

void ConfigurationDialog::done()
{
    m_config.installationId = 1;
    m_config.user.setId( 1 );
    m_config.user.setName( m_ui.nameLineEdit->text() );
    m_config.localStorageType = CHARM_SQLITE_BACKEND_DESCRIPTOR;
    m_config.localStorageDatabase = m_ui.databaseLocation->text();
    m_config.newDatabase = true;
    // m_config.failure = false; currently set by application
    accept();
}

void ConfigurationDialog::on_databaseLocationButton_clicked()
{
    QString filename = QFileDialog::getSaveFileName( this, tr( "Choose Database Location..." ) );
    if ( ! filename.isNull() )
    {
        m_ui.databaseLocation->setText( filename );
    }
}

void ConfigurationDialog::on_nameLineEdit_textChanged( const QString& text )
{
    checkInput();
}

void ConfigurationDialog::checkInput()
{
    const bool ok = ! m_ui.databaseLocation->text().isEmpty() && ! m_ui.nameLineEdit->text().isEmpty();
    m_ui.buttonBox->button( QDialogButtonBox::Ok )->setEnabled( ok );
}

#include "ConfigurationDialog.moc"
