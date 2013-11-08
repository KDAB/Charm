#include "CommandEmitterInterface.h"
#include "CharmCommand.h"

CharmCommand::CharmCommand( const QString& description, QObject *parent )
    : QObject( parent ),
      m_description(description)
{
    CommandEmitterInterface* emitter = dynamic_cast<CommandEmitterInterface*>( parent );
    if ( emitter ) {
        m_owner = emitter;
    } else {
        Q_ASSERT_X( false, Q_FUNC_INFO,
                    "CharmCommand widget pointers have to implement the "
                    "CommandEmitterInterface." );
    }
}

CharmCommand::~CharmCommand()
{
}

QString CharmCommand::description() const
{
    return m_description;
}

CommandEmitterInterface* CharmCommand::owner() const
{
    return m_owner;
}

void CharmCommand::requestExecute()
{
    emit emitExecute(this);
}

void CharmCommand::requestRollback()
{
    emit emitRollback(this);
}

void CharmCommand::requestSlotEventIdChanged(int oldId, int newId)
{
    emit emitSlotEventIdChanged(oldId,newId);
}

#include "CharmCommand.moc"
