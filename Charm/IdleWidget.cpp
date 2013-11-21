#include <QPainter>
#include "Data.h"
#include "IdleWidget.h"

IdleWidget::IdleWidget( QWidget* parent )
    : QWidget( parent )
{}

void IdleWidget::paintEvent( QPaintEvent* )
{
    QPainter painter( this );
    painter.setBrush( Qt::gray );
    painter.drawRect( rect() );
    painter.translate( width()/2, height()/2 );
    painter.drawPixmap( -m_backdrop.width()/2, -m_backdrop.height()/2, m_backdrop );
}

void IdleWidget::resizeEvent( QResizeEvent* )
{
    const double Ratio = 0.8;
    QSize wantedSize = Ratio * size();
    if ( wantedSize.width() < Data::backdropPixmap().width() || wantedSize.height() < Data::backdropPixmap().height() ) {
        m_backdrop = Data::backdropPixmap().scaled( wantedSize, Qt::KeepAspectRatio );
    } else {
        m_backdrop = Data::backdropPixmap();
    }
}

#include "moc_IdleWidget.cpp"


