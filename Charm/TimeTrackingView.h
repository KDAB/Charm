#ifndef TIMETRACKINGVIEW_H
#define TIMETRACKINGVIEW_H

#include <QWidget>

namespace Ui {
    class TimeTrackingView;
}

class TimeTrackingView : public QWidget {
public:
    explicit TimeTrackingView( QWidget* parent = 0 );
    ~TimeTrackingView();

private:
    Ui::TimeTrackingView* m_ui;
};

#endif
