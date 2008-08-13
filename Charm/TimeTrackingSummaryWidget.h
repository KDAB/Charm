#ifndef TIMETRACKINGSUMMARYWIDGET_H
#define TIMETRACKINGSUMMARYWIDGET_H

#include <QWidget>
#include <QVector>
#include <QPen>

#include "Core/Task.h"

#define DAYS_IN_WEEK 7

class TimeTrackingSummaryWidget : public QWidget
{
    Q_OBJECT
    struct WeeklySummary {
        TaskId task;
        QVector<int> durations;
        WeeklySummary() : task( 0 ), durations( DAYS_IN_WEEK, 0 ) {}
    };
    struct DataField {
        QString text;
        QBrush background;
        QPen pen;
    };

public:
    explicit TimeTrackingSummaryWidget( QWidget* parent = 0 );
    void paintEvent( QPaintEvent* );

private:
    DataField data( int column, int row );
    int columnCount() const { return 9; }
    int rowCount() const { return m_summaries.count() + 3; }
    QVector<WeeklySummary> m_summaries;
};

#endif
