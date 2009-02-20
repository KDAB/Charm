#ifndef SELECTTASKDIALOG_H
#define SELECTTASKDIALOG_H

#include <QDialog>
#include <QSortFilterProxyModel>

#include "ViewFilter.h"

class ViewFilter;
class CharmDataModel;

namespace Ui {
	class SelectTaskDialog;
}
class SelectTaskDialogProxy : public ViewFilter
{
    Q_OBJECT

public:
    SelectTaskDialogProxy( CharmDataModel*, QObject* parent = 0 );

    Qt::ItemFlags flags( const QModelIndex & index ) const;
    QVariant data( const QModelIndex& index, int role = Qt::DisplayRole ) const;

protected:
    /*! \reimp */ bool filterAcceptsColumn( int column, const QModelIndex& parent ) const;
};

class SelectTaskDialog : public QDialog
{
    Q_OBJECT

public:
    SelectTaskDialog( QWidget* parent );
    ~SelectTaskDialog();

    TaskId selectedTask() const;

protected:
    void showEvent ( QShowEvent * event );
    void hideEvent( QHideEvent* event );

private slots:
    void slotCurrentItemChanged( const QModelIndex&, const QModelIndex& );
    void slotDoubleClicked ( const QModelIndex & );
    void slotFilterTextChanged( const QString& );
    void slotAccepted();

private:
    Ui::SelectTaskDialog* m_ui;
    TaskId m_selectedTask;
    SelectTaskDialogProxy m_proxy;
};

#endif
