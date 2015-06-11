/*
  ViewHelpers.cpp

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2007-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Mirko Boehm <mirko.boehm@kdab.com>

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

#include "ViewHelpers.h"

#include <QtAlgorithms>
#include <QFile>

void Charm::connectControllerAndView( Controller* controller, CharmWindow* view )
{
    // connect view and controller:
    // make controller process commands send by the view:
    QObject::connect( view, SIGNAL(emitCommand(CharmCommand*)),
                      controller, SLOT(executeCommand(CharmCommand*)) );
    QObject::connect( view, SIGNAL(emitCommandRollback(CharmCommand*)),
                      controller, SLOT(rollbackCommand(CharmCommand*)) );
    // make view receive done commands from the controller:
    QObject::connect( controller, SIGNAL(commandCompleted(CharmCommand*)),
                      view, SLOT(commitCommand(CharmCommand*)) );
}

struct StartsEarlier {
    bool operator()( const EventId& leftId, const EventId& rightId ) const {
        const Event& left = DATAMODEL->eventForId( leftId );
        const Event& right = DATAMODEL->eventForId( rightId );
        return left.startDateTime() < right.startDateTime();
    }
};

EventIdList Charm::eventIdsSortedByStartTime( EventIdList ids )
{
    qStableSort( ids.begin(), ids.end(), StartsEarlier() );
    return ids;
}

EventIdList Charm::filteredBySubtree( EventIdList ids, TaskId parent, bool exclude )
{
    EventIdList result;
    bool isParent = false;
    Q_FOREACH( EventId id, ids ) {
        const Event& event = DATAMODEL->eventForId( id );
        isParent = ( parent == event.taskId() || DATAMODEL->isParentOf( parent, event.taskId() ) );
        if ( isParent != exclude ) {
            result << id;
        }
    }
    return result;
}

QString Charm::elidedTaskName( const QString& text, const QFont& font, int width )
{
    QFontMetrics metrics( font );
    const QString& projectCode =
            text.section( ' ', 0, 0, QString::SectionIncludeTrailingSep );
    const int projectCodeWidth = metrics.width( projectCode );
    if ( width > projectCodeWidth ) {
        const QString& taskName = text.section( ' ', 1 );
        const int taskNameWidth = width - projectCodeWidth;
        const QString& taskNameElided =
                metrics.elidedText( taskName, Qt::ElideLeft, taskNameWidth );
        return projectCode + taskNameElided;
    }

    return metrics.elidedText( text, Qt::ElideMiddle, width );
}

QString Charm::reportStylesheet( const QPalette& palette )
{
    QString style;
    QFile stylesheet( ":/Charm/report_stylesheet.sty" );
    if ( stylesheet.open( QIODevice::ReadOnly | QIODevice::Text ) ) {
        style = stylesheet.readAll();
        style.replace(QLatin1String("@header_row_background_color@"), palette.highlight().color().name());
        style.replace(QLatin1String("@header_row_foreground_color@"), palette.highlightedText().color().name());
        style.replace(QLatin1String("@alternate_row_background_color@"), palette.alternateBase().color().name());
        style.replace(QLatin1String("@event_attributes_row_background_color@"), palette.midlight().color().name());
        if ( style.isEmpty() ) {
            qDebug() << "reportStylesheet: default style sheet is empty, too bad";
        }
    } else {
        qDebug() << "reportStylesheet: cannot load report style sheet:" << stylesheet.errorString();
    }
    return style;
}
