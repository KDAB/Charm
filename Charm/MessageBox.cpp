#include "MessageBox.h"

#include <QAbstractButton>
#include <QPointer>

static int messageBox( QWidget* parent,
                       const QString& title,
                       const QString& text,
                       QMessageBox::StandardButton yesButton,
                       const QString& yesLabel,
                       QMessageBox::StandardButton noButton,
                       const QString& noLabel,
                       QMessageBox::StandardButton defaultButton,
                       QMessageBox::Icon icon )
{
    QPointer<QMessageBox> messageBox( new QMessageBox( parent ) );
    messageBox->setWindowTitle( title );
    messageBox->setIcon( icon );
    messageBox->setText( text );
    messageBox->setStandardButtons( yesButton|noButton );
    messageBox->button( yesButton )->setText( yesLabel );
    messageBox->button( noButton )->setText( noLabel );
    messageBox->setDefaultButton( defaultButton );
    const int result = messageBox->exec();
    delete messageBox;
    return result;
}

int MessageBox::question( QWidget* parent,
    const QString& title,
    const QString& text,
    const QString& yesLabel,
    const QString& noLabel,
    QMessageBox::StandardButton defaultButton )
{
    return messageBox( parent, title, text, QMessageBox::Yes, yesLabel, QMessageBox::No, noLabel, defaultButton, QMessageBox::Question );
}

int MessageBox::warning( QWidget* parent,
    const QString& title,
    const QString& text,
    const QString& yesLabel,
    const QString& noLabel,
    QMessageBox::StandardButton defaultButton )
{
    return messageBox( parent, title, text, QMessageBox::Yes, yesLabel, QMessageBox::No, noLabel, defaultButton, QMessageBox::Warning );
}
