#include <QtDebug>
#include <QPainter>

#include "Data.h"
#include "Event.h"
#include "Charm.h"
#include "EventModelFilter.h"
#include "EventEditorDelegate.h"

EventEditorDelegate::EventEditorDelegate( EventModelFilter* model,
                                          QObject* parent )
    : QItemDelegate( parent )
    , m_model( model )
{
}

QSize EventEditorDelegate::sizeHint( const QStyleOptionViewItem& option,
                                     const QModelIndex& index ) const
{
    // to have the size hint recalculated, simply set m_cachedSizeHint
    // to an invalid value (m_cachedSizeHint = QSize();)
    if ( ! m_cachedSizeHint.isValid() ) {
        // make up event settings and calculate the space they need:
        QPixmap pixmap( 10000, 10000 ); // temp
        QPainter painter( &pixmap );
        QStyleOptionViewItem fakeOption ( option );
        fakeOption.rect.setSize( pixmap.size() );
        const QString task ( tr( "KDAB/Programming" ) );
        QString dateAndDuration;
        QTextStream stream( &dateAndDuration );
        QDate date = QDate::currentDate();
        QTime time = QTime::currentTime();
        stream << date.toString( Qt::SystemLocaleDate )
               << " " << time.toString( Qt::SystemLocaleDate )
               << " " << hoursAndMinutes( 3654 );
        const QString comment( tr( "- implement feature A, plus fix #2345" ) );
        m_cachedSizeHint = paint( &painter, fakeOption,
                                  task, dateAndDuration,
                                  comment, EventState_Locked ).size();
    }
    return m_cachedSizeHint;
}

void EventEditorDelegate::paint( QPainter* painter,
                                 const QStyleOptionViewItem& option,
                                 const QModelIndex& index ) const
{
    const Event& event = m_model->eventForIndex( index );
    Q_ASSERT( event.isValid() );
    const TaskTreeItem& item = DATAMODEL->taskTreeItem( event.taskId() );

    if ( event.isValid() ) {
        bool locked = DATAMODEL->isEventActive( event.id() );
        QString dateAndDuration;
        QTextStream stream( &dateAndDuration );
        QDate date = event.startDateTime().date();
        QTime time = event.startDateTime().time();
        stream << date.toString( Qt::SystemLocaleDate )
               << " " << time.toString( Qt::SystemLocaleDate )
               << " " << hoursAndMinutes( event.duration() );
        paint( painter, option,
               tasknameWithParents( item.task() ),
               dateAndDuration,
               tr( "- implement feature A, plus fix #2345" ), //tmp
               locked ? EventState_Locked : EventState_Default );
    }
}

QRect EventEditorDelegate::paint( QPainter* painter,
                                  const QStyleOptionViewItem& option,
                                  const QString& taskName,
                                  const QString& timespan,
                                  const QString& comment,
                                  EventState state ) const
{
    painter->save();
    const QPalette& palette = qApp->palette();
    QFont mainFont = painter->font();
    QFont detailFont ( mainFont );
    detailFont.setPointSizeF( mainFont.pointSizeF() * 0.8 );
    QPixmap decoration;
    QColor foreground;
    QColor background;

    switch( state ) {
    case EventState_Locked:
        decoration = Data::editorLockedPixmap();
        foreground = palette.color( QPalette::Disabled, QPalette::WindowText );
        background = palette.color( QPalette::Disabled, QPalette::Window );
        break;
    case EventState_Dirty:
        decoration = Data::editorDirtyPixmap();
        foreground = palette.color( QPalette::Active, QPalette::WindowText );
        background = palette.color( QPalette::Active, QPalette::Window );
        break;
    case EventState_Default:
    default:
        foreground = palette.color( QPalette::Active, QPalette::WindowText );
        background = palette.color( QPalette::Active, QPalette::Window );
        break;
    };

    if ( option.state & QStyle::State_Selected ) {
        QBrush brush( palette.color( QPalette::Active, QPalette::Highlight ) );
        painter->setBrush( brush );
        painter->setPen( Qt::NoPen );
        painter->drawRect( option.rect );
        if ( state != EventState_Locked ) {
            foreground = palette.color(
                QPalette::Active, QPalette::HighlightedText );
        }
    }

    painter->setPen( foreground );

    // draw line 1 and decoration:
    painter->setFont( mainFont );
    QRect taskRect;
    taskRect.setTopLeft( option.rect.topLeft() );
    taskRect.setWidth( option.rect.width() - decoration.width() );
    taskRect.setHeight( option.rect.height() );
    QPoint decorationPoint ( option.rect.width() - decoration.width(),
                             option.rect.top() );

    QRect boundingRect;
    QFontMetrics metrics( mainFont );
    QString elidedTaskName = metrics.elidedText( taskName, Qt::ElideMiddle,
                                                 taskRect.width() );
    painter->drawText( taskRect, Qt::AlignLeft | Qt::AlignTop,
                       elidedTaskName, &boundingRect );
    taskRect.setSize( boundingRect.size() );
    taskRect.setHeight( qMax( taskRect.height(), decoration.height() ) );
    // now taskRect tells us where to start line 2
    painter->drawPixmap( decorationPoint, decoration );

    // draw line 2 (timespan and comment, partly):
    painter->setFont( detailFont );
    QRect detailsRect;
    detailsRect.setTopLeft( QPoint( taskRect.topLeft().x(),
                                    taskRect.topLeft().y() + taskRect.height() ) );
    detailsRect.setWidth( option.rect.width() );
    detailsRect.setHeight( option.rect.height() - taskRect.height() );
    painter->drawText( detailsRect, Qt::AlignLeft | Qt::AlignTop,
                       timespan, &boundingRect );
    detailsRect.setSize( boundingRect.size() );

    painter->restore();
    // return bounding rectangle
    return QRect( 0, 0,
                  qMax( taskRect.width(), detailsRect.width() ),
                  taskRect.height() + detailsRect.height() );

}

#include "EventEditorDelegate.moc"
