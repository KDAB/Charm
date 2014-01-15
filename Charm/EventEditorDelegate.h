#ifndef EVENTEDITORDELEGATE_H
#define EVENTEDITORDELEGATE_H

#include <QSize>
#include <QItemDelegate>

class QPainter;
class QStyleOptionViewItem;
class QModelIndex;
class EventModelFilter;

class EventEditorDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    enum EventState {
        EventState_Default,
        EventState_Locked,
        EventState_Dirty
    };

    explicit EventEditorDelegate( EventModelFilter* model,
                                  QObject* parent = nullptr );
    // ~EventEditorDelegate();

    // reimpl
    QSize sizeHint( const QStyleOptionViewItem&, const QModelIndex& ) const;
    // reimpl
    void paint( QPainter*, const QStyleOptionViewItem&, const QModelIndex& ) const;

private:
    EventModelFilter* m_model;
    mutable QSize m_cachedSizeHint;

    // paint the values into the painter at the given rectangle, return the
    // bounding rectangle
    // (factored out to use the same implementation for the size hint
    // and the painting during paintEvent)
    QRect paint( QPainter*, const QStyleOptionViewItem& option,
                 const QString& taskName, const QString& timespan,
                 double logDuration, EventState state ) const;

    // calculate the length for a  visual representation of the event duration
    double logDuration( int seconds ) const;
};

#endif
