#include <QtDebug>
#include <QDebug>
#include <QPainter>
#include <QEvent>

#include "TasksViewDelegate.h"
#include <QApplication>
#include <QMouseEvent>
#include "TaskModelAdapter.h"

TasksViewDelegate::TasksViewDelegate( QObject* parent )
    : QItemDelegate( parent )
    , m_editing( false )
{
    connect( this, SIGNAL( closeEditor( QWidget*, QAbstractItemDelegate::EndEditHint ) ),
             SLOT( slotCloseEditor( QWidget*, QAbstractItemDelegate::EndEditHint ) ) );

}

QWidget* TasksViewDelegate::createEditor( QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index ) const
{
    QWidget* result = QItemDelegate::createEditor( parent, option, index );
    m_editing = true;
    emit editingStateChanged();
    return result;
}

bool TasksViewDelegate::isEditing() const
{
    return m_editing;
}

void TasksViewDelegate::slotCloseEditor( QWidget*, QAbstractItemDelegate::EndEditHint )
{
    m_editing = false;
    emit editingStateChanged();
}

void TasksViewDelegate::paint( QPainter *painter,
                               const QStyleOptionViewItem &option,
                               const QModelIndex &index ) const
{
    painter->save();

    // Nasty QTreeView clips the painting to the editor geometry!
    // We don't want that....
    painter->setClipRect( option.rect );

    drawBackground( painter, option, index );

    const QVariant checkStateVariant = index.data(Qt::CheckStateRole);
    const Qt::CheckState checkState = static_cast<Qt::CheckState>(checkStateVariant.toInt());

    Layout layout = doLayout( option, index );

    const QRect textRect(option.rect.left(),
                         option.rect.top(),
                         option.rect.width() - layout.cbRect.width(),
                         layout.firstLineHeight);

    // Prepare QStyleOptionViewItem with the wanted alignments
    QStyleOptionViewItem modifiedOption = option;
    modifiedOption.displayAlignment = Qt::AlignLeft | Qt::AlignVCenter;
    modifiedOption.decorationAlignment = Qt::AlignLeft | Qt::AlignVCenter;

    // Draw first line of text (task id+name)
    const QString taskName = index.data(Qt::DisplayRole).toString();
    drawDisplay(painter, modifiedOption, textRect, taskName);

    // Draw checkbox
    painter->save(); // preserve text colors, for displaying the running time similarly
    drawCheck(painter, option, layout.cbRect, checkState);
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
                       option.rect.width(), layout.secondLineTextHeight);

        QColor dimHighlight( option.palette.highlight().color() );
        const float dim = 0.25;
        dimHighlight.setAlphaF( dim * dimHighlight.alphaF() );
        const QBrush halfHighlight( dimHighlight );
        painter->setBackground( halfHighlight ); // running time on blue background
        painter->setBackgroundMode( Qt::OpaqueMode );
        painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, runningTime, &textRect);
        painter->setBackgroundMode( Qt::TransparentMode );

        const QString comment = index.data(TasksViewRole_Comment).toString();
        textRect.moveLeft(textRect.right() + 5);
        textRect.setRight(layout.cbRect.left());
        painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, comment);
    }

    painter->restore();
}

QSize TasksViewDelegate::sizeHint( const QStyleOptionViewItem &option,
                                   const QModelIndex &index) const
{
    Layout layout = doLayout( option, index );
    return QSize(option.rect.width(), layout.firstLineHeight + layout.secondLineHeight);
}

bool TasksViewDelegate::editorEvent(QEvent *event, QAbstractItemModel *model,
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

QRect TasksViewDelegate::checkBoxRect( const QStyleOptionViewItem &option,
                                       const QVariant &variant ) const
{
    const QRect bounding = option.rect; // TODO adjust if recording

    QRect cbRect = check(option, bounding, variant);

    // Position checkbox on the right, and vertically aligned
    return QStyle::alignedRect(option.direction, Qt::AlignRight | Qt::AlignVCenter,
                               cbRect.size(), bounding);
}

void TasksViewDelegate::updateEditorGeometry( QWidget * editor,
                                              const QStyleOptionViewItem & option,
                                              const QModelIndex & index ) const
{
    // TODO use doLayout
    const QRect cbRect = checkBoxRect(option, Qt::Checked);
    int firstLineHeight = qMax(cbRect.height(), option.fontMetrics.height());
    const QVariant decorationVariant = index.data(Qt::DecorationRole);
    const QPixmap decorationPixmap = decoration(option, decorationVariant);
    const QString runningTime = index.data(TasksViewRole_RunningTime).toString();
    const int left = decorationPixmap.width() + option.fontMetrics.width(runningTime);
    QRect r = option.rect.translated( left + 5, 0 );
    r.setRight( cbRect.left() );
    r.setTop( r.top() + firstLineHeight );
    editor->setGeometry( r );
}

void TasksViewDelegate::setEditorData( QWidget * editor, const QModelIndex & index ) const
{
    // Do not reset the comment lineedit to empty every time TaskModelAdapter emits
    // dataChanged (because of the running time being updated).
    if ( m_editing )
        return;
    QItemDelegate::setEditorData( editor, index );
}

TasksViewDelegate::Layout TasksViewDelegate::doLayout( const QStyleOptionViewItem& option,
                                                       const QModelIndex& index ) const
{
    Layout layout;
    // Find size of checkbox
    const QVariant checkStateVariant = index.data(Qt::CheckStateRole);
    layout.cbRect = checkBoxRect(option, checkStateVariant);
    layout.firstLineHeight = qMax(layout.cbRect.height(), option.fontMetrics.height());
    layout.secondLineHeight = 0;
    layout.secondLineTextHeight = 0;
    const QVariant decorationVariant = index.data(Qt::DecorationRole);
    if (!decorationVariant.isNull()) {
        const QPixmap decorationPixmap = decoration(option, decorationVariant);

        const QString comment = index.data(TasksViewRole_Comment).toString();
#if 0
        // For editing or rendering a comment we need the whole font height,
        // while for just "00:05" we only need the ascent.
        // Update: changed my mind: looks too narrow.
        if ( m_editing || !comment.isEmpty() ) {
#endif
            layout.secondLineTextHeight = option.fontMetrics.lineSpacing() + 2 +
                                          qApp->style()->pixelMetric(QStyle::PM_DefaultFrameWidth, &option, 0);
#if 0
        } else {
            layout.secondLineTextHeight = option.fontMetrics.ascent();
        }
#endif

        layout.secondLineHeight = qMax(layout.secondLineTextHeight, decorationPixmap.height());
    }

    return layout;
}

#include "TasksViewDelegate.moc"

