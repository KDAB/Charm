#include <QtDebug>
#include "SpecialKeysEventFilter.h"

SpecialKeysEventFilter::SpecialKeysEventFilter( QObject* parent )
    : QObject( parent )
{
}

bool SpecialKeysEventFilter::eventFilter( QObject* watched, QEvent* event)
{
#ifdef Q_WS_MAC
    if ( event->type() == QEvent::KeyPress ) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>( event );
        if ( keyEvent->modifiers() & Qt::ControlModifier ) {
            // this is the Apple key, on Mac
            switch( keyEvent->key() ) {
            case Qt::Key_1:
                qDebug() << "SpecialKeysEventFilter::eventFilter: 1";
                emit toggleWindow1Visibility();
                break;
            case Qt::Key_2:
                qDebug() << "SpecialKeysEventFilter::eventFilter: 2";
                emit toggleWindow2Visibility();
                break;
            default:
                break;
            }
        }
        return true;
    } else {
        return false;
    }
#else
    // the shortcuts might be useful on other platforms, too, but they are too unusual
    Q_UNUSED( watched )
    Q_UNUSED( event )
#endif
}

#include "SpecialKeysEventFilter.moc"
