#include <QtDebug>
#include <QTransform>
#include <QGLWidget>
#include <QTimeLine>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>

#include "Data.h"
#include "OpeningScene.h"

const int TimeLineDuration = 3000;
const int FrameRate = 12;
const int Steps = static_cast<int>( FrameRate * ( 0.001 * TimeLineDuration ) );

OpeningScene::OpeningScene( QWidget* parent )
    : QWidget( parent )
    , m_scene( new QGraphicsScene( this ) )
    , m_view( new QGraphicsView( m_scene, this ) )
    , m_timeline( new QTimeLine( TimeLineDuration, this ) )
{
    // set up view and scene:
    m_view->move( 0, 0 );
    if ( QGLFormat::hasOpenGL() ) {
        qDebug() << "OpeningScene ctor: using OpenGL";
        m_view->setViewport( new QGLWidget( QGLFormat( QGL::SampleBuffers ) ) );
    } else {
        qDebug() << "OpeningScene ctor: no OpenGL available, too bad";
    }
    m_scene->setSceneRect( -1.0, -1.0, 1.0, 1.0 );
    // back drop (splash screen) pixmap:
    {
        QGraphicsPixmapItem* item = new QGraphicsPixmapItem( Data::backdropPixmap() );
        item->setOffset( -.5*item->boundingRect().width(), -.5*item->boundingRect().height() );
        m_splashScreen = item;
        m_scene->addItem( m_splashScreen );
    }
    // the screen we blend to:
    {
        QGraphicsRectItem* item = new QGraphicsRectItem();
        item->setBrush( Qt::black );
        m_appView = item;
        m_scene->addItem( m_appView );
    }
    // set up time line:
    m_timeline->setFrameRange( 0, Steps );
    connect( m_timeline, SIGNAL( frameChanged( int ) ),
             SLOT( slotAnimate( int ) ) );

    // temp: start on demand:
    m_timeline->setLoopCount( 0 ); // loop forever
    m_timeline->start();
}

void OpeningScene::resizeEvent( QResizeEvent* )
{
    // m_view->setSceneRect( rect() );
    m_view->resize( size() );
}

void OpeningScene::slotAnimate( int step )
{
    m_splashScreen->rotate( 1 );
    m_splashScreen->scale( 0.98, 0.98 );
}

#include "OpeningScene.moc"
