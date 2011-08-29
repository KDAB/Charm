#ifndef HTCLIENTDIALOG_H
#define HTCLIENTDIALOG_H 1

#include <QDialog>

class HTClient;

namespace Ui { class HTClientDialog; }

class HTClientDialog : public QDialog
{
    Q_OBJECT;

    HTClient& m_client;
    Ui::HTClientDialog* m_ui;
    bool m_finished;

public:

    HTClientDialog(HTClient &client);
    virtual ~HTClientDialog();

public slots:

    void slotSuccess();
    void slotFailure();

protected:

    void setStatus(const QString &str);

    // reimpl
    void showEvent( QShowEvent* e );

    // reimpl
    void closeEvent( QCloseEvent* e );

};

#endif
