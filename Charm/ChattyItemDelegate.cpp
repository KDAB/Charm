#include <QtDebug>
#include <QDebug>
#include <QPainter>
#include <QEvent>

#include "ChattyItemDelegate.h"
#include <QMouseEvent>
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

    const QVariant checkStateVariant = index.data(Qt::CheckStateRole);
    const Qt::CheckState checkState = static_cast<Qt::CheckState>(checkStateVariant.toInt());
    // Find size of checkbox
    QRect cbRect = checkBoxRect(option, checkStateVariant);
    const QRect textRect(option.rect.left(),
                         option.rect.top(),
                         option.rect.width() - cbRect.width(),
                         firstLineHeight(option));

    // Prepare QStyleOptionViewItem with the wanted alignments
    QStyleOptionViewItem modifiedOption = option;
    modifiedOption.displayAlignment = Qt::AlignLeft | Qt::AlignVCenter;
    modifiedOption.decorationAlignment = Qt::AlignLeft | Qt::AlignVCenter;

    // Draw first line of text (task id+name)
    const QString taskName = index.data(Qt::DisplayRole).toString()
                             + " " + index.data( TasksViewRole_Name ).toString();
    drawDisplay(painter, modifiedOption, textRect, taskName);

    // Draw checkbox
    painter->save(); // preserve text colors, for displaying the running time similarly
    drawCheck(painter, option, cbRect, checkState);
    painter->restore();

    const QVariant decorationVariant = index.data(Qt::DecorationRole);
    if (!decorationVariant.isNull()) {
        // This task is active. Draw decoration, running time, comment field.
        const QPixmap decorationPixmap = decoration(option, decorationVariant);
        const QRect pixmapRect(option.rect.left(),
                               option.rect.top() + textRect.height(),
                               decorationPixmap.width(),
                               option.rect.height() - textRect.height() - 1);
        drawDecoration(painter, modifiedOption, pixmapRect, decorationPixmap);

        const QString runningTime = index.data(TasksViewRole_RunningTime).toString();
        QRect textRect(pixmapRect.right() + 5, pixmapRect.top(),
                       option.rect.width(), option.fontMetrics.ascent());
        painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, runningTime, &textRect);

        const QString comment = index.data(TasksViewRole_Comment).toString();
        textRect.moveLeft(textRect.right() + 5);
        textRect.setRight(cbRect.left());
        painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, comment);
    }

    painter->restore();
}

QSize ChattyItemDelegate::sizeHint(const QStyleOptionViewItem &option,
                                   const QModelIndex &index) const
{
    const int height = firstLineHeight(option) + secondLineHeight(option, index);
    return QSize(option.rect.width(), height);
}

int ChattyItemDelegate::firstLineHeight(const QStyleOptionViewItem& option) const
{
    const QRect cbRect = check(option, option.rect, false);
    return qMax(cbRect.height(), option.fontMetrics.height());
}

int ChattyItemDelegate::secondLineHeight(const QStyleOptionViewItem& option,
                                         const QModelIndex& index) const
{
    int height = 0;
    const QVariant decorationVariant = index.data(Qt::DecorationRole);
    if (!decorationVariant.isNull()) {
        const QPixmap decorationPixmap = decoration(option, decorationVariant);
        height = qMax(option.fontMetrics.ascent(), decorationPixmap.height());
    }
    return height;
}

bool ChattyItemDelegate::editorEvent(QEvent *event, QAbstractItemModel *model,
                                     const QStyleOptionViewItem &option, const QModelIndex &index)
{
    Q_ASSERT(event);
    Q_ASSERT(model);

    // make sure that the item is checkable
    Qt::ItemFlags flags = model->flags(index);
    if (!(flags & Qt::ItemIsUserCheckable) || !(option.state & QStyle::State_Enabled)
        || !(flags & Qt::ItemIsEnabled))
        return false;

    // make sure that we have a check state
    QVariant value = index.data(Qt::CheckStateRole);
    if (!value.isValid())
        return false;

    // make sure that we have the right event type
    if ((event->type() == QEvent::MouseButtonRelease)
        || (event->type() == QEvent::MouseButtonDblClick)) {
        const QRect checkRect = checkBoxRect(option, Qt::Checked);
        QMouseEvent *me = static_cast<QMouseEvent*>(event);
        if (me->button() != Qt::LeftButton || !checkRect.contains(me->pos()))
            return false;

        // eat the double click events inside the check rect
        if (event->type() == QEvent::MouseButtonDblClick)
            return true;

    } else if (event->type() == QEvent::KeyPress) {
        if (static_cast<QKeyEvent*>(event)->key() != Qt::Key_Space
         && static_cast<QKeyEvent*>(event)->key() != Qt::Key_Select)
            return false;
    } else {
        return false;
    }

    Qt::CheckState state = (static_cast<Qt::CheckState>(value.toInt()) == Qt::Checked
                            ? Qt::Unchecked : Qt::Checked);
    return model->setData(index, state, Qt::CheckStateRole);
}

QRect ChattyItemDelegate::checkBoxRect(const QStyleOptionViewItem &option,
                                       const QVariant &variant) const
{
    const QRect bounding = option.rect; // TODO adjust if recording

    QRect cbRect = check(option, bounding, variant);

    // Position checkbox on the right, and vertically aligned
    return QStyle::alignedRect(option.direction, Qt::AlignRight | Qt::AlignVCenter,
                                 cbRect.size(), bounding);
}

#include "ChattyItemDelegate.moc"

