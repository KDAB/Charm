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

	// return the result after the dialgo has been accepted
	Event event() const;

private Q_SLOTS:
	void durationHoursEdited( int );
	void durationMinutesEdited( int );
	void startDateChanged( const QDate& );
	void startTimeChanged( const QTime& );
	void endDateChanged( const QDate& );
	void endTimeChanged( const QTime& );
	void selectTaskClicked();
	void commentChanged();

private:
	void updateEndTime();
	void updateValues( bool all = false );

	Ui::EventEditor* m_ui;
	Event m_event;
	bool m_updating;
};

#endif /* EVENTEDITOR_H_ */
