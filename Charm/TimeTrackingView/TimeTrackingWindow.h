#ifndef TimeTrackingWindow_H
#define TimeTrackingWindow_H

#include "Core/ViewInterface.h"
#include "Core/CharmDataModelAdapterInterface.h"

#include "CharmWindow.h"
#include "TimeTrackingView/WeeklySummary.h"

class CharmCommand;
class TimeTrackingView;

namespace Ui {
    class TimeTrackingWindow;
}

class TimeTrackingWindow : public CharmWindow,
                         public CharmDataModelAdapterInterface
{
    Q_OBJECT
public:
    explicit TimeTrackingWindow( QWidget* parent = 0 );
    ~TimeTrackingWindow();

    // application:
    void stateChanged( State previous );
    void sendCommand( CharmCommand* );
    void commitCommand( CharmCommand* );
    void restore();
    void quit();

    void showEvent( QShowEvent* );
    // model adapter:
    void resetTasks();
    void taskAboutToBeAdded( TaskId parent, int pos );
    void taskAdded( TaskId id );
    void taskModified( TaskId id );
    void taskParentChanged( TaskId task, TaskId oldParent, TaskId newParent );
    void taskAboutToBeDeleted( TaskId );
    void taskDeleted( TaskId id );
    void resetEvents();
    void eventAboutToBeAdded( EventId id );
    void eventAdded( EventId id );
    void eventModified( EventId id, Event discardedEvent );
    void eventAboutToBeDeleted( EventId id );
    void eventDeleted( EventId id );
    void eventActivated( EventId id );
    void eventDeactivated( EventId id );

private slots:
    void slotStartEvent( TaskId );
    void slotStopEvent();
    void slotSelectTasksToShow();

signals:
    void emitCommand( CharmCommand* );

private:
    TimeTrackingView* summaryWidget();
    Ui::TimeTrackingWindow* m_ui;
    QVector<WeeklySummary> m_summaries;
};

#endif
