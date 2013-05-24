/*
 * EventEditor.h
 *
 *  Created on: Jun 27, 2008
 *      Author: mirko
 */

#ifndef EVENTEDITOR_H_
#define EVENTEDITOR_H_

#include <QDialog>

#include "Core/Event.h"

namespace Ui {
    class EventEditor;
}

class EventEditor: public QDialog
{
    Q_OBJECT

public:
    EventEditor( const Event& event, QWidget* parent = 0 );
    virtual ~EventEditor();

    // return the result after the dialog has been accepted
    Event eventResult() const;

protected Q_SLOTS:
    void accept();

private Q_SLOTS:
    void durationHoursEdited( int );
    void durationMinutesEdited( int );
    void startDateChanged( const QDate& );
    void startTimeChanged( const QTime& );
    void endDateChanged( const QDate& );
    void endTimeChanged( const QTime& );
    void selectTaskClicked();
    void commentChanged();
    void startToNowButtonClicked();
    void endToNowButtonClicked();

private:
    void updateEndTime();
    void updateValues( bool all = false );

    Ui::EventEditor* m_ui;
    Event m_event;
    bool m_updating;
    bool m_endDateChanged;
};

#endif /* EVENTEDITOR_H_ */
