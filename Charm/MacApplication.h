#ifndef MACCOCOAAPPLICATION_H
#define MACCOCOAAPPLICATION_H

#include "Application.h"

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

    QMenu m_dockMenu;

    class Private;
    Private* m_private;
};

#endif
