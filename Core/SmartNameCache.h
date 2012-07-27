#ifndef SMARTNAMECACHE_H
#define SMARTNAMECACHE_H

#include "Task.h"

class SmartNameCache {
public:
    void setAllTasks( const TaskList& taskList );
    QString smartName( const TaskId& id ) const;
    void addTask( const Task& task );
    void modifyTask( const Task& task );
    void deleteTask( const Task& task );
    void clearTasks();

private:
    void regenerateSmartNames();
    void sortTasks();
    Task findTask( TaskId id ) const;
    QString makeCombined( const Task& task ) const;

private:
    QMap<TaskId, QString> m_smartTaskNamesById;
    TaskList m_tasks;
};

#endif
