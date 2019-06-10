/*
  ReportPreviewWindow.h

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2014-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Frank Osterfeld <frank.osterfeld@kdab.com>

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

#ifndef REPORTPREVIEWWINDOW_H
#define REPORTPREVIEWWINDOW_H

#include <QDialog>
#include <QDomDocument>
#include <QScopedPointer>
#include <QTextDocument>
#include <QTimer>

namespace Ui {
class ReportPreviewWindow;
}

class QPushButton;

class ReportPreviewWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ReportPreviewWindow(QWidget *parent = nullptr);
    ~ReportPreviewWindow() override;

Q_SIGNALS:
    void anchorClicked(const QUrl &which);
    void nextClicked();
    void previousClicked();

protected:
    void setDocument(const QTextDocument *document);
    void setTimeSpanTypeName(const QString &name);
    QDomDocument createReportTemplate() const;
    QPushButton *saveToXmlButton() const;
    QPushButton *saveToTextButton() const;
    QPushButton *uploadButton() const;

    QTimer m_updateTimer;

private Q_SLOTS:
    virtual void slotSaveToXml();
    virtual void slotSaveToText();
    virtual void slotPrint();
    virtual void slotUpdate();
    virtual void slotClose();

private:
    QScopedPointer<Ui::ReportPreviewWindow> m_ui;
    QScopedPointer<QTextDocument> m_document;
    QString m_timeSpanTypeName;
};

#endif
