#ifndef EVENTVIEW_H
#define EVENTVIEW_H

#include <QWidget>
#include <QAction>
// #include <QTimer>

#include "Core/Event.h"
#include "Core/TimeSpans.h"
#include "Core/CommandEmitterInterface.h"

#include "ViewModeInterface.h"

class QModelIndex;

class MainWindow;
class CharmCommand;
class QAbstractItemModel;
class EventModelFilter;

namespace Ui {
    class EventView;
}

class EventView : public QWidget,
                    public ViewModeInterface,
                    public CommandEmitterInterface
{
    Q_OBJECT

public:
    explicit EventView( MainWindow* parent );
    ~EventView();

    void closeEvent( QCloseEvent* );

    void reject();

    void makeVisibleAndCurrent( const Event& );

    // implement ViewModeInterface:
    void saveGuiState();
    void restoreGuiState();
    void stateChanged( State previous );
    void configurationChanged();
    void setModel( ModelConnector* );

signals:
    void visible( bool );
    void emitCommand( CharmCommand* );

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
    Ui::EventView* m_ui;
    MainWindow* m_view;
    Event m_event;
    TaskId m_selectedTask;
    bool m_dirty;
    EventModelFilter* m_model;
    QAction m_actionNewEvent;
    QAction m_actionDeleteEvent;
    QAction m_actionPreviousEvent;
    QAction m_actionNextEvent;
    // QTimer m_commitTimer;
};

#endif
