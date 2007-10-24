#ifndef COMMANDEMITTERINTERFACE_H
#define COMMANDEMITTERINTERFACE_H

class CharmCommand;

class CommandEmitterInterface
{
public:
    virtual ~CommandEmitterInterface() {}
    // virtual void emitCommand( CharmCommand* ) = 0;
    virtual void commitCommand( CharmCommand* ) = 0;
};

#endif
