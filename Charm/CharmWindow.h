#ifndef CHARMWINDOW_H
#define CHARMWINDOW_H

#include <QMainWindow>
#include "Core/ViewInterface.h"
#include "Core/CommandEmitterInterface.h"

class CharmWindow : public QMainWindow,
                    public ViewInterface,
                    public CommandEmitterInterface
{
    Q_OBJECT

public:
    explicit CharmWindow( QWidget* parent = 0 );

public slots:
    /* reimpl */ void sendCommand( CharmCommand* );
    /* reimpl */ void commitCommand( CharmCommand* );
};

#endif
