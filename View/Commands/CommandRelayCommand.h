#ifndef COMMANDRELAYCOMMAND_H
#define COMMANDRELAYCOMMAND_H

#include <Core/CharmCommand.h>

class CommandEmitterInterface;

/** CommandRelayCommand is a decorator class that is used to wrap all
    commands send by the view.
    ATM, CommandRelayCommand sets the hour glass cursor on the view
    and resets it when it is deleted.
*/
class CommandRelayCommand : public CharmCommand
{
    Q_OBJECT

public:
    explicit CommandRelayCommand( QObject* parent );
    ~CommandRelayCommand();

    void setCommand( CharmCommand* command );

    bool prepare();
    bool execute( ControllerInterface* );
    bool finalize();

private:
    CharmCommand* m_payload;
};

#endif
