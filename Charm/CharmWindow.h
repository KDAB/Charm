#ifndef CHARMWINDOW_H
#define CHARMWINDOW_H

#include <QMainWindow>

#include "Core/ViewInterface.h"
#include "Core/CommandEmitterInterface.h"

class QAction;
class QShortcut;

class CharmWindow : public QMainWindow,
                    public ViewInterface,
                    public CommandEmitterInterface
{
    Q_OBJECT

public:
    explicit CharmWindow( const QString& name, QWidget* parent = 0 );

    QAction* showHideAction();

    const QString& windowName() const;
    const QString& windowIdentfier() const;
    int windowNumber() const;

    virtual QToolBar* toolBar() const;

protected:
    /** The window name is the human readable name the application uses to reference the window.
     */
    void setWindowName( const QString& name );
    /** The window identifier is used to reference window specific configuration groups, et cetera.
     * It is generally not recommend to change it once the application is in use. */
    void setWindowIdentifier( const QString& id );
    /** The window number is a Mac concept that allows to pull up application windows by entering CMD+<number>.
     */
    void setWindowNumber( int number );
    /** Insert the Edit menu. Empty by default. */
    virtual void insertEditMenu() {}

public:
    /* reimpl */ void stateChanged( State previous );
    /* reimpl */ void showEvent( QShowEvent* );
    /* reimpl */ void hideEvent( QHideEvent* );
    /* reimpl */ void keyPressEvent( QKeyEvent* event );

    virtual void saveGuiState();
    virtual void restoreGuiState();

    static bool showHideView( QWidget* w );

signals:
    /* reimpl */ void visibilityChanged( bool );
    /* reimpl */ void saveConfiguration();

public slots:
    /* reimpl */ void sendCommandRollback( CharmCommand* );
    /* reimpl */ void sendCommand( CharmCommand* );
    /* reimpl */ void commitCommand( CharmCommand* );
    void restore();
    void showHideView();
    void configurationChanged();

private:
    void handleShowHide( bool visible );
    QString m_windowName;
    QAction* m_showHideAction;
    int m_windowNumber; // Mac numerical window number, used for shortcut etc
    QString m_windowIdentifier;
    QShortcut* m_shortcut;
    QToolBar* m_toolBar;
};

#endif
