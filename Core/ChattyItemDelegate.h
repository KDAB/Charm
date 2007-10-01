#ifndef CHATTYITEMDELEGATE_H
#define CHATTYITEMDELEGATE_H

#include <QItemDelegate>

class ChattyItemDelegate : public QItemDelegate
{
    Q_OBJECT

public:
    explicit ChattyItemDelegate( QObject* parent = 0 );

    // reimpl
    QWidget* createEditor( QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index ) const;

    bool isEditing() const;

signals:
    void editingStateChanged() const;

private slots:
    void slotCloseEditor( QWidget* editor, QAbstractItemDelegate::EndEditHint );

private:
    mutable bool m_editing;
};
#endif
