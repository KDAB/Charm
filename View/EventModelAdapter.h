#ifndef EVENTMODELADAPTER_H
#define EVENTMODELADAPTER_H

#include <QAbstractItemModel>

#include "Core/Event.h"
#include "Core/EventModelInterface.h"
#include "Core/CharmDataModelAdapterInterface.h"
#include "Core/CommandEmitterInterface.h"

class CharmDataModel;

class EventModelAdapter : public QAbstractListModel,
                          public CharmDataModelAdapterInterface,
                          public CommandEmitterInterface,
                          public EventModelInterface
{
    Q_OBJECT

public:
    explicit EventModelAdapter( CharmDataModel* parent );
    virtual ~EventModelAdapter();

    int rowCount( const QModelIndex& parent = QModelIndex() ) const;

    QVariant data( const QModelIndex& index, int role = Qt::DisplayRole ) const;

    // reimplement CharmDataModelAdapterInterface:
    void resetTasks() {}
    void taskAboutToBeAdded( TaskId parentTask, int pos ) {}
    void taskAdded( TaskId id ) {}
    void taskModified( TaskId id ) {}
    void taskAboutToBeDeleted( TaskId ) {}
    void taskDeleted( TaskId id ) {}

    void resetEvents();
    void eventAboutToBeAdded( EventId id );
    void eventAdded( EventId id );
    void eventModified( EventId id, Event );
    void eventAboutToBeDeleted( EventId id );
    void eventDeleted( EventId id );

    void eventActivated( EventId id );
    void eventDeactivated( EventId id );

    // reimplement EventModelInterface:
    const Event& eventForIndex( const QModelIndex& index ) const;
    QModelIndex indexForEvent( const Event& ) const;

    // reimplement CommandEmitterInterface:
    void commitCommand( CharmCommand* );

signals:
    void eventActivationNotice( EventId id );
    void eventDeactivationNotice( EventId id );

private:
    EventId m_selectedEvent;
    // if this is slow, we may want to store pointers here:
    EventIdList m_events;
    CharmDataModel* m_dataModel;
};

#endif
