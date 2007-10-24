#ifndef OPENINGSCENE_H
#define OPENINGSCENE_H

#include <QWidget>

class QGraphicsView;
class QGraphicsScene;
class QGraphicsItem;
class QTimeLine;

class OpeningScene : public QWidget
{
    Q_OBJECT

public:
    explicit OpeningScene( QWidget* parent = 0 );

    void resizeEvent( QResizeEvent* );

private slots:
    void slotAnimate( int );

private:
    QGraphicsScene* m_scene;
    QGraphicsView* m_view;
    QGraphicsItem* m_splashScreen;
    QGraphicsItem* m_appView;
    QTimeLine* m_timeline;
};

#endif
