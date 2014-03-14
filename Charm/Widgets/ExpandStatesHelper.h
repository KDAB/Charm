#ifndef EXPANDSTATESHELPER_H
#define EXPANDSTATESHELPER_H

#include "Core/Task.h"
#include "TaskModelAdapter.h"

#include <QHash>

class QTreeView;

namespace Charm {
    //helper functions for saving and restoring expansion states when filtering
    void saveExpandStates( QTreeView* tv, QHash<TaskId,bool>* map );
    void restoreExpandStates( QTreeView* tv, QHash<TaskId,bool>* map );
}

#endif
