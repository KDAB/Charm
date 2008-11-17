#ifndef MACAPPLICATION_H
#define MACAPPLICATION_H

#include <QApplication>

class MacApplication : public QApplication
{
    Q_OBJECT
public:
    MacApplication( int& argc, char* argv[] );
    ~MacApplication();

protected:
    bool macEventFilter( EventHandlerCallRef caller, EventRef event );

Q_SIGNALS:
    void dockIconClicked();
};

#endif
