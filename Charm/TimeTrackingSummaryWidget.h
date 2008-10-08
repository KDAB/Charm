#ifndef TIMETRACKINGSUMMARYWIDGET_H
#define TIMETRACKINGSUMMARYWIDGET_H

#include <QWidget>
#include <QVector>
#include <QPen>
#include <QMenu>
#include <QToolButton>

#include "Core/Task.h"

#define DAYS_IN_WEEK 7

class TimeTrackingSummaryWidget : public QWidget
{
    Q_OBJECT
public:
    struct WeeklySummary {
        TaskId task;
        QString taskname;
        QVector<int> durations;
        WeeklySummary() : task( 0 ), durations( DAYS_IN_WEEK, 0 ) {}
    };
private:
    struct DataField {
        DataField() : hasHighlight( false ) {}
        QString text;
        QBrush background;
        bool hasHighlight; // QBrush does not have isValid()
        QBrush highlight;
        QFont font;
    };

public:
    explicit TimeTrackingSummaryWidget( QWidget* parent = 0 );
    void paintEvent( QPaintEvent* );
    void resizeEvent( QResizeEvent* );

    void setSummaries( QVector<WeeklySummary> );
    QSize sizeHint() const;
    QSize minimumSizeHint() const;

    void handleActiveEvents();

signals:
    void maybeShrink();

signals:
    void startEvent( TaskId );
    void stopEvent();

private slots:
    void slotGoStopToggled( bool );
    void slotActionSelected( QAction* );

private:
    DataField data( int column, int row );
    int columnCount() const { return 9; }
    int rowCount() const { return qMax( 6, m_summaries.count() ) + 3; }
    QVector<WeeklySummary> m_summaries;
    mutable QSize m_cachedSizeHint;
    mutable QSize m_cachedMinimumSizeHint;
    mutable QRect m_cachedTotalsFieldRect;
    mutable QRect m_cachedDayFieldRect;
    mutable QFont m_fixedFont;
    mutable QFont m_narrowFont;
    QToolButton m_stopGoButton;
    QToolButton m_taskSelector;
    QMenu m_menu;
    QList<QAction*> m_currentActions;
    int m_selectedSummary;
};

#endif
