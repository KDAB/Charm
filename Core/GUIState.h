#ifndef GUISTATE_H
#define GUISTATE_H

#include <Task.h>

class QSettings;

// I am unsure if this is a good idea (making a class for this at
// all), depends on how it turns out in the future. If there are no
// more options than that, it can be merged back into View.
class GUIState
{
public:
    GUIState();

    const TaskIdList& expandedTasks() const;
    TaskId selectedTask() const;

    void setSelectedTask( TaskId );
    void setExpandedTasks( const TaskIdList& );

    void saveTo( QSettings& settings );
    void loadFrom( const QSettings& settings );

private:
    TaskIdList m_expandedTasks;
    TaskId m_selectedTask;
};

#endif
