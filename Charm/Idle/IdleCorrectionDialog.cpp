#include "IdleCorrectionDialog.h"
#include "ui_IdleCorrectionDialog.h"

IdleCorrectionDialog::IdleCorrectionDialog( QWidget* parent )
    : QDialog( parent )
{
    m_ui = new Ui::IdleCorrectionDialog;
    m_ui->setupUi( this );
}

IdleCorrectionDialog::~IdleCorrectionDialog()
{
    delete m_ui;
    m_ui = nullptr;
}

IdleCorrectionDialog::Result IdleCorrectionDialog::result()
{
    if ( m_ui->ignore->isChecked() ) {
        return Idle_Ignore;
    } else if ( m_ui->endEvent->isChecked() ) {
        return Idle_EndEvent;
    } else {
        Q_ASSERT( false ); // unhandled whatever?
    }

    return Idle_NoResult;
}

#include "IdleCorrectionDialog.moc"
