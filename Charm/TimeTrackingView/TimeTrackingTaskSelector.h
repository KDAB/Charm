#ifndef TIMETRACKINGTASKSELECTOR_H
#define TIMETRACKINGTASKSELECTOR_H

#include <QWidget>
#include <QVector>

#include "Core/Task.h"

#include "WeeklySummary.h"

class QMenu;
class QToolButton;

class TimeTrackingTaskSelector : public QWidget
{
    Q_OBJECT
public:
    explicit TimeTrackingTaskSelector(QWidget *parent = 0);

    void populate( const QVector<WeeklySummary>& summaries  );
    void handleActiveEvents();
    void taskSelected( const WeeklySummary& );

    void resizeEvent( QResizeEvent* );
    QSize sizeHint() const;

signals:
    void startEvent( TaskId );
    void stopEvent( TaskId );
    void updateSummariesPlease();

private slots:
    void slotActionSelected( QAction* );
    void slotGoStopToggled( bool );
    void slotManuallySelectTask();

private:
    void taskSelected( const QString& taskname, TaskId id );
    QToolButton* m_stopGoButton;
    QToolButton* m_taskSelectorButton;
    QMenu *m_menu;
    /** The task that has been selected from the menu. */
    TaskId m_selectedTask;
    /** If the user selected a task through the "Select other task..." menu action,
      its Id is stored here. */
    TaskId m_manuallySelectedTask;
    /** Temporarily store that a task has been manually selected, so that it can be
      activated in the menu once after selection. */
    bool m_taskManuallySelected;
};

#endif // TIMETRACKINGTASKSELECTOR_H

