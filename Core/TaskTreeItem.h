#ifndef TASKTREEITEM_H
#define TASKTREEITEM_H

#include <QList>

#include "Task.h"

/** TaskTreeItem is a node in the task tree.
    The tasks form a tree, since tasks can belong to a parent task.
    This structure is modeled by storing TaskTreeItems in a map that
    associates them with the respective task id.
    Every TaskTreeItem has a parent that represents the parent
    task. If a task has no parent, it is a child of the root
    TaskTreeItem stored in the model.
    Every TaskTreeItem keeps a list of children.
    Every TaskTreeItem also has a position in it's parents list of
    children. This integer position can be retrieved by calling row on
    the item.
*/
class TaskTreeItem
{
public:
    typedef QList<const TaskTreeItem*> ConstPointerList;
    typedef std::map<TaskId, TaskTreeItem> Map;

    TaskTreeItem();

    explicit TaskTreeItem( const Task& task );

    void makeChildOf( TaskTreeItem& parent );

    bool isValid() const;

    Task& task();

    const Task& task() const;

    const TaskTreeItem& child( int row ) const;

    int row() const;

    int childCount() const;

    // recursively find all children of this item
    // warning: SLOW
    TaskList children() const;

    TaskIdList childIds() const;

private:
    TaskTreeItem* m_parent;
    ConstPointerList m_children;
    Task m_task;
};


#endif
