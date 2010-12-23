#ifndef TIMETRACKINGTASKSELECTOR_H
#define TIMETRACKINGTASKSELECTOR_H

#include <QWidget>
#include <QVector>
#include <QDialog>

#include "Core/Event.h"
#include "Core/Task.h"

#include "WeeklySummary.h"

class QAction;
class QMenu;
class QToolButton;
class QTextEdit;
class QToolBar;

class CommentEditorPopup : public QDialog {
    Q_OBJECT
public:
    explicit CommentEditorPopup( QWidget* parent=0 );

public Q_SLOTS:
    void accept();

private:
    QTextEdit* m_edit;
    EventId m_id;
};

class TimeTrackingTaskSelector : public QWidget
{
    Q_OBJECT
public:
    explicit TimeTrackingTaskSelector(QToolBar* toolBar, QWidget *parent = 0);

    void populate( const QVector<WeeklySummary>& summaries  );
    void handleActiveEvents();
    void taskSelected( const WeeklySummary& );

    void resizeEvent( QResizeEvent* );
    QSize sizeHint() const;

signals:
    void startEvent( TaskId );
    void stopEvents();
    void updateSummariesPlease();

private slots:
    void slotActionSelected( QAction* );
    void slotGoStopToggled( bool );
    void slotEditCommentClicked();
    void slotManuallySelectTask();

private:
    void taskSelected( const QString& taskname, TaskId id );
    QToolButton* m_stopGoButton;
    QAction* m_stopGoAction;
    QToolButton* m_editCommentButton;
    QAction* m_editCommentAction;
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

