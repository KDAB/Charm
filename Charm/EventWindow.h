#ifndef EVENTWINDOW_H
#define EVENTWINDOW_H

#include <QMainWindow>

class EventWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit EventWindow( QWidget* parent = 0 );
    ~EventWindow();
};

#endif
