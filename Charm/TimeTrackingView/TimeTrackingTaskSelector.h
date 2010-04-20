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
    void handleActiveEvents( int count, const QVector<WeeklySummary>& summaries );
    void taskSelected( const WeeklySummary& );

    void resizeEvent( QResizeEvent* );
    QSize sizeHint() const;

signals:
    void startEvent( TaskId );
    void stopEvent( TaskId );

private slots:
    void slotActionSelected( QAction* );
    void slotGoStopToggled( bool );

private:
    void taskSelected( const QString& taskname, TaskId id );

    QToolButton* m_stopGoButton;
    QMenu *m_menu;
    QToolButton* m_taskSelectorButton;
    TaskId m_selectedTask;
};

#endif // TIMETRACKINGTASKSELECTOR_H

