#ifndef TASKIDDIALOG_H
#define TASKIDDIALOG_H

#include <QDialog>

#include "TasksView.h"
#include "Core/TaskModelInterface.h"
#include "ui_TaskIdDialog.h"

/**
 * Dialog shown when creating a task
 */
class TaskIdDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TaskIdDialog( TaskModelInterface* model, View* parent );
    ~TaskIdDialog();

    void setSuggestedId( int );
    int selectedId() const;

    QString taskName() const;

private slots:
    void on_spinBox_valueChanged( int );

private:
    Ui::TaskIdDialog m_ui;
    TaskModelInterface* m_model;
};

#endif
