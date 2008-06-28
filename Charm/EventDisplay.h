/*
 * EventDisplay.h
 *
 *  Created on: Jun 28, 2008
 *      Author: mirko
 */

#ifndef EVENTDISPLAY_H_
#define EVENTDISPLAY_H_

#include <QWidget>

#include "Core/Event.h"

namespace Ui {
	class EventDisplay;
}

class EventDisplay: public QWidget
{
	Q_OBJECT

public:
	EventDisplay( QWidget* parent = 0 );
	virtual ~EventDisplay();

	void setEvent( const Event& );
Q_SIGNALS:
	void editEvent( const Event&  );

protected:
	void mouseDoubleClickEvent ( QMouseEvent* );

private:
	Ui::EventDisplay* m_ui;
	Event m_event;
};

#endif /* EVENTDISPLAY_H_ */
