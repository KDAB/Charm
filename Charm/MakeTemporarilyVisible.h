#ifndef MAKETEMPORARILYVISIBLE_H
#define MAKETEMPORARILYVISIBLE_H

class MakeTemporarilyVisible {
public:
    explicit MakeTemporarilyVisible( QWidget* widget )
        : m_widget( widget )
        , m_wasVisible( false )
    {
        Q_ASSERT( m_widget );
        m_wasVisible = m_widget->isVisible();
        if ( ! m_wasVisible ) {
            m_widget->show();
        }
    }

    ~MakeTemporarilyVisible() {
        if ( ! m_wasVisible ) {
            m_widget->hide();
        }
    }
private:
    QWidget* m_widget;
    bool m_wasVisible;
};

#endif
