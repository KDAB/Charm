#include <Core/CharmCommand.h>

#include "Commands/CommandRelayCommand.h"
#include "CharmWindow.h"

CharmWindow::CharmWindow( QWidget* parent )
    : QMainWindow( parent )
{
}

void CharmWindow::sendCommand( CharmCommand* cmd )
{
    cmd->prepare();
    CommandRelayCommand* relay = new CommandRelayCommand( this );
    relay->setCommand( cmd );
    emit emitCommand( relay );
}

void CharmWindow::commitCommand( CharmCommand* command )
{
    command->finalize();
    delete command;
}

#include "CharmWindow.moc"

