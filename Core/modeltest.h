#ifndef MODELTEST_H
#define MODELTEST_H

/****************************************************************************
**
** Copyright (C) 1992-$THISYEAR$ Trolltech AS. All rights reserved.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include <QtCore/QtCore>

class Modeltest : public QObject
{
    Q_OBJECT
public:
    Modeltest(QAbstractItemModel *_model, QObject *parent=0) : QObject(parent){
        model = _model;
connect(model, SIGNAL(columnsAboutToBeInserted(const QModelIndex &, int, int)), this, SLOT(runAllTests()));
connect(model, SIGNAL(columnsAboutToBeRemoved(const QModelIndex &, int, int)), this, SLOT(runAllTests()));
connect(model, SIGNAL(columnsInserted(const QModelIndex &, int, int)), this, SLOT(runAllTests()));
connect(model, SIGNAL(columnsRemoved(const QModelIndex &, int, int)), this, SLOT(runAllTests()));
connect(model, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)), this, SLOT(runAllTests()));
connect(model, SIGNAL(headerDataChanged(Qt::Orientation, int, int)), this, SLOT(runAllTests()));
connect(model, SIGNAL(layoutAboutToBeChanged ()), this, SLOT(runAllTests()));
connect(model, SIGNAL(layoutChanged ()), this, SLOT(runAllTests()));
connect(model, SIGNAL(modelReset ()), this, SLOT(runAllTests()));
connect(model, SIGNAL(rowsAboutToBeInserted(const QModelIndex &, int, int)), this, SLOT(runAllTests()));
connect(model, SIGNAL(rowsAboutToBeRemoved(const QModelIndex &, int, int)), this, SLOT(runAllTests()));
connect(model, SIGNAL(rowsInserted(const QModelIndex &, int, int)), this, SLOT(runAllTests()));
connect(model, SIGNAL(rowsRemoved(const QModelIndex &, int, int)), this, SLOT(runAllTests()));

connect(model, SIGNAL(rowsAboutToBeInserted(const QModelIndex &, int, int)), this, SLOT(rowsAboutToBeInserted(const QModelIndex &, int, int)));
connect(model, SIGNAL(rowsAboutToBeRemoved(const QModelIndex &, int, int)), this, SLOT(rowsAboutToBeRemoved(const QModelIndex &, int, int)));
connect(model, SIGNAL(rowsInserted(const QModelIndex &, int, int)), this, SLOT(rowsInserted(const QModelIndex &, int, int)));
connect(model, SIGNAL(rowsRemoved(const QModelIndex &, int, int)), this, SLOT(rowsRemoved(const QModelIndex &, int, int)));

        runAllTests();

    }

private slots:
    void nonDestructiveBasicTest();
    void rowCount();
    void columnCount();
    void hasIndex();
    void index();
    void parent();
    void data();

protected slots:
    void runAllTests(){
        nonDestructiveBasicTest();
        rowCount();
        columnCount();
        hasIndex();
        index();
        parent();
        data();
    };

private:
    struct Changing {
        QModelIndex parent;
        int oldSize;
        QVariant last;
        QVariant next;
    };

protected slots:
    void rowsAboutToBeInserted(const QModelIndex &parent, int start, int end) {
        Q_UNUSED(end);
        Changing c;
        c.parent = parent;
        c.oldSize = model->rowCount(parent);
        c.last = model->data(model->index(start - 1, 0, parent));
        c.next = model->data(model->index(start, 0, parent));
        insert.push(c);
    }

    void rowsInserted(const QModelIndex & parent, int start, int end) {
        Changing c = insert.pop();
        Q_ASSERT(c.parent == parent);
        Q_ASSERT(c.oldSize + (end - start + 1) == model->rowCount(parent));
        Q_ASSERT(c.last == model->data(model->index(start - 1, 0, c.parent)));
        /*
        if (c.next != model->data(model->index(end + 1, 0, c.parent))) {
            qDebug() << start << end;
            for (int i=0; i < model->rowCount(); ++i)
                qDebug() << model->index(i, 0).data().toString();
            qDebug() << c.next << model->data(model->index(end + 1, 0, c.parent));
        }
        */
        Q_ASSERT(c.next == model->data(model->index(end + 1, 0, c.parent)));
    }

    void rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end) {
        Changing c;
        c.parent = parent;
        c.oldSize = model->rowCount(parent);
        c.last = model->data(model->index(start - 1, 0, parent));
        c.next = model->data(model->index(end + 1, 0, parent));
        remove.push(c);
    }

    void rowsRemoved(const QModelIndex & parent, int start, int end) {
        Changing c = remove.pop();
        Q_ASSERT(c.parent == parent);
        Q_ASSERT(c.oldSize - (end - start + 1) == model->rowCount(parent));
        Q_ASSERT(c.last == model->data(model->index(start - 1, 0, c.parent)));
        Q_ASSERT(c.next == model->data(model->index(start, 0, c.parent)));
    }

private:
    QAbstractItemModel *model;
    QStack<Changing> insert;
    QStack<Changing> remove;
};


#endif
