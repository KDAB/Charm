/*
  ReportPreviewWindow.cpp

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

#include "ReportPreviewWindow.h"
#include "ViewHelpers.h"

#ifndef QT_NO_PRINTER
#include <QPrinter>
#include <QPrintDialog>
#endif

#include "ui_ReportPreviewWindow.h"

ReportPreviewWindow::ReportPreviewWindow(QWidget *parent)
    : QDialog(parent)
    , m_ui(new Ui::ReportPreviewWindow)
{
    m_ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    connect(m_ui->pushButtonClose, &QPushButton::clicked,
            this, &ReportPreviewWindow::slotClose);
    connect(m_ui->pushButtonUpdate, &QPushButton::clicked,
            this, &ReportPreviewWindow::slotUpdate);
    connect(m_ui->pushButtonSave, &QPushButton::clicked,
            this, &ReportPreviewWindow::slotSaveToXml);
    connect(m_ui->pushButtonSaveTotals, &QPushButton::clicked,
            this, &ReportPreviewWindow::slotSaveToText);
    connect(m_ui->textBrowser, &QTextBrowser::anchorClicked,
            this, &ReportPreviewWindow::anchorClicked);
    connect(m_ui->pushButtonNext, &QPushButton::clicked,
            this, &ReportPreviewWindow::nextClicked);
    connect(m_ui->pushButtonPrevious, &QPushButton::clicked,
            this, &ReportPreviewWindow::previousClicked);
#ifndef QT_NO_PRINTER
    connect(m_ui->pushButtonPrint, &QPushButton::clicked,
            this, &ReportPreviewWindow::slotPrint);
#else
    m_ui->pushButtonPrint->setEnabled(false);
#endif

    m_updateTimer.setInterval(60 * 1000);
    m_updateTimer.start();
    connect(&m_updateTimer, &QTimer::timeout,
            this, &ReportPreviewWindow::slotUpdate);

    resize(850, 600);
}

ReportPreviewWindow::~ReportPreviewWindow()
{
}

void ReportPreviewWindow::setDocument(const QTextDocument *document)
{
    if (document != nullptr) {
        // we keep a copy, to be able to show different versions of the same document
        QScopedPointer<QTextDocument> docClone(document->clone());
        m_document.swap(docClone);
        m_ui->textBrowser->setDocument(m_document.data());
    } else {
        m_ui->textBrowser->setDocument(nullptr);
        m_document.reset();
    }
}

void ReportPreviewWindow::setTimeSpanTypeName(const QString &name)
{
    m_ui->pushButtonPrevious->setText(tr("Previous %1").arg(name));
    m_ui->pushButtonNext->setText(tr("Next %1").arg(name));
}

QDomDocument ReportPreviewWindow::createReportTemplate() const
{
    // create XHTML v1.0 structure:
    QDomDocument doc(QStringLiteral("html"));
    // FIXME this is only a rudimentary subset of a valid xhtml 1 document

    // html element
    QDomElement html = doc.createElement(QStringLiteral("html"));
    html.setAttribute(QStringLiteral("xmlns"), QStringLiteral("http://www.w3.org/1999/xhtml"));
    doc.appendChild(html);

    // head and body, children of html
    QDomElement head = doc.createElement(QStringLiteral("head"));
    html.appendChild(head);
    QDomElement body = doc.createElement(QStringLiteral("body"));
    html.appendChild(body);

    return doc;
}

QPushButton *ReportPreviewWindow::saveToXmlButton() const
{
    return m_ui->pushButtonSave;
}

QPushButton *ReportPreviewWindow::saveToTextButton() const
{
    return m_ui->pushButtonSaveTotals;
}

QPushButton *ReportPreviewWindow::uploadButton() const
{
    return m_ui->pushButtonUpload;
}

void ReportPreviewWindow::slotSaveToXml()
{
}

void ReportPreviewWindow::slotSaveToText()
{
}

void ReportPreviewWindow::slotPrint()
{
#ifndef QT_NO_PRINTER
    QPrinter printer;
    QPrintDialog dialog(&printer, this);

    if (dialog.exec())
        m_document->print(&printer);

#endif
}

void ReportPreviewWindow::slotUpdate()
{
}

void ReportPreviewWindow::slotClose()
{
    close();
}
