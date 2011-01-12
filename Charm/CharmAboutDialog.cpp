#include "CharmAboutDialog.h"
#include "ui_CharmAboutDialog.h"

#include <CharmCMake.h>

CharmAboutDialog::CharmAboutDialog( QWidget* parent )
    : QDialog( parent )
    , m_ui( new Ui::CharmAboutDialog )
{
    m_ui->setupUi( this );
    QString aboutText = m_ui->textBrowser->toHtml();
    aboutText.replace( "CHARM_VERSION", CHARM_VERSION );
    m_ui->textBrowser->setHtml( aboutText );
}

CharmAboutDialog::~CharmAboutDialog()
{
    delete m_ui; m_ui = 0;
}

#include "CharmAboutDialog.moc"
