#ifndef TIMETRACKINGVIEW_H
#define TIMETRACKINGVIEW_H

#include <QWidget>

#include "Core/ViewInterface.h"
#include "Core/CharmDataModelAdapterInterface.h"

#include "TimeTrackingSummaryWidget.h"

class CharmCommand;
class TimeTrackingSummaryWidget;

namespace Ui {
    class TimeTrackingView;
}

class TimeTrackingView : public QWidget,
                         public ViewInterface,
                         public CharmDataModelAdapterInterface
{
    Q_OBJECT
public:
    explicit TimeTrackingView( QWidget* parent = 0 );
    ~TimeTrackingView();
    // application:
    void stateChanged( State previous );
    void saveConfiguration();
    void emitCommand( CharmCommand* );
    void sendCommand( CharmCommand* );
    void commitCommand( CharmCommand* );
    void restore();
    void quit();

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
    void slotMaybeShrink();
    void slotStartEvent( TaskId );
    void slotStopEvent( TaskId );

private:
    void selectTasksToShow();
    TimeTrackingSummaryWidget* summaryWidget();
    Ui::TimeTrackingView* m_ui;
    QVector<TimeTrackingSummaryWidget::WeeklySummary> m_summaries;
};

#endif
