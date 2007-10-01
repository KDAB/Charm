#ifndef EVENTEDITOR_H
#define EVENTEDITOR_H

#include <QDialog>
#include <QAction>
#include <QTimer>

#include "Event.h"
#include "TimeSpans.h"
#include "Commands/CommandEmitterInterface.h"

class QModelIndex;

class View;
class CharmCommand;
class QAbstractItemModel;
class EventModelFilter;

namespace Ui {
    class EventEditor;
}

class EventEditor : public QDialog,
                    public CommandEmitterInterface
{
    Q_OBJECT

public:
    explicit EventEditor( View* parent );
    ~EventEditor();

    void closeEvent( QCloseEvent* );

    void reject();

    void setModel( EventModelFilter* );

    void makeVisibleAndCurrent( const Event& );

signals:
    void visible( bool );

public slots:
    void commitCommand( CharmCommand* );
    void delayedInitialization();
    void timeSpansChanged();
    void timeFrameChanged(int );
    void slotConfigureUi();

private slots:
    void slotEventChanged();
    void slotCurrentItemChanged( const QModelIndex&, const QModelIndex& );
    void slotContextMenuRequested( const QPoint& );
    void slotSelectTask();
    void slotNextEvent();
    void slotPreviousEvent();
    void slotNewEvent();
    void slotDeleteEvent();
    void slotEventActivated( EventId );
    void slotEventDeactivated( EventId );
    void slotUpdateTotal();
    void slotUpdateCurrent();
    void slotCommitTimeout();

private:
    Event newSettings();
    void setCurrentEvent( const Event& );
    void commitChanges();

    QList<NamedTimeSpan> m_timeSpans;
    Ui::EventEditor* m_ui;
    View* m_view;
    Event m_event;
    TaskId m_selectedTask;
    bool m_dirty;
    EventModelFilter* m_model;
    QAction m_actionNewEvent;
    QAction m_actionDeleteEvent;
    QAction m_actionPreviousEvent;
    QAction m_actionNextEvent;
    QTimer m_commitTimer;
};

#endif
