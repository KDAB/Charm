#ifndef TASKSWINDOW_H
#define TASKSWINDOW_H

#include "CharmWindow.h"

class View; // FIXME rename to TasksView

class TasksWindow : public CharmWindow
{
    Q_OBJECT

public:
    explicit TasksWindow( QWidget* parent = 0 );
    ~TasksWindow();

    /* reimpl */ void stateChanged( State previous );

    // restore the view
    /* reimpl */ void restore();

signals:
    /* reimpl */ void emitCommand( CharmCommand* );
    /* reimpl */ void visibilityChanged( bool );
    /* reimpl */ void saveConfiguration();
    /* reimpl */ void quit();


private:
    View* m_tasksView;
};

#endif
