#ifndef EVENTVIEW_H
#define EVENTVIEW_H

#include <QWidget>
#include <QAction>

#include "Core/Event.h"
#include "Core/TimeSpans.h"
#include "Core/CommandEmitterInterface.h"

#include "ViewModeInterface.h"

class QModelIndex;

class EventDisplay;
class CharmCommand;
class QAbstractItemModel;
class EventModelFilter;

namespace Ui {
    class EventView;
}

class EventView : public QWidget,
                  public ViewModeInterface,
                  public CommandEmitterInterface
{
    Q_OBJECT

public:
    explicit EventView( QWidget* parent );
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
    void slotEditEventCompleted( const Event& );
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

private:
    Event newSettings();
    void setCurrentEvent( const Event& );

    QList<NamedTimeSpan> m_timeSpans;
    Ui::EventView* m_ui;
    Event m_event;
    EventModelFilter* m_model;
    QAction m_actionNewEvent;
    QAction m_actionEditEvent;
    QAction m_actionDeleteEvent;
};

#endif
