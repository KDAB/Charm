#include "CharmAboutDialog.h"
#include "ui_CharmAboutDialog.h"

CharmAboutDialog::CharmAboutDialog( QWidget* parent )
    : QDialog( parent )
    , m_ui( new Ui::CharmAboutDialog )
{
    m_ui->setupUi( this );
}

CharmAboutDialog::~CharmAboutDialog()
{
    delete m_ui; m_ui = 0;
}

#include "CharmAboutDialog.moc"
