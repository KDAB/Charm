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
    setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    setFixedSize( 48 * 9, 2 * 40 );
    QFont myfont = font();
    myfont.setFamily( "Andale Mono" );
    myfont.setPointSize( 10 );
    setFont( myfont );
    QFontMetrics metrics( myfont );
    QRect fieldRect = metrics.boundingRect( tr( "T00:yg" ) ).adjusted( 0, 0, 2 * Margin, 2 * Margin );
    setFixedSize( columnCount() * fieldRect.width(), rowCount() * fieldRect.height() + 20);
}

void TimeTrackingSummaryWidget::paintEvent( QPaintEvent* e )
{
    const int FieldWidth = width() / columnCount();
    const int FieldHeight = height() / rowCount();

    QPainter painter( this );
    // all attributes are determined in data(), we just paint the rects:
    for ( int column = 0; column < columnCount(); ++column ) {
        for ( int row = 0; row < rowCount() - 1; ++row ) {
            const QRect fieldRect( column * FieldWidth, row * FieldHeight, FieldWidth, FieldHeight );
            const QRect textRect = fieldRect.adjusted( Margin, Margin, -2*Margin, -2*Margin );
            const DataField field = data( column, row );
            painter.setBrush( field.background );
            painter.setPen( Qt::NoPen );
            painter.drawRect( fieldRect );
            painter.setPen( field.pen );
            painter.drawText( textRect, Qt::AlignRight | Qt::AlignVCenter, field.text );
        }
    }
    // paint the tracking row
    const QRect fieldRect( 0, ( rowCount() - 1 ) * FieldHeight, width(), FieldHeight );
    const QRect textRect = fieldRect.adjusted( Margin, Margin, -2*Margin, -2*Margin );
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

