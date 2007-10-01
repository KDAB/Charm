/****************************************************************************
**
** Copyright (C) 1992-$THISYEAR$ Trolltech AS. All rights reserved.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include <QtGui/QtGui>
#include "modeltest.h"
Q_DECLARE_METATYPE(QModelIndex)

/*!
    nonDestructiveBasicTest trys to call a number of the basic functions (not all)
    to make sure the model doesn't segfault, testing the functions that makes sense.
 */
void Modeltest::nonDestructiveBasicTest()
{
    Q_ASSERT(model->buddy(QModelIndex()) == QModelIndex());
    model->canFetchMore(QModelIndex());
    Q_ASSERT(model->columnCount(QModelIndex()) >= 0);
    Q_ASSERT(model->data(QModelIndex()) == QVariant());
    model->fetchMore(QModelIndex());
    Qt::ItemFlags flags = model->flags(QModelIndex());
    Q_ASSERT(flags == Qt::ItemIsDropEnabled || flags == 0);
    model->hasChildren(QModelIndex());
    model->hasIndex(0, 0);
    model->headerData(0, Qt::Horizontal);
    model->index(0,0);
    model->itemData(QModelIndex());
    QVariant cache;
    model->match(QModelIndex(), -1, cache);
    model->mimeTypes();
    Q_ASSERT(model->parent(QModelIndex()) == QModelIndex());
    Q_ASSERT(model->rowCount() >= 0);
    QVariant variant;
    model->setData(QModelIndex(), variant, -1);
    model->setHeaderData(-1, Qt::Horizontal, QVariant());
    model->setHeaderData(0, Qt::Horizontal, QVariant());
    model->setHeaderData(999999, Qt::Horizontal, QVariant());
    QMap<int, QVariant> roles;
    model->sibling(0, 0,QModelIndex());
    model->span(QModelIndex());
    model->supportedDropActions();
}


/*!
    Tests model's implimentation of QAbstractItemModel::rowCount() and hasChildren()
 */
void Modeltest::rowCount()
{
    // check top row
    QModelIndex topIndex = model->index(0, 0, QModelIndex());
    int rows = model->rowCount(topIndex);
    Q_ASSERT(rows >= 0);
    if (rows > 0)
        Q_ASSERT(model->hasChildren(topIndex) == true);
    //else
    //    Q_ASSERT(model->hasChildren(topIndex) == false);

    QModelIndex secondLevelIndex = model->index(0, 0, topIndex);
    if (secondLevelIndex.isValid()) { // not the top level
        // check a row count where parent is valid
        rows = model->rowCount(secondLevelIndex);
        Q_ASSERT(rows >= 0);
        if (rows > 0)
            Q_ASSERT(model->hasChildren(secondLevelIndex) == true);
        //else
        //    Q_ASSERT(model->hasChildren(secondLevelIndex) == false);
    }

    // rowCount is tested more extensivly more later in checkChildren(),
    // but this catches the big mistakes
}

/*!
    Tests model's implimentation of QAbstractItemModel::columnCount() and hasChildren()
 */
void Modeltest::columnCount()
{
    // check top row
    QModelIndex topIndex = model->index(0, 0, QModelIndex());
    int columns = model->columnCount(topIndex);

    // check a row count where parent is valid
    columns = model->columnCount(model->index(0, 0, topIndex));
    Q_ASSERT(columns >= 0);

    // columnCount is tested more extensivly more later in checkChildren(),
    // but this catches the big mistakes
}

/*!
    Tests model's implimentation of QAbstractItemModel::hasIndex()
 */
void Modeltest::hasIndex()
{
    // Make sure that invalid values returns an invalid index
    Q_ASSERT(model->hasIndex(-2, -2) == false);
    Q_ASSERT(model->hasIndex(-2, 0) == false);
    Q_ASSERT(model->hasIndex(0, -2) == false);

    int rows = model->rowCount();
    int columns = model->columnCount();

    Q_ASSERT(model->hasIndex(rows, columns) == false);
    Q_ASSERT(model->hasIndex(rows+1, columns+1) == false);

    if (rows > 0)
        Q_ASSERT(model->hasIndex(0,0) == true);

    // hasIndex is tested more extensivly more later in checkChildren(),
    // but this catches the big mistakes
}

/*!
    Tests model's implimentation of QAbstractItemModel::index()
 */
void Modeltest::index()
{
    // Make sure that invalid values returns an invalid index
    Q_ASSERT(model->index(-2, -2) == QModelIndex());
    Q_ASSERT(model->index(-2, 0) == QModelIndex());
    Q_ASSERT(model->index(0, -2) == QModelIndex());

    int rows = model->rowCount();
    int columns = model->columnCount();

    if (rows == 0)
        return;

    // Catch off by one errors
    Q_ASSERT(model->index(rows, columns) == QModelIndex());
    Q_ASSERT(model->index(0, 0).isValid() == true);

    // Make sure that the same index is always returned
    QModelIndex a = model->index(0,0);
    QModelIndex b = model->index(0,0);
    Q_ASSERT(a == b);

    // index is tested more extensivly more later in checkChildren(),
    // but this catches the big mistakes
}


/*!
    A model that returns an index of parent X should also return X when asking
    for the parent of the index.

    This recursive function does pretty extensive testing on the whole model in an
    effort to catch edge cases.

    This function assumes that rowCount(), columnCount() and index() work.  If they have
    a bug it will point it out, but the above tests should have already found the basic bugs
    because it is easier to figure out the problem in those tests then this one.
 */
void checkChildren(QAbstractItemModel *model, const QModelIndex &parent, int currentDepth=0)
{
    QModelIndex p = parent;
    while (p.isValid())
       p = p.parent();

    if (model->canFetchMore(parent))
        model->fetchMore(parent);

    int rows = model->rowCount(parent);
    int columns = model->columnCount(parent);

    if (rows > 0)
        Q_ASSERT((rows > 0) == (model->hasChildren(parent)));

    // Some reasuring testing against rows(),columns(), and hasChildren()
    Q_ASSERT(rows >= 0);
    Q_ASSERT(columns >= 0);
    if (rows > 0)
        Q_ASSERT(model->hasChildren(parent) == true);
    //else
    //    Q_ASSERT(model->hasChildren(parent) == false);

    //qDebug() << "parent:" << model->data(parent).toString() << "rows:" << rows
    //         << "columns:" << columns << "parent column:" << parent.column();

    Q_ASSERT(model->hasIndex(rows+1, 0, parent) == false);
    for (int r = 0; r < rows; ++r) {
        if (model->canFetchMore(parent))
            model->fetchMore(parent);

        Q_ASSERT(model->hasIndex(r, columns+1, parent) == false);
        for (int c = 0; c < columns; ++c) {
            Q_ASSERT(model->hasIndex(r, c, parent) == true);
            QModelIndex index = model->index(r, c, parent);
            Q_ASSERT(index.isValid() == true);

            QModelIndex modifiedIndex = model->index(r, c, parent);
            Q_ASSERT(index == modifiedIndex);

            // Make sure we get the same index if we request it twice in a row
            QModelIndex a = model->index(r, c, parent);
            QModelIndex b = model->index(r, c, parent);
            Q_ASSERT(a == b);

            // Some basic checking on the index that is returned
            Q_ASSERT(index.model() == model);
            Q_ASSERT(index.row() == r);
            Q_ASSERT(index.column() == c);
            // Q_ASSERT(model->data(index, Qt::DisplayRole).isValid() == true);

            // If the next test fails here is some somewhat usefull debug you play with.
            /*
            if (model->parent(index) != parent) {
                qDebug() << r << c << currentDepth << model->data(index).toString()
                         << model->data(parent).toString();
                qDebug() << index << parent << model->parent(index);
                //QTreeView view;
                //view.setModel(model);
                //view.show();
                //QTest::qWait(9000);
            }*/
            //qDebug() << index << model->parent(index) << parent;
            Q_ASSERT(model->parent(index) == parent);

            // recursivly go down
            if (model->hasChildren(index) && currentDepth < 2 ) {
                //qDebug() << r << c << "has children" << model->rowCount(index);
                checkChildren(model, index, ++currentDepth);
            }/* else { if (currentDepth >= 10) qDebug() << "checked 10 deep"; };*/

            // make sure that after testing the children that the index pointer doesn't change.
            QModelIndex newerIndex = model->index(r, c, parent);
            Q_ASSERT(index == newerIndex);
        }
    }
}

/*!
    Tests model's implimentation of QAbstractItemModel::parent()
 */
void Modeltest::parent()
{
    // Make sure the model wont crash and will return an invalid QModelIndex
    // when asked for the parent of an invalid index.
    Q_ASSERT(model->parent(QModelIndex()) == QModelIndex());

    if (model->rowCount() == 0)
        return;

    // Common error test #1, make sure that a top level index has a parent
    // that is a invalid QModelIndex.  You model
    QModelIndex topIndex = model->index(0, 0, QModelIndex());
    Q_ASSERT(model->parent(topIndex) == QModelIndex());

    // Common error test #2, make sure that a second level index has a parent
    // that is the top level index.
    if (model->rowCount(topIndex) > 0) {
        QModelIndex childIndex = model->index(0, 0, topIndex);
        Q_ASSERT(model->parent(childIndex) == topIndex);
    }

    // Common error test #3, the second colum has the same children
    // as the first column in a row.
    QModelIndex topIndex1 = model->index(0, 1, QModelIndex());
    if (model->rowCount(topIndex1) > 0) {
        QModelIndex childIndex = model->index(0, 0, topIndex);
        QModelIndex childIndex1 = model->index(0, 0, topIndex1);
        Q_ASSERT(childIndex != childIndex1);
    }

    // Full test, walk 10 levels deap through the model making sure that all
    // parents's children correctly specify their parent
    QModelIndex top = QModelIndex();
    checkChildren(model, top);
}


/*!
    Tests model's implimentation of QAbstractItemModel::data()
 */
void Modeltest::data()
{
    // Invalid index should return an invalid qvariant
    Q_ASSERT(!model->data(QModelIndex()).isValid());

    if (model->rowCount() == 0)
        return;

    // A valid index should have a valid qvariant data
    Q_ASSERT(model->index(0,0).isValid());

    // shouldn't be able to set data on an invalid index
    Q_ASSERT(model->setData(QModelIndex(), "foo", Qt::DisplayRole) == false);

    // General Purpose roles
    QVariant variant = model->data(model->index(0,0), Qt::ToolTipRole);
    if (variant.isValid()) {
        Q_ASSERT(qVariantCanConvert<QString>(variant));
    }
    variant = model->data(model->index(0,0), Qt::StatusTipRole);
    if (variant.isValid()) {
        Q_ASSERT(qVariantCanConvert<QString>(variant));
    }
    variant = model->data(model->index(0,0), Qt::WhatsThisRole);
    if (variant.isValid()) {
        Q_ASSERT(qVariantCanConvert<QString>(variant));
    }

    variant = model->data(model->index(0,0), Qt::SizeHintRole);
    if (variant.isValid()) {
        Q_ASSERT(qVariantCanConvert<QSize>(variant));
    }


    // Appearance roles
    QVariant fontVariant = model->data(model->index(0,0), Qt::FontRole);
    if (fontVariant.isValid()) {
        Q_ASSERT(qVariantCanConvert<QFont>(fontVariant));
    }

    QVariant textAlignmentVariant = model->data(model->index(0,0), Qt::TextAlignmentRole);
    if (textAlignmentVariant.isValid()) {
        int alignment = textAlignmentVariant.toInt();
        Q_ASSERT(alignment == Qt::AlignLeft ||
                alignment == Qt::AlignRight ||
                alignment == Qt::AlignHCenter ||
                alignment == Qt::AlignJustify);
    }

    QVariant colorVariant = model->data(model->index(0,0), Qt::BackgroundColorRole);
    if (colorVariant.isValid()) {
        Q_ASSERT(qVariantCanConvert<QColor>(colorVariant));
    }

    colorVariant = model->data(model->index(0,0), Qt::TextColorRole);
    if (colorVariant.isValid()) {
        Q_ASSERT(qVariantCanConvert<QColor>(colorVariant));
    }

    QVariant checkStateVariant = model->data(model->index(0,0), Qt::CheckStateRole);
    if (checkStateVariant.isValid()) {
        int state = checkStateVariant.toInt();
        Q_ASSERT(state == Qt::Unchecked ||
                state == Qt::PartiallyChecked ||
                state == Qt::Checked);
    }
}

#include "modeltest.moc"
