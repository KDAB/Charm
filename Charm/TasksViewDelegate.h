#ifndef CHATTYITEMDELEGATE_H
#define CHATTYITEMDELEGATE_H

#include <QItemDelegate>

/**
 * Delegate for the tasks view and for the "select task" dialog.
 */
class TasksViewDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    explicit TasksViewDelegate( QObject* parent = 0 );

    /*! \reimp */ void paint( QPainter *painter,
                              const QStyleOptionViewItem &option,
                              const QModelIndex &index ) const;
    /*! \reimp */ QSize sizeHint( const QStyleOptionViewItem &option,
                                  const QModelIndex &index ) const;

    /*! \reimp */ QWidget* createEditor( QWidget* parent,
                                         const QStyleOptionViewItem& option,
                                         const QModelIndex& index ) const;
    /*! \reimp */ bool editorEvent( QEvent *event, QAbstractItemModel *model,
                                    const QStyleOptionViewItem &option,
                                    const QModelIndex &index );
    /*! \reimp */ void updateEditorGeometry( QWidget * editor,
                                             const QStyleOptionViewItem & option,
                                             const QModelIndex & index ) const;
    /*! \reimp */ void setEditorData( QWidget * editor, const QModelIndex & index ) const;

    bool isEditing() const;

signals:
    void editingStateChanged() const;

private slots:
    void slotCloseEditor( QWidget* editor, QAbstractItemDelegate::EndEditHint );

private:
    int secondLineTextHeight( const QStyleOptionViewItem& option,
                              const QModelIndex& index ) const;
    QRect checkBoxRect( const QStyleOptionViewItem &option,
                        const QVariant &variant ) const;
    struct Layout {
        int firstLineHeight;
        int secondLineHeight;
        int secondLineTextHeight;
        QRect cbRect;
    };
    Layout doLayout( const QStyleOptionViewItem& option,
                     const QModelIndex& index ) const;
    mutable bool m_editing;
};
#endif
