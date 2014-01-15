#ifndef IDLECORRECTIONDIALOG_H
#define IDLECORRECTIONDIALOG_H

#include <QDialog>

namespace Ui {
    class IdleCorrectionDialog;
}

class IdleCorrectionDialog : public QDialog
{
    Q_OBJECT

public:
    enum Result {
        Idle_NoResult,
        Idle_Ignore,
        Idle_EndEvent
    };

    explicit IdleCorrectionDialog( QWidget* parent = nullptr );
    ~IdleCorrectionDialog();

    Result result();

private:
    Ui::IdleCorrectionDialog* m_ui;
};

#endif
