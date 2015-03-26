/*
  BillDialog.cpp

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2014-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

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

#include "BillDialog.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QDialogButtonBox>

BillDialog::BillDialog( QWidget* parent, Qt::WindowFlags f )
: QDialog(parent, f), m_year( 0 ), m_week( 0 )
{
    setResult(Later);
    QPalette p = palette();
    QImage billImage(":/Charm/bill.jpg");
    QBrush billBrush(billImage);
    p.setBrush(QPalette::Window, billBrush);
    setPalette(p);
    setAutoFillBackground(true);
    setMinimumSize(billImage.size());
    setMaximumSize(billImage.size());
    setWindowTitle("Yeah... about those timesheets...");

    m_asYouWish = new QPushButton("As you wish");
    connect(m_asYouWish, SIGNAL(clicked()), SLOT(slotAsYouWish()));
    m_alreadyDone = new QPushButton("Already done");
    connect(m_alreadyDone, SIGNAL(clicked()), SLOT(slotAlreadyDone()));
    m_later = new QPushButton("Later");
    connect(m_later, SIGNAL(clicked()), SLOT(slotLater()));

    auto layout = new QVBoxLayout(this);
    auto buttonBox = new QDialogButtonBox();
    buttonBox->addButton(m_asYouWish, QDialogButtonBox::YesRole);
    buttonBox->addButton(m_alreadyDone, QDialogButtonBox::NoRole);
    buttonBox->addButton(m_later, QDialogButtonBox::RejectRole);
    layout->addWidget(buttonBox, 0, Qt::AlignBottom);
}

void BillDialog::setReport(int year, int week)
{
    m_year = year;
    m_week = week;
    m_alreadyDone->setText(QString("Already sent Week %1 (%2)").arg(week).arg(year));
}

int BillDialog::year() const
{
    return m_year;
}

int BillDialog::week() const
{
    return m_week;
}

void BillDialog::slotAsYouWish()
{
    done(AsYouWish);
}

void BillDialog::slotAlreadyDone()
{
    done(AlreadyDone);
}

void BillDialog::slotLater()
{
    done(Later);
}

#include "moc_BillDialog.cpp"
