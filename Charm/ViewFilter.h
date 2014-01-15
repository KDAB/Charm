#ifndef VIEWFILTER_H
#define VIEWFILTER_H

#include <QSortFilterProxyModel>

#include "Core/Configuration.h"
#include "TaskModelAdapter.h"
#include "Core/TaskModelInterface.h"
#include "Core/CommandEmitterInterface.h"

class QTreeView;
template <typename K, typename V> class QMap;

class CharmDataModel;
class CharmCommand;

namespace Charm {
    //helper functions for saving and restoring expansion states when filtering
    void saveExpandStates( QTreeView* tv, QHash<TaskId,bool>* map );
    void restoreExpandStates( QTreeView* tv, QHash<TaskId,bool>* map );
}

// ViewFilter is implemented as a decorator to avoid accidental direct
// access to the task model with indexes of the proxy
class ViewFilter : public QSortFilterProxyModel,
                   public TaskModelInterface,
                   public CommandEmitterInterface
{
    Q_OBJECT
public:    
    explicit ViewFilter( CharmDataModel*, QObject* parent = nullptr );
    virtual ~ViewFilter();

    // implement TaskModelInterface
    Task taskForIndex( const QModelIndex& ) const;
    QModelIndex indexForTaskId( TaskId ) const;
    bool taskIsActive( const Task& task ) const;
    bool taskHasChildren( const Task& task ) const;

    // filter for subscriptions:
    void prefilteringModeChanged();

    bool taskIdExists( TaskId taskId ) const;
    void commitCommand( CharmCommand* );
    bool filterAcceptsColumn( int source_column, const QModelIndex& source_parent ) const;
    bool filterAcceptsRow( int row, const QModelIndex& parent ) const;

signals:
    void eventActivationNotice( EventId id );
    void eventDeactivationNotice( EventId id );

private:
    TaskModelAdapter m_model;
};

#endif
