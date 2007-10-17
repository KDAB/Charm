// #include "View.h"
#include "CommandEmitterInterface.h"x
#include "CharmCommand.h"

CharmCommand::CharmCommand( QObject* parent )
    : QObject( parent )
{
    CommandEmitterInterface* emitter = dynamic_cast<CommandEmitterInterface*>( parent );
    if ( emitter ) {
        m_owner = emitter;
    } else {
        Q_ASSERT_X( false, "CharmCommand ctor",
                    "CharmCommand widget pointers have to implement the "
                    "CommandEmitterInterface." );
    }
}

CharmCommand::~CharmCommand()
{
}

CommandEmitterInterface* CharmCommand::owner() const
{
    return m_owner;
}

#include "CharmCommand.moc"
