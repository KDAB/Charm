#ifndef TIMETRACKINGSUMMARYWIDGET_H
#define TIMETRACKINGSUMMARYWIDGET_H

#include <QWidget>
#include <QVector>
#include <QPen>
#include <QMenu>
#include <QToolButton>
#include <QTimeLine>

#include "Core/Task.h"
#include "TimeTrackingTaskSelector.h"

class QPalette;

class TimeTrackingSummaryWidget : public QWidget
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
        QColor pulseColor;
        float dim;
        void initialize( const QPalette& palette );
    };

public:
    explicit TimeTrackingSummaryWidget( QWidget* parent = 0 );
    void paintEvent( QPaintEvent* );
    void resizeEvent( QResizeEvent* );
    void mousePressEvent( QMouseEvent* event );
    void mouseDoubleClickEvent( QMouseEvent * event );

    void setSummaries( QVector<WeeklySummary> );
    QSize sizeHint() const;
    QSize minimumSizeHint() const;

    void handleActiveEvents();

    bool isTracking() const;

signals:
    void maybeShrink();
    void startEvent( TaskId );
    void stopEvent();

private slots:
    void slotPulseValueChanged( qreal );

private:
    void data( DataField& out, int column, int row );
    int columnCount() const { return 9; }
    int rowCount() const { return qMax( 6, m_summaries.count() ) + 3; }
    int getSummaryAt( const QPoint& position );

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
    QTimeLine m_pulse;
    PaintAttributes m_paintAttributes;
    DataField m_defaultField;
    /** Stored for performance reasons, QDate::currentDate() is expensive. */
    int m_dayOfWeek;
};

#endif
