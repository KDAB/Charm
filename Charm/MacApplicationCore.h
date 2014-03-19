#ifndef MACCOCOAAPPLICATIONCORE_H
#define MACCOCOAAPPLICATIONCORE_H

#include "ApplicationCore.h"

class MacApplicationCore : public ApplicationCore
{
    Q_OBJECT
public:
    explicit MacApplicationCore( QObject* parent = 0 );
    ~MacApplicationCore();
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
