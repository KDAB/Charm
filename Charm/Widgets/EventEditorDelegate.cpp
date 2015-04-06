/*
  EventEditorDelegate.cpp

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2014-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Frank Osterfeld <frank.osterfeld@kdab.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "EventEditorDelegate.h"
#include "Data.h"
#include "EventModelFilter.h"
#include "ViewHelpers.h"

#include "Core/CharmConstants.h"
#include "Core/Event.h"

#include <QPainter>

#include <cmath>

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
        QPixmap pixmap( 2000, 800 ); // temp
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
        m_cachedSizeHint = paint( &painter, fakeOption,
                                  task, dateAndDuration,
                                  42, EventState_Locked ).size();
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
        QTextStream dateStream( &dateAndDuration );
        QDate date = event.startDateTime().date();
        QTime time = event.startDateTime().time();
        QTime endTime = event.endDateTime().time();
        dateStream << date.toString( Qt::SystemLocaleDate )
               << " " << time.toString( "h:mm" )
               << " - " << endTime.toString( "h:mm" )
               << " (" << hoursAndMinutes( event.duration() ) << ") Week "
               << date.weekNumber();

        QString taskName;
        QTextStream taskStream( &taskName );
        // print leading zeroes for the TaskId
        const int taskIdLength = CONFIGURATION.taskPaddingLength;
        taskStream << QString( "%1" ).arg( item.task().id(), taskIdLength, 10, QChar( '0' ) )
                   << " " << DATAMODEL->smartTaskName( item.task() );

        paint( painter, option,
               taskName,
               dateAndDuration,
               logDuration( event.duration() ),
               locked ? EventState_Locked : EventState_Default );
    }
}

QRect EventEditorDelegate::paint( QPainter* painter,
                                  const QStyleOptionViewItem& option,
                                  const QString& taskName,
                                  const QString& timespan,
                                  double logDuration,
                                  EventState state ) const
{
    painter->save();
    const QPalette& palette = option.palette;
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
                             option.rect.top() + ( option.rect.height() - decoration.height() ) / 2 );

    QRect boundingRect;
    QString elidedTask = Charm::elidedTaskName( taskName, mainFont, taskRect.width() );
    painter->drawText( taskRect, Qt::AlignLeft | Qt::AlignTop, elidedTask,
                       &boundingRect );
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

    // draw the duration line:
    const int Margin = 2;
    QRect durationRect( option.rect.left() + 1, detailsRect.bottom(),
                static_cast<int>( logDuration * ( option.rect.width() - 2 ) ), Margin  );
    painter->setBrush( palette.dark() );
    painter->setPen( Qt::NoPen );
    painter->drawRect( durationRect );

    painter->restore();
    // return bounding rectangle
    return QRect( 0, 0,
                  qMax( taskRect.width(), detailsRect.width() ),
                  durationRect.bottom() + 1 - option.rect.top() );

}

double EventEditorDelegate::logDuration( int duration ) const
{   // we rely on the compiler to optimize at compile time :-)
        if( duration <= 0) {
                return 0;
        }
        if( duration <= 3600 ) {
                return 0.2 * 1.0 / 3600.0 * duration;
        } else {
                const double log2 = std::log( 2.0 );
                const double hours = 1.0 / 3600 * duration;
                const double value = log( hours ) / log2;
                return 0.2 * ( 1.0 + value );
        }
}

#include "moc_EventEditorDelegate.cpp"
