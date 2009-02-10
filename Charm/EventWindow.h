#ifndef EVENTWINDOW_H
#define EVENTWINDOW_H

#include "CharmWindow.h"

class EventView;

class EventWindow : public CharmWindow
{
    Q_OBJECT

public:
    explicit EventWindow( QWidget* parent = 0 );
    ~EventWindow();

    /* reimpl */ void stateChanged( State previous );

    // restore the view
    /* reimpl */ void restore();

signals:
    /* reimpl */ void emitCommand( CharmCommand* );
    /* reimpl */ void saveConfiguration();
    /* reimpl */ void quit();

private:
    EventView* m_eventView;

};

#endif
