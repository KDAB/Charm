/*
  TimeTrackingView.h

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

#ifndef TimeTrackingView_H
#define TimeTrackingView_H

#include <QWidget>
#include <QVector>
#include <QMenu>

#include "Core/Task.h"
#include "TimeTrackingTaskSelector.h"

class QPalette;
class QToolBar;

class TimeTrackingView : public QWidget
{
    Q_OBJECT
private:
    struct DataField {
        DataField() : hasHighlight( false ), storeAsActive( false ) {}
        QString text;
        QBrush background;
        bool hasHighlight; // QBrush does not have isValid()
        bool storeAsActive;
        QBrush highlight;
        QFont font;
    };

    /** A struct to store the attributes used during painting.
      The initialize function has and this class have been factored out for performance reasonsduring profiling. */
    struct PaintAttributes {
        QBrush headerBrush;
        QBrush taskBrushEven;
        QBrush taskBrushOdd;
        QBrush totalsRowBrush;
        QBrush totalsRowEvenDayBrush;
        QBrush headerEvenDayBrush;
        QBrush halfHighlight;
        QColor runningTaskColor;
        float dim;
        void initialize( const QPalette& palette );
    };

public:
    explicit TimeTrackingView( QWidget* parent = nullptr );
    void paintEvent( QPaintEvent* ) override;
    void resizeEvent( QResizeEvent* ) override;
    void mousePressEvent( QMouseEvent* event ) override;
    void mouseDoubleClickEvent( QMouseEvent * event ) override;

    void setSummaries( const QVector<WeeklySummary>& summaries );
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;
    QMenu* menu() const;

    void populateEditMenu( QMenu* );

    void handleActiveEvents();

    bool isTracking() const;

    void configurationChanged();

signals:
    void maybeShrink();
    void startEvent( TaskId );
    void stopEvents();

private slots:
    void slotUpdateSummaries();

private:
    void data( DataField& out, int column, int row ) const;
    int columnCount() const { return 9; }
    int rowCount() const { return qMax( 6, m_summaries.count() ) + 3; }
    int getSummaryAt( const QPoint& position );
    bool taskIsValidAndTrackable( int taskId );

    int taskColumnWidth() const;

    QVector<WeeklySummary> m_summaries;
    mutable QSize m_cachedSizeHint;
    mutable QSize m_cachedMinimumSizeHint;
    mutable QRect m_cachedTotalsFieldRect;
    mutable QRect m_cachedDayFieldRect;
    mutable QFont m_fixedFont;
    mutable QFont m_narrowFont;
    TimeTrackingTaskSelector* m_taskSelector;
    QList<QRect> m_activeFieldRects;
    PaintAttributes m_paintAttributes;
    DataField m_defaultField;
    /** Stored for performance reasons, QDate::currentDate() is expensive. */
    int m_dayOfWeek;
    /** Stored for performance reasons, QDate::shortDayName() is slow on Mac. */
    QString m_shortDayNames[7];
    /** Stored for performance reasons, QFontMetrics::elidedText is slow if called many times. */
    QMap<QString, QString> m_elidedTexts;
    QString elidedText( const QString& text, const QFont& font, int width );
};

#endif
