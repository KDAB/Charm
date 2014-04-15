#ifndef COMMENTEDITORPOPUP_H
#define COMMENTEDITORPOPUP_H

#include <QDialog>

#include "Core/Event.h"

namespace Ui {
class CommentEditorPopup;
}

class CommentEditorPopup : public QDialog
{
    Q_OBJECT

public:
    explicit CommentEditorPopup( QWidget *parent = 0 );
    ~CommentEditorPopup();

public Q_SLOTS:
    void loadEvent( EventId id );
    void accept();

private:
    Ui::CommentEditorPopup *ui;
    EventId m_id;
};

#endif // COMMENTEDITORPOPUP_H
