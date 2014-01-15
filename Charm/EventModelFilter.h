#ifndef EVENTMODELFILTER_H
#define EVENTMODELFILTER_H

#include <QDate>
#include <QSortFilterProxyModel>

#include <Core/EventModelInterface.h>
#include <Core/CommandEmitterInterface.h>

#include "EventModelAdapter.h"

class CharmDataModel;

class EventModelFilter : public QSortFilterProxyModel,
                         public CommandEmitterInterface,
                         public EventModelInterface
{
    Q_OBJECT

public:
    explicit EventModelFilter( CharmDataModel*, QObject* parent = nullptr );
    virtual ~EventModelFilter();

    /** Returns the total number of seconds of all events in the model. */
    int totalDuration();

    // implement EventModelInterface:
    const Event& eventForIndex( const QModelIndex& ) const;
    QModelIndex indexForEvent( const Event& ) const;

    bool filterAcceptsRow( int srow, const QModelIndex & sparent ) const;

    void setFilterStartDate( const QDate& date );
    void setFilterEndDate( const QDate& date );

    // implement CommandEmitterInterface:
    void commitCommand( CharmCommand* );

    // implement to sort by event start datetime
    bool lessThan( const QModelIndex& left, const QModelIndex& right ) const;

signals:
    void eventActivationNotice( EventId id );
    void eventDeactivationNotice( EventId id );

private:
    EventModelAdapter m_model;
    QDate m_start;
    QDate m_end;
};

#endif
