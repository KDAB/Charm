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

const int Margin = 3;

TimeTrackingSummaryWidget::TimeTrackingSummaryWidget( QWidget* parent )
    : QWidget( parent )
    , m_stopGoButton( this )
    , m_taskSelector( this )
    , m_selectedSummary( -1 )
{
    // FIXME use platform defined, hand-picked fonts, so far those have been selected for Mac:
#ifdef Q_WS_MAC
    m_fixedFont.setFamily( "Andale Mono" );
    m_fixedFont.setPointSize( 11 );
    m_narrowFont = font(); // stay with the desktop
    m_narrowFont.setPointSize( 11 );
#elif defined Q_WS_X11
    m_fixedFont.setFamily(  "Bitstream Vera Sans Mono" );
    m_fixedFont.setPointSize( 9 );
    m_narrowFont = font(); // stay with the desktop
    m_narrowFont.setPointSize( 9 );
#endif
    //
    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
    // plumbing
    m_stopGoButton.setCheckable( true );
    connect( &m_stopGoButton, SIGNAL( clicked( bool ) ),
             SLOT( slotGoStopToggled( bool ) ) );
    m_taskSelector.setEnabled( false );
    m_taskSelector.setPopupMode( QToolButton::InstantPopup );
    m_taskSelector.setMenu( &m_menu );
    m_taskSelector.setText( tr( "Select Task" ) );
    m_pulse.setLoopCount( 0 );
    m_pulse.setDuration( 2000 );
    m_pulse.setUpdateInterval( 160 ); // any smaller value uses too much cpu
    m_pulse.setCurveShape( QTimeLine::SineCurve );
    connect( &m_pulse, SIGNAL( valueChanged( qreal ) ),
             SLOT( slotPulseValueChanged( qreal ) ) );
    connect( &m_menu, SIGNAL( triggered( QAction* ) ),
             SLOT( slotActionSelected( QAction* ) ) );
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
    m_activeFieldRects.clear();
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
                fieldRect = QRect( 0, y, taskColumnWidth(), FieldHeight );
                QFontMetrics metrics( field.font );
                field.text = metrics.elidedText( field.text, Qt::ElideLeft,
                                                 fieldRect.width() - 2*Margin );
            } else if ( column > 0 ) { //  a task
                fieldRect = QRect( width() - m_cachedTotalsFieldRect.width()
                                   - 8 * m_cachedDayFieldRect.width()
                                   + column * m_cachedDayFieldRect.width(), y,
                                   m_cachedDayFieldRect.width(), FieldHeight );
            }
            if ( field.storeAsActive ) m_activeFieldRects << fieldRect;
            if ( e->rect().contains( fieldRect ) ) {
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
    const int left = m_stopGoButton.geometry().right() + Margin;
    const int top = ( rowCount() - 1 ) * FieldHeight;
    const QRect fieldRect( 0, top, width(), height() - top );
    if ( e->rect().contains( fieldRect ) ) {
        const QRect textRect = fieldRect.adjusted( left + Margin, Margin, -Margin, -Margin );
        const DataField field = data( 0, rowCount() - 1 );
        painter.setBrush( field.background );
        painter.setPen( Qt::NoPen );
        painter.drawRect( fieldRect );
    }
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

void TimeTrackingSummaryWidget::mousePressEvent( QMouseEvent* event )
{
    if ( ! isTracking() ) {
        const int position = getSummaryAt( event->pos() );
        if ( position >= 0 ) {
            selectSummary( position );
        }
    }
}

void TimeTrackingSummaryWidget::mouseDoubleClickEvent( QMouseEvent* event )
{   // we rely on the mouse press event that was received before the doubleclick!
    if( ! isTracking() ) {
        // start tracking
        const int position = getSummaryAt( event->pos() );
        if ( position >= 0 ) {
            slotGoStopToggled( true );
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
        field.font = m_narrowFont;
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

        // field.text = tr( " 00:45 2345 KDAB/HR/Project Time Bookkeeping" );
    } else { // a task row
        field.background = row % 2 ? TaskBrushEven : TaskBrushOdd;
        if ( m_summaries.size() > row - 1 ) {
            const int index = row - 1; // index into summaries
            const bool active = DATAMODEL->isTaskActive( m_summaries[index].task );
            QColor dimHighlight( palette().highlight().color() );
            const float dim = 0.25;
            dimHighlight.setAlphaF( dim * dimHighlight.alphaF() );
            const QBrush halfHighlight( dimHighlight );

            if ( active ) {
                field.hasHighlight = true;
                field.highlight = halfHighlight;
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
                if ( day == QDate::currentDate().dayOfWeek() -1 ) {
                    field.hasHighlight = true;
                    field.storeAsActive = active;
                    QColor pulseColor = palette().highlight().color();
                    pulseColor.setAlphaF( dim + ( 1.0 - dim ) * m_pulse.currentValue() );
                    const QBrush pulseBrush( pulseColor );
                    field.highlight = active ? pulseBrush : halfHighlight;
                }
            }
        }
    }

    return field;
}

void TimeTrackingSummaryWidget::setSummaries( QVector<WeeklySummary> s )
{
    m_activeFieldRects.clear();
    m_summaries = s;
    m_cachedMinimumSizeHint = QSize();
    m_cachedSizeHint = QSize();
    updateGeometry();
    update();
    // populate menu:
    qDeleteAll( m_currentActions );
    m_currentActions.clear();
    Q_FOREACH( const WeeklySummary& s, m_summaries ) {
        m_currentActions << m_menu.addAction( s.taskname );
    }
    // FIXME maybe remember last selected task
    emit maybeShrink();
    handleActiveEvents();
}

void TimeTrackingSummaryWidget::slotGoStopToggled( bool on )
{
    Q_ASSERT( ( m_selectedSummary >= 0 && m_selectedSummary < m_summaries.size() )
              || m_selectedSummary == -1 );

    if ( on ) {
        if ( m_selectedSummary != -1 ) {
            emit startEvent( m_summaries[m_selectedSummary].task );
        }
    } else {
        emit stopEvent();
    }
}

bool TimeTrackingSummaryWidget::isTracking() const
{
    return DATAMODEL->activeEventCount() > 0;
}

void TimeTrackingSummaryWidget::slotActionSelected( QAction* action )
{
    QList<QAction*>::iterator it = std::find( m_currentActions.begin(), m_currentActions.end(), action );
    const int position = std::distance( m_currentActions.begin(), it );
    selectSummary( position );
}

void TimeTrackingSummaryWidget::selectSummary( int position )
{
    Q_ASSERT( position >= 0 && position < m_summaries.size() );
    m_selectedSummary = position;
    m_taskSelector.setText( m_summaries[m_selectedSummary].taskname );
    m_stopGoButton.setEnabled( true );
}

void TimeTrackingSummaryWidget::handleActiveEvents()
{
    m_activeFieldRects.clear();
    const int activeEventCount = DATAMODEL->activeEventCount();
    Q_ASSERT( activeEventCount >= 0 );

    bool taskSelected = m_selectedSummary >= 0 && m_selectedSummary < m_summaries.size();

    if ( activeEventCount > 1 ) {
        m_stopGoButton.setIcon( Data::recorderGoIcon() );
        m_stopGoButton.setText( tr( "Start" ) );
        m_taskSelector.setEnabled( false );
        m_stopGoButton.setEnabled( false );
        m_stopGoButton.setChecked( true );
        qDebug() << "TimeTrackingView::eventActivated: disable GUI, multiple events are active!";
        if ( m_pulse.state() != QTimeLine::Running ) m_pulse.start();
    } else if ( activeEventCount == 1 ) {
        m_stopGoButton.setIcon( Data::recorderStopIcon() );
        m_stopGoButton.setText( tr( "Stop" ) );
        m_stopGoButton.setEnabled( true );
        m_taskSelector.setEnabled( false );
        m_stopGoButton.setChecked( true );
        if ( m_pulse.state() != QTimeLine::Running ) m_pulse.start();
    } else {
        m_stopGoButton.setIcon( Data::recorderGoIcon() );
        m_stopGoButton.setText( tr( "Start" ) );
        m_taskSelector.setEnabled( true );
        m_stopGoButton.setEnabled( taskSelected );
        m_stopGoButton.setChecked( false );
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

#include "TimeTrackingSummaryWidget.moc"
