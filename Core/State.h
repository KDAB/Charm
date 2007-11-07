#ifndef CHARM_STATE_H
#define CHARM_STATE_H

#include <QMetaType>

enum State {
    Constructed,
    StartingUp,
    Connecting,
    Connected,
    Disconnecting,
    ShuttingDown,
    Dead,
    NumberOfCharmApplicationStates
};

Q_DECLARE_METATYPE( State )

extern const char* StateNames[NumberOfCharmApplicationStates];

#endif
