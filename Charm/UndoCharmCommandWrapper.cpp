#include "UndoCharmCommandWrapper.h"

UndoCharmCommandWrapper::UndoCharmCommandWrapper(CharmCommand* command)
    : m_command(command)
{
    setText(command->description());
}

UndoCharmCommandWrapper::~UndoCharmCommandWrapper()
{
    delete m_command;
}

void UndoCharmCommandWrapper::undo()
{
    m_command->requestRollback();
}

void UndoCharmCommandWrapper::redo()
{
    m_command->requestExecute();
}

CharmCommand *UndoCharmCommandWrapper::command() const
{
    return m_command;
}
