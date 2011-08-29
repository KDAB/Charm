#ifndef HTCONFIGDIALOG_H
#define HTCONFIGDIALOG_H 1

#include <QDialog>

class HTClient;

namespace Ui { class HTConfigDialog; }

class HTConfigDialog : public QDialog
{
    Q_OBJECT;

    HTClient& m_client;
    Ui::HTConfigDialog* m_ui;

public:

    HTConfigDialog(HTClient &client);
    virtual ~HTConfigDialog();

public:

    /*reimpl*/ void accept();

};

#endif
