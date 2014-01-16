#ifndef TimeTrackingView_H
#define TimeTrackingView_H

#include <QWidget>
#include <QVector>
#include <QPen>
#include <QMenu>
#include <QToolButton>
#include <QTimeLine>

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
    explicit TimeTrackingView( QToolBar* toolBar, QWidget* parent = 0 );
    void paintEvent( QPaintEvent* );
    void resizeEvent( QResizeEvent* );
    void mousePressEvent( QMouseEvent* event );
    void mouseDoubleClickEvent( QMouseEvent * event );

    void setSummaries( const QVector<WeeklySummary>& summaries );
    QSize sizeHint() const;
    QSize minimumSizeHint() const;
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
