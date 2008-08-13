#include <QPainter>
#include <QFont>
#include <QFontMetrics>

#include "TimeTrackingSummaryWidget.h"

const int Margin = 2;

TimeTrackingSummaryWidget::TimeTrackingSummaryWidget( QWidget* parent )
    : QWidget( parent )
{
    // temp:
    WeeklySummary s1;
    s1.task = 1;
    s1.durations << 1000 << 1100 << 1200 << 1300 << 1400 << 1500 << 1600;
    m_summaries << s1 << s1 << s1 << s1;
    //
    m_fixedFont.setFamily( "Andale Mono" );
    m_fixedFont.setPointSize( 10 );
    m_narrowFont.setFamily( "Arial Narrow" );
    m_narrowFont.setPointSize( 10 );
    //
    setFont( m_fixedFont );
    setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed );
}

QSize TimeTrackingSummaryWidget::sizeHint() const
{
    if ( ! m_cachedSizeHint.isValid() ) {
        // the sizeHint is like the minimum size hint, only it allows
        // for more text in the task name column
        const QFontMetrics narrowFontMetrics = QFontMetrics( m_narrowFont );
        const QRect textRect =
            narrowFontMetrics.boundingRect( tr( "moremoremoremoremore" ) );
        const int widthHint = minimumSizeHint().width() + textRect.width();
        m_cachedSizeHint = QSize( widthHint, minimumSizeHint().height() );
    }
    return m_cachedSizeHint;
}

QSize TimeTrackingSummaryWidget::minimumSizeHint() const
{
    if ( ! m_cachedMinimumSizeHint.isValid() ) {
        const QFontMetrics fixedFontMetrics = QFontMetrics( m_fixedFont );
        const QFontMetrics narrowFontMetrics = QFontMetrics( m_narrowFont );
        const QRect totalsColumnFieldRect(
            fixedFontMetrics.boundingRect( "100:00" )
            .adjusted( 0, 0, 2 * Margin, 2 * Margin ) );
        const QRect dayColumnRect(
            fixedFontMetrics.boundingRect( "99:99" )
            .adjusted( 0, 0, 2 * Margin, 2 * Margin ) );
        const int fieldHeight = qMax( fixedFontMetrics.lineSpacing(),
                                      narrowFontMetrics.lineSpacing() )
                                + 2 * Margin;
        const QRect taskColumnFieldRect = narrowFontMetrics.boundingRect(
            tr( "KDABStuffngy" ) )
            .adjusted( 0, 0, 2 * Margin, 2 * Margin );
        const int minimumWidth =
            totalsColumnFieldRect.width()
            + 7 * dayColumnRect.width()
            + taskColumnFieldRect.width();
        const int minimumHeight = rowCount() * fieldHeight;
        m_cachedMinimumSizeHint = QSize( minimumWidth, minimumHeight );
        m_cachedTotalsFieldRect = QRect( 0, 0, totalsColumnFieldRect.width(), fieldHeight );
        m_cachedDayFielRect = QRect( 0, 0, dayColumnRect.width(), fieldHeight );
    }
    return m_cachedMinimumSizeHint;
}

void TimeTrackingSummaryWidget::paintEvent( QPaintEvent* e )
{
    const int FieldHeight = m_cachedTotalsFieldRect.height();
    QPainter painter( this );
    // all attributes are determined in data(), we just paint the rects:
    for ( int row = 0; row < rowCount() - 1; ++row ) {
        for ( int column = 0; column < columnCount(); ++column ) {
            int alignment = Qt::AlignRight | Qt::AlignVCenter;
            if ( row == 0 ) {
                alignment = Qt::AlignCenter | Qt::AlignVCenter;
            } else if ( column == 0 && row < rowCount() - 1 ) {
                alignment = Qt::AlignLeft | Qt::AlignVCenter;
            }
            QRect fieldRect;
            const int y = row * FieldHeight;
            if ( column == columnCount() - 1 ) { // totals column
                fieldRect = QRect( width() - m_cachedTotalsFieldRect.width(), y,
                                   m_cachedTotalsFieldRect.width(), FieldHeight );
            } else if ( column == 0 ) { // task column
                const int fieldWidth = width() - m_cachedTotalsFieldRect.width()
                                       - 7 * m_cachedDayFielRect.width();
                fieldRect = QRect( 0, y, fieldWidth, FieldHeight );
            } else if ( column > 0 ) { //  a task
                fieldRect = QRect( width() - m_cachedTotalsFieldRect.width()
                                   - column * m_cachedDayFielRect.width(), y,
                                   m_cachedDayFielRect.width(), FieldHeight );
            }
            const QRect textRect = fieldRect.adjusted( Margin, Margin, -Margin, -Margin );
            const DataField field = data( column, row );
            painter.setBrush( field.background );
            painter.setPen( Qt::NoPen );
            painter.drawRect( fieldRect );
            painter.setPen( field.pen );
            painter.drawText( textRect, alignment, field.text );
        }
    }
    // paint the tracking row
    const QRect fieldRect( 0, ( rowCount() - 1 ) * FieldHeight, width(), FieldHeight );
    const QRect textRect = fieldRect.adjusted( Margin, Margin, -Margin, -Margin );
    const DataField field = data( 0, rowCount() - 1 );
    painter.setBrush( field.background );
    painter.setPen( Qt::NoPen );
    painter.drawRect( fieldRect );
    painter.setPen( field.pen );
    painter.drawText( textRect, Qt::AlignLeft | Qt::AlignVCenter, field.text );
}

TimeTrackingSummaryWidget::DataField TimeTrackingSummaryWidget::data( int column, int row )
{
    const QBrush& HeaderBrush = palette().mid();
    const QBrush& TaskBrushEven = palette().light();
    const QBrush& TaskBrushOdd = palette().midlight();
    const QBrush& TotalsRowBrush = HeaderBrush;
    const int HeaderRow = 0;
    const int TotalsRow = rowCount() - 2;
    const int TrackingRow = rowCount() - 1;
    const int TaskColumn = 0;
    const int TotalsColumn = columnCount() - 1;

    DataField field;
    if ( row == HeaderRow ) {
        field.pen = QPen( Qt::black );
        if ( column == TaskColumn ) {
            field.text = tr( "Task" );
        } else if ( column == TotalsColumn ) {
            field.text = tr( "Total" );
        } else {
            field.text = QDate::shortDayName( column );
        }
        field.background = HeaderBrush;
    } else if ( row == TotalsRow ) {
        field.pen = QPen( Qt::black );
        if ( column == TaskColumn ) {
            field.text = tr( "Total" );
        } else if ( column == TotalsColumn ) {
            field.text = tr( "42:11" );
        } else {
            field.text = tr( "00:11" );
        }
        field.background = TotalsRowBrush;
    } else if ( row == TrackingRow ) {
        // we only return one value, the paint method will treat this
        // column as a special case
        field.pen = QPen( Qt::black );
        field.background = TaskBrushOdd;
        field.text = tr( " 00:45 2345 KDAB/HR/Project Time Bookkeeping" );
    } else { // a task row
        field.pen = QPen( Qt::black );
        if ( column == TaskColumn ) {
            field.text = tr( "1234" );
            field.background = HeaderBrush;
        } else if ( column == TotalsColumn ) {
            field.text = tr( "(sum)" );
            field.background = HeaderBrush;
        } else {
            field.text = tr( "00:01" );
            field.background = row % 2 ? TaskBrushEven : TaskBrushOdd;
        }
    }

    return field;
}

#include "TimeTrackingSummaryWidget.moc"
