#ifndef CHARM_MESSAGEBOX_H
#define CHARM_MESSAGEBOX_H

#include <QMessageBox>

namespace MessageBox
{
int question( QWidget* parent,
              const QString& title,
              const QString& text,
              const QString& yesLabel,
              const QString& noLabel,
              QMessageBox::StandardButton defaultButton=QMessageBox::NoButton );

int warning( QWidget* parent,
              const QString& title,
              const QString& text,
              const QString& yesLabel,
              const QString& noLabel,
              QMessageBox::StandardButton defaultButton=QMessageBox::NoButton );

}
#endif
