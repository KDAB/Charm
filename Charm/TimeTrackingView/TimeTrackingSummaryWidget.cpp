#include <numeric>
#include <functional>
#include <algorithm>

#include <QPainter>
#include <QFont>
#include <QFontMetrics>
#include <QToolButton>
#include <QPaintEvent>

#include "Core/CharmConstants.h"

#include "Data.h"
#include "ViewHelpers.h"
#include "TimeTrackingSummaryWidget.h"

const int Margin = 2;

TimeTrackingSummaryWidget::TimeTrackingSummaryWidget( QWidget* parent )
    : QWidget( parent )
    , m_taskSelector( new TimeTrackingTaskSelector( this ) )
    , m_dayOfWeek( 0 )
{
#ifdef Q_WS_MAC
    m_fixedFont.setFamily( "Andale Mono" );
    m_fixedFont.setPointSize( 11 );
    m_narrowFont = font(); // stay with the desktop
    m_narrowFont.setPointSize( 11 );
#else
    m_fixedFont = font();
    m_fixedFont.setPointSizeF( 0.9 * m_fixedFont.pointSizeF() );
    m_narrowFont = m_fixedFont;
#endif
    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
    // plumbing
    m_pulse.setLoopCount( 0 );
    m_pulse.setDuration( 2000 );
    m_paintAttributes.initialize( palette() );
    m_pulse.setCurveShape( QTimeLine::SineCurve );
    for( int i = 0; i < 7; ++i ) {
        m_shortDayNames[i] = QDate::shortDayName( i + 1 );
    }
    connect( &m_pulse, SIGNAL( valueChanged( qreal ) ),
             SLOT( slotPulseValueChanged( qreal ) ) );
    connect( m_taskSelector, SIGNAL( startEvent( TaskId ) ),
             SIGNAL( startEvent( TaskId ) ) );
    connect( m_taskSelector, SIGNAL( stopEvent( TaskId ) ),
             SIGNAL( stopEvent() ) );
    connect( m_taskSelector, SIGNAL( updateSummariesPlease() ),
             SLOT( slotUpdateSummaries() ) );
}

void TimeTrackingSummaryWidget::PaintAttributes::initialize( const QPalette& palette ) {
    headerBrush = palette.mid();
    taskBrushEven = palette.light();
    taskBrushOdd = palette.midlight();
    totalsRowBrush = headerBrush;
    totalsRowEvenDayBrush = QBrush( taskBrushEven.color().darker(125) );
    headerEvenDayBrush = totalsRowEvenDayBrush;
    QColor dimHighlight = palette.highlight().color();
    dim = 0.25;
    dimHighlight.setAlphaF( dim * dimHighlight.alphaF() );
    const QBrush halfHighlight( dimHighlight );
    pulseColor = palette.highlight().color();
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
        const QFontMetrics fixedFontMetrics( m_fixedFont );
        const QFontMetrics narrowFontMetrics( m_narrowFont );
        const QRect totalsColumnFieldRect(
            fixedFontMetrics.boundingRect( "100:00" )
            .adjusted( 0, 0, 2 * Margin, 2 * Margin ) );
        const int dayWidth = fixedFontMetrics.width( "00:00" ) + 2 * Margin;
        const int fieldHeight = qMax( fixedFontMetrics.lineSpacing(),
                                      narrowFontMetrics.lineSpacing() )
                                + 2 * Margin;
        const QRect taskColumnFieldRect = narrowFontMetrics.boundingRect(
            tr( "KDABStuffngy" ) )
                                          .adjusted( 0, 0, 2 * Margin, 2 * Margin );
        // the tracking row needs to accomodate the task selector widget
        const QSize taskSelectorSizeHint = m_taskSelector->sizeHint();
        const int trackingRowHeight = qMax( fieldHeight, taskSelectorSizeHint.height() + 2 * Margin );

        const int minimumWidth =
            totalsColumnFieldRect.width()
            + 7 * dayWidth
            + taskColumnFieldRect.width();
        const int minimumHeight = ( rowCount() - 1 ) * fieldHeight + trackingRowHeight;
        m_cachedMinimumSizeHint = QSize( minimumWidth, minimumHeight );
        m_cachedTotalsFieldRect = QRect( 0, 0, totalsColumnFieldRect.width(), fieldHeight );
        m_cachedDayFieldRect = QRect( 0, 0, dayWidth, fieldHeight );
    }
    return m_cachedMinimumSizeHint;
}

void TimeTrackingSummaryWidget::paintEvent( QPaintEvent* e )
{
    m_activeFieldRects.clear();
    const int FieldHeight = m_cachedTotalsFieldRect.height();
    QPainter painter( this );
    // all attributes are determined in data(), we just paint the rects:
    for ( int row = 0; row < rowCount() - 1; ++row ) {
        for ( int column = 0; column < columnCount(); ++column ) {
            // get the rectangle of the field that will be drawn
            QRect fieldRect;
            const int y = row * FieldHeight;
            if ( column == columnCount() - 1 ) { // totals column
                fieldRect = QRect( width() - m_cachedTotalsFieldRect.width(), y,
                                   m_cachedTotalsFieldRect.width(), FieldHeight );
            } else if ( column == 0 ) { // task column
                fieldRect = QRect( 0, y, taskColumnWidth(), FieldHeight );
            } else if ( column > 0 ) { //  a task
                fieldRect = QRect( width() - m_cachedTotalsFieldRect.width()
                                   - 8 * m_cachedDayFieldRect.width()
                                   + column * m_cachedDayFieldRect.width(), y,
                                   m_cachedDayFieldRect.width(), FieldHeight );
            }
            // paint the field, if it is in the dirty region
            if( e->rect().contains( fieldRect ) ) {
                DataField field = m_defaultField;
                data( field, column, row );
                int alignment = Qt::AlignRight | Qt::AlignVCenter;
                if ( row == 0 ) {
                    alignment = Qt::AlignCenter | Qt::AlignVCenter;
                } else if ( column == 0 && row < rowCount() - 1 ) {
                    alignment = Qt::AlignLeft | Qt::AlignVCenter;
                }
                if( column == 0 ) { // task column
                    field.text = elidedText( field.text, field.font, fieldRect.width() - 2*Margin );
                }
                if ( field.storeAsActive ) m_activeFieldRects << fieldRect;
                const QRect textRect = fieldRect.adjusted( Margin, Margin, -Margin, -Margin );
                if ( field.hasHighlight ) {
                    painter.setBrush( field.highlight );
                    painter.setPen( Qt::NoPen );
                    painter.drawRect( fieldRect );
                } else {
                    painter.setBrush( field.background );
                    painter.setPen( Qt::NoPen );
                    painter.drawRect( fieldRect );
                }
                painter.setPen( palette().text().color() );
                painter.setFont( field.font );
                painter.drawText( textRect, alignment, field.text );
            }
        }
    }
    // paint the tracking row
    const int left = m_taskSelector->geometry().right() + Margin;
    const int top = ( rowCount() - 1 ) * FieldHeight;
    const QRect fieldRect( 0, top, width(), height() - top );
    if ( e->rect().contains( fieldRect ) ) {
        const QRect textRect = fieldRect.adjusted( left + Margin, Margin, -Margin, -Margin );
        DataField field = m_defaultField;
        data( field, 0, rowCount() - 1 );
        painter.setBrush( field.background );
        painter.setPen( Qt::NoPen );
        painter.drawRect( fieldRect );
    }
}


void TimeTrackingSummaryWidget::resizeEvent( QResizeEvent* )
{
    sizeHint(); // make sure cached values are updated
    m_taskSelector->resize( width() - 2*Margin, m_taskSelector->sizeHint().height() );
    m_taskSelector->move( Margin, height() - Margin - m_taskSelector->height() );
    m_elidedTexts.clear();
}

void TimeTrackingSummaryWidget::mousePressEvent( QMouseEvent* event )
{
    if ( ! isTracking() ) {
        const int position = getSummaryAt( event->pos() );
        if ( position >= 0 && position < m_summaries.size() ) {
            m_taskSelector->taskSelected( m_summaries.at( position ) );
        }
    }
}

void TimeTrackingSummaryWidget::mouseDoubleClickEvent( QMouseEvent* event )
{   // we rely on the mouse press event that was received before the doubleclick!
    if( ! isTracking() ) {
        // start tracking
        const int position = getSummaryAt( event->pos() );
        if ( position >= 0 ) {
            const TaskId id = m_summaries.at( position ).task;
            emit startEvent( id );
        }
    }
}

int TimeTrackingSummaryWidget::getSummaryAt( const QPoint& position )
{
    const int left = 0;
    const int right = taskColumnWidth();
    const int fieldIndex = position.y() / m_cachedTotalsFieldRect.height();
    const int taskIndex = fieldIndex - 1;
    if ( taskIndex < 0 || taskIndex >= m_summaries.count() ) {
        return -1;
    }
    if ( position.x() < left || position.x() > right ) {
        return -1;
    }
    return taskIndex;
}

int TimeTrackingSummaryWidget::taskColumnWidth() const
{
    return width() - m_cachedTotalsFieldRect.width() - 7 * m_cachedDayFieldRect.width();
}

void TimeTrackingSummaryWidget::data( DataField& field, int column, int row )
{
    const int HeaderRow = 0;
    const int TotalsRow = rowCount() - 2;
    const int TrackingRow = rowCount() - 1;
    const int TaskColumn = 0;
    const int TotalsColumn = columnCount() - 1;
    const int Day = column - 1;

    field.font = m_fixedFont;
    if ( row == HeaderRow ) {
        field.font = m_narrowFont;
        if ( column == TaskColumn ) {
            field.text = tr( "Task" );
        } else if ( column == TotalsColumn ) {
            field.text = tr( "Total" );
        } else {
            field.text = m_shortDayNames[ column - 1 ];
        }
        field.background = (Day % 2)
                           ? m_paintAttributes.headerBrush
                               : m_paintAttributes.headerEvenDayBrush;
    } else if ( row == TotalsRow ) {
        field.background = m_paintAttributes.totalsRowBrush;
        if ( column == TaskColumn ) {
            // field.text = tr( "Total" );
        } else if ( column == TotalsColumn ) {
            int total = 0;
            Q_FOREACH( const WeeklySummary& s, m_summaries ) {
                total += std::accumulate( s.durations.begin(), s.durations.end(), 0 );
            }
            field.text = hoursAndMinutes( total );
        } else {
            int total = 0;
            Q_FOREACH( const WeeklySummary& s, m_summaries ) {
                total += s.durations[Day];
            }
            field.text = hoursAndMinutes( total );
            field.background = (Day % 2)
                               ? m_paintAttributes.totalsRowBrush
                                   : m_paintAttributes.totalsRowEvenDayBrush;
        }
    } else if ( row == TrackingRow ) {
        // we only return one value, the paint method will treat this
        // column as a special case
        field.background = m_paintAttributes.taskBrushOdd;
        // field.text = tr( " 00:45 2345 KDAB/HR/Project Time Bookkeeping" );
    } else { // a task row
        field.background = row % 2 ? m_paintAttributes.taskBrushEven
            : m_paintAttributes.taskBrushOdd;
        if ( m_summaries.size() > row - 1 ) {
            const int index = row - 1; // index into summaries
            const bool active = DATAMODEL->isTaskActive( m_summaries[index].task );
            if ( active ) {
                field.hasHighlight = true;
                field.highlight = m_paintAttributes.halfHighlight;
            }
            int day = column - 1;
            if ( column == TaskColumn ) {
                field.text = m_summaries[index].taskname;
                field.font = m_narrowFont;
            } else if ( column == TotalsColumn ) {
                const QVector<int>& durations = m_summaries[index].durations;
                const int total = std::accumulate( durations.begin(), durations.end(), 0 );
                field.text = hoursAndMinutes( total );
            } else {
                int duration = m_summaries[index].durations[day];
                field.text = duration > 0 ? hoursAndMinutes( duration) : QString();
                // highlight today as well, with the half highlight:
                if ( day == m_dayOfWeek -1 ) {
                    field.hasHighlight = true;
                    field.storeAsActive = active;
                    QColor pulseColor = m_paintAttributes.pulseColor;
                    pulseColor.setAlphaF( m_paintAttributes.dim
                                          + ( 1.0 - m_paintAttributes.dim )
                                          * m_pulse.currentValue() );
                    const QBrush pulseBrush( pulseColor );
                    field.highlight = active ? pulseBrush : m_paintAttributes.halfHighlight;
                }
            }
        }
    }
}

void TimeTrackingSummaryWidget::setSummaries( QVector<WeeklySummary> s )
{
    m_activeFieldRects.clear();
    m_summaries = s;
    m_cachedMinimumSizeHint = QSize();
    m_cachedSizeHint = QSize();
    m_dayOfWeek = QDate::currentDate().dayOfWeek();
    m_elidedTexts.clear();
    updateGeometry();
    update();
    // populate menu:
    m_taskSelector->populate( m_summaries );
    // FIXME maybe remember last selected task
    handleActiveEvents();
}

bool TimeTrackingSummaryWidget::isTracking() const
{
    return DATAMODEL->activeEventCount() > 0;
}

void TimeTrackingSummaryWidget::handleActiveEvents()
{
    m_activeFieldRects.clear();
    const int activeEventCount = DATAMODEL->activeEventCount();
    Q_ASSERT( activeEventCount >= 0 );

    m_taskSelector->handleActiveEvents();
    if ( activeEventCount > 1 ) {
        if ( m_pulse.state() != QTimeLine::Running ) m_pulse.start();
    } else if ( activeEventCount == 1 ) {
        if ( m_pulse.state() != QTimeLine::Running ) m_pulse.start();
    } else {
        m_pulse.stop();
    }

}

void TimeTrackingSummaryWidget::slotPulseValueChanged( qreal value )
{
    if ( m_activeFieldRects.isEmpty() ) {
        update();
    } else {
        Q_FOREACH( const QRect& rect, m_activeFieldRects ) {
            update( rect );
        }
    }
}

QString TimeTrackingSummaryWidget::elidedText( const QString& text, const QFont& font, int width )
{
    if( ! m_elidedTexts.contains( text ) ) {
        QFontMetrics metrics( font );
        const QString elided = metrics.elidedText( text, Qt::ElideLeft, width );
        m_elidedTexts.insert( text, elided );
    }
    Q_ASSERT( m_elidedTexts.contains( text ) );
    return m_elidedTexts.value( text );
}

void TimeTrackingSummaryWidget::slotUpdateSummaries()
{
    setSummaries( m_summaries );
}

#include "TimeTrackingSummaryWidget.moc"
