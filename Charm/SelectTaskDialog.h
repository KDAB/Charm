#ifndef SELECTTASKDIALOG_H
#define SELECTTASKDIALOG_H

#include <QDialog>
#include <QHash>
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
    explicit SelectTaskDialogProxy( CharmDataModel*, QObject* parent = 0 );

    Qt::ItemFlags flags( const QModelIndex & index ) const;
    QVariant data( const QModelIndex& index, int role = Qt::DisplayRole ) const;

protected:
    /*! \reimp */ bool filterAcceptsColumn( int column, const QModelIndex& parent ) const;
};

class SelectTaskDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SelectTaskDialog( QWidget* parent=0 );
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
    bool isValidAndTrackable( const QModelIndex& index ) const;

private:
    Ui::SelectTaskDialog* m_ui;
    TaskId m_selectedTask;
    SelectTaskDialogProxy m_proxy;
    QHash<TaskId,bool> m_expansionStates;
};

#endif
