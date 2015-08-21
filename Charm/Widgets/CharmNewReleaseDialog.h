/*
  CharmNewReleaseDialog.h

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2015-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Michel Boyer de la Giroday <michel.giroday@kdab.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
