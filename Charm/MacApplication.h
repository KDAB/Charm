#ifndef MACAPPLICATION_H
#define MACAPPLICATION_H

#include <QApplication>

#include <QShortcut>

class MacApplication : public QApplication
{
Q_OBJECT
public:
    explicit MacApplication( int& argc, char* argv[] );

protected:
    static QList< QShortcut* > shortcuts( QWidget* parent );
    static QList< QShortcut* > activeShortcuts( const QKeySequence& seq, bool autorep, QWidget* parent = 0);

signals:
    void dockIconClicked();
};

#endif // MACAPPLICATION_H
