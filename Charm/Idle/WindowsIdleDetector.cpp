#include "WindowsIdleDetector.h"

#include "windows.h"

#include <QDebug>

#if 0 //callback for suspend events, couldn't get it to work (using a dummy window (see constructor))
LRESULT CALLBACK handleWindowsEvent( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam ) {
    qDebug() << hwnd << message;
    switch ( message ) {
    case WM_POWERBROADCAST:
        switch ( static_cast<DWORD>( wParam ) ) {
        case PBT_APMQUERYSUSPEND:
            qDebug() << "query suspend";
            return TRUE;
        case PBT_APMQUERYSUSPENDFAILED:
            qDebug() << "suspend failed";
            break;
        case PBT_APMSUSPEND:
            qDebug() << "suspend";
            break;
        case PBT_APMRESUMESUSPEND:
            qDebug() << "resume";
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }

    return DefWindowProc( hwnd, message, wParam, lParam );
}
#endif

WindowsIdleDetector::WindowsIdleDetector( QObject* parent ) : IdleDetector( parent ) {
    connect( &m_timer, SIGNAL(timeout()), this, SLOT(timeout()) );
    m_timer.setInterval( idlenessDuration() * 1000 / 2 );
    m_timer.setSingleShot( false );
    m_timer.start();
#if 0
    WNDCLASS wndclass;
    ZeroMemory( &wndclass, sizeof(wndclass) );
    wndclass.lpfnWndProc = handleWindowsEvent;
    wndclass.hInstance = GetModuleHandle( 0 );
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = "foobar";
    if ( !RegisterClass( &wndclass ) ) {
        qDebug() << "could not register wndclass";
        return;
    }
    HWND handle = CreateWindow( "DummyWindow", "Dummy Window", WS_OVERLAPPEDWINDOW,
                           CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                           CW_USEDEFAULT, 0, 0, GetModuleHandle( 0 ), 0 );
    ShowWindow( handle, SW_SHOW );
    UpdateWindow( handle );
#endif
}

void WindowsIdleDetector::idlenessDurationChanged() {
    m_timer.stop();
    m_timer.setInterval( idlenessDuration() * 1000 / 2 );
    m_timer.start();
}

void WindowsIdleDetector::timeout() {
    LASTINPUTINFO lif;
    lif.cbSize = sizeof( lif );
    const bool ret = GetLastInputInfo( &lif );
    if ( !ret ) {
        qWarning() << "Idle detection: GetLastInputInfo failed.";
        return;
    }

    const qint64 dwTime = static_cast<qint64>( lif.dwTime );
    const qint64 ctk = static_cast<qint64>( GetTickCount64() );
    const int idleSecs = ( ctk - dwTime ) / 1000;
    if ( idleSecs >= idlenessDuration() )
        maybeIdle( IdlePeriod(QDateTime::currentDateTime().addSecs( -idleSecs ),
                              QDateTime::currentDateTime() ) );
}

#include "WindowsIdleDetector.moc"
