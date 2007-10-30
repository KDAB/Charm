#ifndef EVENT_H
#define EVENT_H

#include <map>
#include <exception>

#include <QList>
#include <QtDebug>
#include <QDateTime>
#include <QDomElement>

#include "Task.h"

typedef int EventId;
class QDomDocument;

/** An event is a recorded time for a task.
    It records the according task, the duration and a possible
    comment. */
class Event {
public:
    class XmlSerializationException : public std::exception {
    public:
        XmlSerializationException( const char* message = 0 )
            : m_message( message )
        {}
        const char* what() const throw() {
            return m_message;
        }
    private:
        const char* m_message;
    };

    Event();

    bool operator == ( const Event& other ) const;

    bool operator != ( const Event& other ) const
    { return ! operator==( other ); }

    EventId id() const;

    void setId( EventId id );

    void setInstallationId( int instId );

    int installationId() const;

    bool isValid() const;

    TaskId taskId() const;

    void setTaskId( TaskId id);

    const QString& comment() const;

    void setComment( const QString& );

    const QDateTime& startDateTime() const;

    void setStartDateTime( const QDateTime& start = QDateTime::currentDateTime() );

    const QDateTime& endDateTime() const;

    void setEndDateTime( const QDateTime& end = QDateTime::currentDateTime() );

    /** Returns the duration of this event in seconds. */
    int duration () const;

    void dump() const;

    QDomElement toXml( QDomDocument ) const;

    static Event fromXml( const QDomElement& ) throw ( XmlSerializationException );

private:
    /** The installation-unique id of the event. */
    int m_installationId;
    int m_id;
    /** The task this event belongs to. */
    TaskId m_taskId;
    /** A possible user comment.
        May be empty. */
    QString m_comment;
    /** The start datetime of the event. */
    QDateTime m_start;
    /** The end datetime of the event. */
    QDateTime m_end;
};

/** A list of events. */
typedef QList<Event> EventList;

/** A list of event ids. */
typedef QList<EventId> EventIdList;

/** A mapof events. */
typedef std::map<EventId, Event> EventMap;

void dumpEvents( const EventList& events );

#endif
