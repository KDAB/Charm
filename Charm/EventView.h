#ifndef EVENTVIEW_H
#define EVENTVIEW_H

#include <QWidget>
#include <QAction>
#include <QUndoStack>

#include "Core/Event.h"
#include "Core/TimeSpans.h"
#include "Core/CommandEmitterInterface.h"

#include "ViewModeInterface.h"
#include "UndoCharmCommandWrapper.h"

class QModelIndex;

class EventDisplay;
class CharmCommand;
class QAbstractItemModel;
class EventModelFilter;
class QToolBar;
class QComboBox;
class QLabel;
class QListView;

class EventView : public QWidget,
                  public ViewModeInterface,
                  public CommandEmitterInterface
{
    Q_OBJECT

public:
    explicit EventView( QToolBar* toolBar, QWidget* parent );
    ~EventView();

    void closeEvent( QCloseEvent* );

    void reject();

    void makeVisibleAndCurrent( const Event& );

    // implement ViewModeInterface:
    void saveGuiState();
    void restoreGuiState();
    void stateChanged( State previous );
    void configurationChanged();
    void setModel( ModelConnector* );

    void populateEditMenu( QMenu* );

signals:
    void visible( bool );
    void emitCommand( CharmCommand* );
    void emitCommandRollback( CharmCommand* );

public slots:
    void commitCommand( CharmCommand* );
    void delayedInitialization();
    void timeSpansChanged();
    void timeFrameChanged(int );
    void slotConfigureUi();

private slots:
    void slotEventDoubleClicked( const QModelIndex& );
    void slotEditEvent();
    void slotEditEvent( const Event& );
    void slotEditNewEventCompleted( const Event& );
    void slotCurrentItemChanged( const QModelIndex&, const QModelIndex& );
    void slotContextMenuRequested( const QPoint& );
    void slotNextEvent();
    void slotPreviousEvent();
    void slotNewEvent();
    void slotDeleteEvent();
    void slotEventActivated( EventId );
    void slotEventDeactivated( EventId );
    void slotUpdateTotal();
    void slotUpdateCurrent();
    void slotUndoTextChanged(const QString&);
    void slotRedoTextChanged(const QString&);
    void slotEventIdChanged(int oldId, int newId);

private:
    Event newSettings();
    void setCurrentEvent( const Event& );
    void stageCommand( CharmCommand* );

    QUndoStack* m_undoStack;
    QList<NamedTimeSpan> m_timeSpans;
    Event m_event;
    EventModelFilter* m_model;
    QAction m_actionUndo;
    QAction m_actionRedo;
    QAction m_actionNewEvent;
    QAction m_actionEditEvent;
    QAction m_actionDeleteEvent;
    QAction m_actionCreateTimeSheet;
    QComboBox* m_comboBox;
    QLabel* m_labelTotal;
    QListView* m_listView;
};

#endif
