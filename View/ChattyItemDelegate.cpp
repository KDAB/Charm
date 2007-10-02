#include <QtDebug>
#include "ChattyItemDelegate.h"


ChattyItemDelegate::ChattyItemDelegate( QObject* parent )
    : QItemDelegate( parent )
    , m_editing( false )
{
    connect( this, SIGNAL( closeEditor( QWidget*, QAbstractItemDelegate::EndEditHint ) ),
             SLOT( slotCloseEditor( QWidget*, QAbstractItemDelegate::EndEditHint ) ) );

}

QWidget* ChattyItemDelegate::createEditor( QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index ) const
{
    QWidget* result = QItemDelegate::createEditor( parent, option, index );
    m_editing = true;
    emit editingStateChanged();
    return result;
}

bool ChattyItemDelegate::isEditing() const
{
    return m_editing;
}

void ChattyItemDelegate::slotCloseEditor( QWidget*, QAbstractItemDelegate::EndEditHint )
{
    m_editing = false;
    emit editingStateChanged();
}

#include "ChattyItemDelegate.moc"

