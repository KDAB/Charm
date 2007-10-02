#ifndef TASKIDDIALOG_H
#define TASKIDDIALOG_H

#include <QDialog>

#include "View.h"
#include "Core/TaskModelInterface.h"
#include "ui_TaskIdDialog.h"

class TaskIdDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TaskIdDialog( TaskModelInterface* model, View* parent );
    ~TaskIdDialog();

    void setSuggestedId( int );
    int selectedId() const;

private slots:
    void on_spinBox_valueChanged( int );

private:
    Ui::TaskIdDialog m_ui;
    TaskModelInterface* m_model;
};

#endif
