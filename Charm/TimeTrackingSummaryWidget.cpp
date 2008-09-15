#include <numeric>
#include <functional>

#include <QPainter>
#include <QFont>
#include <QFontMetrics>
#include <QToolButton>

#include "Data.h"
#include "Core/CharmConstants.h"
#include "TimeTrackingSummaryWidget.h"

const int Margin = 2;

TimeTrackingSummaryWidget::TimeTrackingSummaryWidget( QWidget* parent )
    : QWidget( parent )
    , m_stopGoButton( this )
    , m_taskSelector( this )
{
    // FIXME use platform defined, hand-picked fonts, so far those have been selected for Mac:
    m_fixedFont.setFamily( "Andale Mono" );
    m_fixedFont.setPointSize( 12 );
    m_narrowFont = font(); // stay with the desktop
    m_narrowFont.setPointSize( 12);
    //
    setSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed );
    // plumbing
    m_stopGoButton.setCheckable( true );
    connect( &m_stopGoButton, SIGNAL( toggled( bool ) ),
             SLOT( slotGoStopToggled( bool ) ) );
    m_stopGoButton.setChecked( false );
    slotGoStopToggled( false );
    m_stopGoButton.setPopupMode( QToolButton::InstantPopup );
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
        // the header row, task rows, and totals row are all of the same height
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
        // the tracking row needs to accomodate a tool button:
        const QSize buttonSizeHint = m_stopGoButton.sizeHint();
        const int trackingRowHeight = qMax( fieldHeight, buttonSizeHint.height() + 2 * Margin );

        const int minimumWidth =
            totalsColumnFieldRect.width()
            + 7 * dayColumnRect.width()
            + taskColumnFieldRect.width();
        const int minimumHeight = ( rowCount() - 1 ) * fieldHeight + trackingRowHeight;
        m_cachedMinimumSizeHint = QSize( minimumWidth, minimumHeight );
        m_cachedTotalsFieldRect = QRect( 0, 0, totalsColumnFieldRect.width(), fieldHeight );
        m_cachedDayFieldRect = QRect( 0, 0, dayColumnRect.width(), fieldHeight );
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
            DataField field = data( column, row );
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
                                       - 7 * m_cachedDayFieldRect.width();
                fieldRect = QRect( 0, y, fieldWidth, FieldHeight );
                QFontMetrics metrics( field.font );
                field.text = metrics.elidedText( field.text, Qt::ElideMiddle,
                                                 fieldRect.width() - 2*Margin );
            } else if ( column > 0 ) { //  a task
                fieldRect = QRect( width() - m_cachedTotalsFieldRect.width()
                                   - 8 * m_cachedDayFieldRect.width()
                                   + column * m_cachedDayFieldRect.width(), y,
                                   m_cachedDayFieldRect.width(), FieldHeight );
            }
            const QRect textRect = fieldRect.adjusted( Margin, Margin, -Margin, -Margin );
            painter.setBrush( field.background );
            painter.setPen( Qt::NoPen );
            painter.drawRect( fieldRect );
            painter.setPen( palette().text().color() );
            painter.setFont( field.font );
            painter.drawText( textRect, alignment, field.text );
        }
    }
    // paint the tracking row
    const int left = m_stopGoButton.geometry().right() + Margin;
    const int top = ( rowCount() - 1 ) * FieldHeight;
    const QRect fieldRect( 0, top, width(), height() - top );
    const QRect textRect = fieldRect.adjusted( left + Margin, Margin, -Margin, -Margin );
    const DataField field = data( 0, rowCount() - 1 );
    painter.setBrush( field.background );
    painter.setPen( Qt::NoPen );
    painter.drawRect( fieldRect );
//     painter.setPen( palette().text().color() );
//     painter.drawText( textRect, Qt::AlignLeft | Qt::AlignVCenter, field.text );
}

void TimeTrackingSummaryWidget::resizeEvent( QResizeEvent* )
{
    sizeHint(); // make sure cached values are updated
    m_stopGoButton.resize( m_stopGoButton.sizeHint() );
    m_stopGoButton.move( Margin, height() - Margin - m_stopGoButton.height() );
    const int left = m_stopGoButton.geometry().right() + Margin;
    const int remainingWidth = width() - Margin - left;
    const QRect selectorGeometry( left, m_stopGoButton.geometry().top(),
                                  remainingWidth, m_stopGoButton.height() );
    m_taskSelector.setGeometry( selectorGeometry );
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
    field.font = m_fixedFont;
    if ( row == HeaderRow ) {
        if ( column == TaskColumn ) {
            field.text = tr( "Task" );
        } else if ( column == TotalsColumn ) {
            field.text = tr( "Total" );
        } else {
            field.text = QDate::shortDayName( column );
        }
        field.background = HeaderBrush;
    } else if ( row == TotalsRow ) {
        if ( column == TaskColumn ) {
            // field.text = tr( "Total" );
        } else if ( column == TotalsColumn ) {
            int total = 0;
            Q_FOREACH( const WeeklySummary& s, m_summaries ) {
                total += std::accumulate( s.durations.begin(), s.durations.end(), 0 );
            }
            field.text = hoursAndMinutes( total );
        } else {
            int day = column - 1;
            int total = 0;
            Q_FOREACH( const WeeklySummary& s, m_summaries ) {
                total += s.durations[day];
            }
            field.text = hoursAndMinutes( total );
        }
        field.background = TotalsRowBrush;
    } else if ( row == TrackingRow ) {
        // we only return one value, the paint method will treat this
        // column as a special case
        field.background = TaskBrushOdd;

        field.text = tr( " 00:45 2345 KDAB/HR/Project Time Bookkeeping" );
    } else { // a task row
        field.background = row % 2 ? TaskBrushEven : TaskBrushOdd;
        if ( m_summaries.size() > row - 1 ) {
            int index = row - 1; // index into summaries
            int day = column - 1;
            if ( column == TaskColumn ) {
                field.text = m_summaries[index].taskname;
                field.font = m_narrowFont;
            } else if ( column == TotalsColumn ) {
                const QVector<int>& durations = m_summaries[index].durations;
                const int total = std::accumulate( durations.begin(), durations.end(), 0 );
                field.text = hoursAndMinutes( total );
                // field.background = TaskBrushOdd;
            } else {
                int duration = m_summaries[index].durations[day];
                field.text = duration > 0 ? hoursAndMinutes( duration) : QString();
                field.background = row % 2 ? TaskBrushEven : TaskBrushOdd;
            }
        }
    }

    return field;
}

void TimeTrackingSummaryWidget::setSummaries( QVector<WeeklySummary> s )
{
    m_summaries = s;
    if ( m_summaries.isEmpty() ) {
        m_taskSelector.setEnabled( false );
    } else {
        m_taskSelector.setEnabled( true );
        m_taskSelector.setText( m_summaries.last().taskname );
    }
    m_cachedMinimumSizeHint = QSize();
    m_cachedSizeHint = QSize();
    updateGeometry();
    update();
    emit maybeShrink();
}

void TimeTrackingSummaryWidget::slotGoStopToggled( bool on )
{
    if ( on ) {
        m_stopGoButton.setIcon( Data::recorderStopIcon() );
        m_stopGoButton.setText( tr( "Stop" ) );
        m_taskSelector.setEnabled( false );
    } else {
        m_stopGoButton.setIcon( Data::recorderGoIcon() );
        m_stopGoButton.setText( tr( "Start" ) );
        m_taskSelector.setEnabled( true );
    }
}


#include "TimeTrackingSummaryWidget.moc"
