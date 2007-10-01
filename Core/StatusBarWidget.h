#ifndef STATUSBARWIDGET_H
#define STATUSBARWIDGET_H

#include <QWidget>

namespace Ui {
    class StatusBarWidget;
}

class StatusBarWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StatusBarWidget( QWidget* parent = 0 );
    ~StatusBarWidget();

private:
    Ui::StatusBarWidget* m_ui;
};

#endif
