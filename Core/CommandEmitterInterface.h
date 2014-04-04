#ifndef COMMANDEMITTERINTERFACE_H
#define COMMANDEMITTERINTERFACE_H

class CharmCommand;

class CommandEmitterInterface
{
public:
    virtual ~CommandEmitterInterface() {}
    virtual void commitCommand( CharmCommand* ) = 0;
};

#endif
