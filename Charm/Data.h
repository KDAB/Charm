#ifndef DATA_H
#define DATA_H

class QIcon;
class QPixmap;

class Data {
public:
    static const QIcon& charmIcon();
    static const QIcon& charmTrayIcon();

    static const QIcon& goIcon();
    static const QIcon& stopIcon();
    static const QIcon& newTaskIcon();
    static const QIcon& editTaskIcon();
    static const QIcon& deleteTaskIcon();
    static const QIcon& editEventIcon();
    static const QIcon& previousEventIcon();
    static const QIcon& nextEventIcon();
    static const QPixmap& checkIcon();
    static const QPixmap& activePixmap();
    static const QIcon& quitCharmIcon();
    static const QIcon& clearFilterIcon();
    static const QIcon& configureIcon();
    static const QPixmap& editorLockedPixmap();
    static const QPixmap& editorDirtyPixmap();
    static const QPixmap& recorderStopIcon();
    static const QPixmap& recorderGoIcon();
    static const QPixmap& backdropPixmap();
};

#endif
