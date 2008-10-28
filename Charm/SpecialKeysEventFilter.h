#ifndef SPECIALKEYSEVENTFILTER_H
#define SPECIALKEYSEVENTFILTER_H

#include <QObject>
#include <QKeyEvent>

/** SpecialKeysEventFilter catches events like Apple-Close Window,
 * or Apple-Toggle View.
 * */
class SpecialKeysEventFilter : public QObject
{
    Q_OBJECT

public:
    SpecialKeysEventFilter( QObject* parent );
    bool eventFilter( QObject* watched, QEvent* event );

signals:
    void toggleWindow1Visibility();
    void toggleWindow2Visibility();
};

#endif
