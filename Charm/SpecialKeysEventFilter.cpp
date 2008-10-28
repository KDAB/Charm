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
                emit toggleWindow1Visibility();
                break;
            case Qt::Key_2:
                emit toggleWindow2Visibility();
                break;
            default:
                return false; // the other keys remain unhandled
            }
            return true;
        }
    }
#else
    // the shortcuts might be useful on other platforms, too, but they are too unusual
    Q_UNUSED( watched );
    Q_UNUSED( event );
#endif
    return false;
}

#include "SpecialKeysEventFilter.moc"
