#ifndef IDLEWIDGET_H
#define IDLEWIDGET_H

#include <QWidget>
#include <QPixmap>

class IdleWidget : public QWidget
{
    Q_OBJECT

public:
    explicit IdleWidget( QWidget* parent = nullptr );

    void paintEvent( QPaintEvent* );
    void resizeEvent( QResizeEvent* );

private:
    QPixmap m_backdrop;
};

#endif
