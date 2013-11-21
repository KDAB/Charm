#include "TaskIdDialog.h"

#include <QDialogButtonBox>
#include <QPushButton>

TaskIdDialog::TaskIdDialog( TaskModelInterface* model, TasksView* parent )
    : QDialog( parent )
    , m_model( model )
{
    m_ui.setupUi( this );
    m_ui.spinBox->setRange( 1, 1000*1000*1000 );
    connect( m_ui.buttonBox, SIGNAL(accepted()), this, SLOT(accept()) );
    connect( m_ui.buttonBox, SIGNAL(rejected()), this, SLOT(reject()) );
    // resize( minimumSize() );
}

TaskIdDialog::~TaskIdDialog()
{
}

void TaskIdDialog::setSuggestedId( int id )
{
    m_ui.spinBox->setValue( id );
    m_ui.spinBox->selectAll();
}

void TaskIdDialog::on_spinBox_valueChanged( int value )
{
    const bool taskExists = m_model->taskIdExists( value );
    m_ui.buttonBox->button( QDialogButtonBox::Ok )->setEnabled( !taskExists );
    m_ui.labelExists->setText( taskExists ? tr( "(not ok, exists)" ) : tr( "(ok, does not exist)" ) );
}

int TaskIdDialog::selectedId() const
{
    return m_ui.spinBox->value();
}

QString TaskIdDialog::taskName() const
{
    return m_ui.taskName->text();
}

#include "moc_TaskIdDialog.cpp"
