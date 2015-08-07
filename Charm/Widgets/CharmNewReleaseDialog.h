#ifndef CHARMNEWRELEASEDIALOG
#define CHARMNEWRELEASEDIALOG

#include <QDialog>
#include <QScopedPointer>
#include <QUrl>

namespace Ui {
    class CharmNewReleaseDialog;
}

class CharmNewReleaseDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CharmNewReleaseDialog( QWidget* parent = nullptr );
    ~CharmNewReleaseDialog();

    void setVersion( const QString& newVersion , const QString& localVersion );
    void setDownloadLink( const QUrl& link );
    void setReleaseInformationLink( const QString& link );

private slots:
    void slotLaunchBrowser();
    void slotSkipVersion();
    void slotRemindMe();

private:
    QUrl m_link;
    QString m_version;
    QPushButton* m_skipUpdate;
    QPushButton* m_remindMeLater;
    QPushButton* m_update;
    QScopedPointer<Ui::CharmNewReleaseDialog> m_ui;
};

#endif // CHARMNEWRELEASEDIALOG

