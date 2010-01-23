#ifndef MACCARBONAPPLICATION_H
#define MACCARBONAPPLICATION_H

#include <QApplication>

class MacCarbonApplication : public QApplication
{
    Q_OBJECT
public:
    MacCarbonApplication( int& argc, char* argv[] );
    ~MacCarbonApplication();

protected:
    bool macEventFilter( EventHandlerCallRef caller, EventRef event );

Q_SIGNALS:
    void dockIconClicked();
};

#endif
