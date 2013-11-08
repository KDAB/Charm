#include <QApplication>
#include <Core/CommandEmitterInterface.h>

#include "CommandRelayCommand.h"

CommandRelayCommand::CommandRelayCommand( QObject* parent )
    : CharmCommand( tr("Relay"), parent )
    , m_payload( 0 )
{   // as long as Charm is single-threaded, this does not do anything,
    // because there will be no repaint
    QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ) );
}

CommandRelayCommand::~CommandRelayCommand()
{
    QApplication::restoreOverrideCursor();
}

void CommandRelayCommand::setCommand( CharmCommand* command )
{
    m_payload = command;
}

bool CommandRelayCommand::prepare()
{
    Q_ASSERT_X( false, Q_FUNC_INFO,
                "Prepare should have been called by the owner instead." );
    return true;
}

bool CommandRelayCommand::execute( ControllerInterface* controller )
{
    return m_payload->execute( controller );
}

bool CommandRelayCommand::rollback( ControllerInterface* controller )
{
    return m_payload->rollback( controller );
}

bool CommandRelayCommand::finalize()
{
    QApplication::restoreOverrideCursor();
    m_payload->owner()->commitCommand( m_payload );
    return true;
}

#include "CommandRelayCommand.moc"

