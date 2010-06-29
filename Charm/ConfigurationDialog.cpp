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
    m_ui.stackedWidget->setCurrentWidget( m_ui.databaseTypePage );
    QStringList backendTypes;
    backendTypes << "Local Database"
        // << "Network Database" // not implemented
        ;
    m_ui.comboDbType->addItems( backendTypes );
    m_ui.sqliteLocation->setText( config.localStorageDatabase );
    m_ui.nameLineEdit->setText( config.user.name() );
    // resize( minimumSize() );
}

ConfigurationDialog::~ConfigurationDialog()
{
}

Configuration ConfigurationDialog::configuration() const
{
    return m_config;
}

void ConfigurationDialog::on_comboDbType_currentIndexChanged( int )
{
}

void ConfigurationDialog::on_dbTypeNextButton_clicked()
{
    if ( m_ui.comboDbType->currentIndex() == 0 ) {
        m_ui.stackedWidget->setCurrentWidget( m_ui.sqliteDatabasePage );
    }
    // else ...
}

void ConfigurationDialog::on_sqliteBackButton_clicked()
{
    m_ui.stackedWidget->setCurrentWidget( m_ui.databaseTypePage );
}

void ConfigurationDialog::on_sqliteLocation_textChanged( const QString& text )
{
    m_ui.sqliteDoneButton->setEnabled( ! text.isEmpty() );
}

void ConfigurationDialog::on_sqliteDoneButton_clicked()
{
    m_config.installationId = 1;
    m_config.user.setId( 1 );
    m_config.user.setName( m_ui.nameLineEdit->text() );
    m_config.localStorageType = CHARM_SQLITE_BACKEND_DESCRIPTOR;
    m_config.localStorageDatabase = m_ui.sqliteLocation->text();
    m_config.newDatabase = true;
    // m_config.failure = false; currently set by application
    accept();
}

void ConfigurationDialog::on_sqliteLocationButton_clicked()
{
    QString filename = QFileDialog::getSaveFileName( this, tr( "Choose Database Location..." ) );
    if ( ! filename.isNull() )
    {
        m_ui.sqliteLocation->setText( filename );
    }
}

void ConfigurationDialog::on_nameLineEdit_textChanged( const QString& text )
{
    m_ui.nameLineEdit->setEnabled( ! text.isEmpty() );
}

#include "ConfigurationDialog.moc"
