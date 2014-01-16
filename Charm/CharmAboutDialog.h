#ifndef CHARMABOUTDIALOG_H
#define CHARMABOUTDIALOG_H

#include <QDialog>

namespace Ui {
    class CharmAboutDialog;
}

class CharmAboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CharmAboutDialog( QWidget* parent = 0 );
    ~CharmAboutDialog();

private:
    Ui::CharmAboutDialog* m_ui;
};

#endif
