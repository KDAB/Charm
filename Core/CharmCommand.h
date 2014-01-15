#ifndef CHARMCOMMAND_H
#define CHARMCOMMAND_H

#include <QObject>

class View;
class ControllerInterface;
class CommandEmitterInterface;

/** CharmCommand encapsulates a command the view sends to the controller.
    A command is able to, for example, set the hour glass cursor on
    creation and restore the previous cursor on deletion, if it can be
    executed without user interaction.

    CharmCommand is the implementation of the command pattern in
    Charm. It holds the complete state of the requested operation. When the
    operation has finished, the object has to be deleted.

    The QObject parent has to implement the CommandEmitterInterface.

    Commands cannot be copied or assigned. After creating them, the
    View will send the
    command object to the controller. The controller will execute the
    necessary operations, and send the command back to the View.

    Objects that initiate view actions and therefore issue there own
    commands need to relay those to the view (relayCommand()).

    The View will call prepare() on the command before it is send to
    the controller.
    execute() is called by the controller.
    finalize() is called by the view after the controller has returned
    the command to the view.
*/

class CharmCommand : public QObject
{
    Q_OBJECT

public:
    explicit CharmCommand( const QString& description, QObject* parent = nullptr );
    virtual ~CharmCommand();

    QString description() const;

    virtual bool prepare() = 0;
    virtual bool execute( ControllerInterface* controller ) = 0;
    virtual bool rollback( ControllerInterface* controller ) { return false; }
    virtual bool finalize() = 0;

    CommandEmitterInterface* owner() const;

    //used by UndoCharmCommandWrapper to forward signal firing
    //forwards to emitExecute/emitRollback/emitRequestSlotEventIdChanged
    void requestExecute();
    void requestRollback();
    void requestSlotEventIdChanged(int oldId, int newId);

    //notify CharmCommands in a QUndoStack that an event ID has changed
    virtual void eventIdChanged(int,int){}

signals:
    void emitExecute(CharmCommand*);
    void emitRollback(CharmCommand*);
    void emitSlotEventIdChanged(int,int);

private:
    CharmCommand( const CharmCommand& ); // disallow copying

    CommandEmitterInterface* m_owner;
    const QString m_description;
};

#endif
