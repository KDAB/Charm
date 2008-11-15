#include <QtDebug>
#include <QDebug>
#include <QPainter>

#include "ChattyItemDelegate.h"
#include "TaskModelAdapter.h"

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

void ChattyItemDelegate::paint(QPainter *painter,
                               const QStyleOptionViewItem &option,
                               const QModelIndex &index) const
{
    painter->save();
    drawBackground( painter, option, index );
    const QString taskName = index.data(Qt::DisplayRole).toString()
                             + " " + index.data( TasksViewRole_Name ).toString();
#if 0
    const QVariant checkStateVariant = index.data(Qt::CheckStateRole);
    const Qt::CheckState checkState = static_cast<Qt::CheckState>(checkStateVariant.toInt());
#else
    const Qt::CheckState checkState = Qt::Checked;
    const QVariant checkStateVariant = QVariant(checkState);
#endif
    // Find size of checkbox
    const QRect bounding = option.rect; // TODO adjust if recording
    QRect cbRect = check(option, bounding, checkStateVariant);
    // Position checkbox on the right, and vertically aligned
    cbRect = QStyle::alignedRect(option.direction, Qt::AlignRight | Qt::AlignVCenter,
                                 cbRect.size(), bounding);
    const QRect textRect(option.rect.left(),
                         option.rect.top(),
                         option.rect.width() - cbRect.width(),
                         firstLineHeight(option));
    QStyleOptionViewItem optText = option;
    optText.displayAlignment = Qt::AlignLeft | Qt::AlignVCenter;
    drawDisplay(painter, optText, textRect, taskName);

    drawCheck(painter, option, cbRect, checkState);
    painter->restore();
}

QSize ChattyItemDelegate::sizeHint(const QStyleOptionViewItem &option,
                                   const QModelIndex &index) const
{
    return QSize(option.rect.width(), firstLineHeight(option));
}

int ChattyItemDelegate::firstLineHeight(const QStyleOptionViewItem& option) const
{
    const QRect cbRect = check(option, option.rect, false);
    return qMax(cbRect.height(), option.fontMetrics.height());
}

#include "ChattyItemDelegate.moc"

