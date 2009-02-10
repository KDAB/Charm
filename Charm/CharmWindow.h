#ifndef CHARMWINDOW_H
#define CHARMWINDOW_H

#include <QMainWindow>

#include "Core/ViewInterface.h"
#include "Core/CommandEmitterInterface.h"

class QAction;

class CharmWindow : public QMainWindow,
                    public ViewInterface,
                    public CommandEmitterInterface
{
    Q_OBJECT

public:
    explicit CharmWindow( const QString& name, QWidget* parent = 0 );

    void showHideView();
    QAction* showHideAction();

    /* reimpl */ void showEvent( QShowEvent* );
    /* reimpl */ void hideEvent( QHideEvent* );
    /* reimpl */ void keyPressEvent( QKeyEvent* event );

signals:
    /* reimpl */ void visibilityChanged( bool );

public slots:
    /* reimpl */ void sendCommand( CharmCommand* );
    /* reimpl */ void commitCommand( CharmCommand* );

private:
    const QString m_windowName;
    QAction* m_showHideAction;
};

#endif
