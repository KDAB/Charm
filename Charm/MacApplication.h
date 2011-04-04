#ifndef MACCOCOAAPPLICATION_H
#define MACCOCOAAPPLICATION_H

#include "Application.h"

class NSEvent;
class objc_object;

class MacApplication : public Application
{
    Q_OBJECT
public:
    MacApplication( int& argc, char* argv[] );
    ~MacApplication();
    // This method to be public due to lack of friend classes in Objective-C and
    // the lack inheritance of Objective-C classes from C++ ones.
    void dockIconClickEvent();

private slots:
    void handleStateChange( State state ) const;

private:
    static QList< QShortcut* > shortcuts( QWidget* parent );
    static QList< QShortcut* > activeShortcuts( const QKeySequence& seq, bool autorep, QWidget* parent = 0);
    NSEvent* cocoaEventFilter( NSEvent* incomingEvent );
    void setupCocoaEventHandler() const;

    QMenu m_dockMenu;

    objc_object* m_pool;
    objc_object* m_eventMonitor;
    objc_object* m_dockIconClickEventHandler;
};

#endif
