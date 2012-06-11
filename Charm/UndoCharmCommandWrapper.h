#ifndef UNDOCHARMCOMMANDWRAPPER_H
#define UNDOCHARMCOMMANDWRAPPER_H

#include <QUndoCommand>

#include "Core/CharmCommand.h"

/**
  Thin wrapper for CharmCommand -> QUndoCommand

  It simply forwards the command text and emits signals for commit/rollback on undo/redo
  **/
class UndoCharmCommandWrapper : public QUndoCommand
{
public:
    UndoCharmCommandWrapper(CharmCommand* command);
    virtual ~UndoCharmCommandWrapper();
    virtual void undo();
    virtual void redo();
    CharmCommand* command() const;
private:
    CharmCommand* m_command;
};

#endif
