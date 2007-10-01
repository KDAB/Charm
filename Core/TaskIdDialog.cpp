#include "TaskIdDialog.h"

TaskIdDialog::TaskIdDialog( TaskModelInterface* model, View* parent )
    : QDialog( parent )
    , m_model( model )
{
    m_ui.setupUi( this );
    m_ui.spinBox->setRange( 1, 1000*1000*1000 );
    // resize( minimumSize() );
}

TaskIdDialog::~TaskIdDialog()
{
}

void TaskIdDialog::setSuggestedId( int id )
{
    m_ui.spinBox->setValue( id );
}

void TaskIdDialog::on_spinBox_valueChanged( int value )
{
    if ( m_model->taskIdExists( value ) ) {
        m_ui.okButton->setEnabled( false );
        m_ui.labelExists->setText( tr( "(not ok, exists)" ) );
    } else {
        m_ui.okButton->setEnabled( true );
        m_ui.labelExists->setText( tr( "(ok, does not exist)" ) );
    }
}

int TaskIdDialog::selectedId() const
{
    return m_ui.spinBox->value();
}

#include "TaskIdDialog.moc"
