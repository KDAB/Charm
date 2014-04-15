/*
 * TaskEditor.h
 *
 *  Created on: Jul 4, 2008
 *      Author: mirko
 */

#ifndef TASKEDITOR_H_
#define TASKEDITOR_H_

#include <QDialog>

#include "Core/Task.h"

#include <QScopedPointer>

namespace Ui {
    class TaskEditor;
}

class TaskEditor: public QDialog
{
    Q_OBJECT

public:
    explicit TaskEditor( QWidget* parent = 0 );
    virtual ~TaskEditor();

    void setTask( const Task& task );

    Task getTask() const;

private Q_SLOTS:
    void slotSelectParent();
    void slotDateChanged( const QDate & date );
    void slotCheckBoxChecked( bool );

private:
    void checkInvariants();

    QScopedPointer<Ui::TaskEditor> m_ui;
    Task m_task;
};

#endif /* TASKEDITOR_H_ */
