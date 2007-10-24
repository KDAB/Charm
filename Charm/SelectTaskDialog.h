#ifndef SELECTTASKDIALOG_H
#define SELECTTASKDIALOG_H

#include <QDialog>
#include <QSortFilterProxyModel>

#include "Core/TaskModelInterface.h"
#include "ui_SelectTaskDialog.h"

class ViewFilter;

class SelectTaskDialogProxy : public QSortFilterProxyModel,
                              public TaskModelInterface
{
    Q_OBJECT

public:
    SelectTaskDialogProxy();

    Task taskForIndex( const QModelIndex& ) const;
    QModelIndex indexForTaskId( TaskId ) const;
    bool taskIsActive( const Task& task ) const;
    bool taskHasChildren( const Task& task ) const;
    bool taskIdExists( TaskId taskId ) const;

    void eventActivationNotice( EventId ) {}
    void eventDeactivationNotice( EventId ) {}

protected:
    bool filterAcceptsColumn( int column, const QModelIndex& parent ) const;

private:
    const ViewFilter* source() const;
};

class SelectTaskDialog : public QDialog
{
    Q_OBJECT

public:
    SelectTaskDialog( QWidget* parent );

    TaskId selectedTask() const;

private slots:
    void slotCurrentItemChanged( const QModelIndex&, const QModelIndex& );
    void slotFilterTextChanged( const QString& );

private:
    Ui::SelectTaskDialog m_ui;
    TaskId m_selectedTask;
    SelectTaskDialogProxy m_proxy;
};

#endif
