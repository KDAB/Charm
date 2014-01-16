#include "CharmAboutDialog.h"
#include "ui_CharmAboutDialog.h"

#include <CharmCMake.h>

CharmAboutDialog::CharmAboutDialog( QWidget* parent )
    : QDialog( parent )
    , m_ui( new Ui::CharmAboutDialog )
{
    m_ui->setupUi( this );
    QString versionText = m_ui->versionLabel->text();
    versionText.replace( "CHARM_VERSION", CHARM_VERSION );
    m_ui->versionLabel->setText( versionText );
}

CharmAboutDialog::~CharmAboutDialog()
{
    delete m_ui; m_ui = 0;
}

#include "moc_CharmAboutDialog.cpp"
