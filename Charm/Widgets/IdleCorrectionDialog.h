#ifndef IDLECORRECTIONDIALOG_H
#define IDLECORRECTIONDIALOG_H

#include <QDialog>
#include <QScopedPointer>

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

    explicit IdleCorrectionDialog( QWidget* parent = 0 );
    ~IdleCorrectionDialog();

    Result result() const;

private:
    QScopedPointer<Ui::IdleCorrectionDialog> m_ui;
};

#endif
