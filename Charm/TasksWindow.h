#ifndef TASKSWINDOW_H
#define TASKSWINDOW_H

#include "CharmWindow.h"

class TasksView;

class TasksWindow : public CharmWindow
{
    Q_OBJECT

public:
    explicit TasksWindow( QWidget* parent = 0 );
    ~TasksWindow();

    /* reimpl */ void stateChanged( State previous );
    /* reimpl */ void sendCommand( CharmCommand* );
    /* reimpl */ void commitCommand( CharmCommand* );

    // restore the view
    /* reimpl */ void restore();

public slots:
    /* reimpl */ void configurationChanged();

protected:
    /* reimpl */ void insertEditMenu();

signals:
    /* reimpl */ void emitCommand( CharmCommand* );
    /* reimpl */ void quit();

private:
    TasksView* m_tasksView;
};

#endif
