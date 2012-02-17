/*
Copyright (C) 2011 by Mike McQuaid

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "qsearchfield.h"

#include <QLineEdit>
#include <QVBoxLayout>
#include <QToolButton>
#include <QStyle>

#include <QDir>
#include <QDebug>

class QSearchFieldPrivate
{
public:
    QSearchFieldPrivate(QLineEdit *lineEdit, QToolButton *clearButton)
        : lineEdit(lineEdit), clearButton(clearButton) {}
    QLineEdit *lineEdit;
    QToolButton *clearButton;
};

QSearchField::QSearchField(QWidget *parent) : QWidget(parent)
{
    QLineEdit *lineEdit = new QLineEdit(this);
    connect(lineEdit, SIGNAL(textChanged(QString)),
            this, SIGNAL(textChanged(QString)));
    connect(lineEdit, SIGNAL(editingFinished()),
            this, SIGNAL(editingFinished()));
    connect(lineEdit, SIGNAL(textChanged(QString)),
            this, SLOT(setText(QString)));

    QToolButton *clearButton = new QToolButton(this);
    QPixmap clearIcon(QString(":/Qocoa/qsearchfield_nonmac.png"));
    clearButton->setIcon(QIcon(clearIcon));
    clearButton->setIconSize(clearIcon.size());
    clearButton->setFixedSize(clearIcon.size());
    clearButton->setStyleSheet("border: none;");
    clearButton->hide();
    qDebug() << clearButton->size() << QDir(":/").entryList();
    connect(clearButton, SIGNAL(clicked()), this, SLOT(clear()));

    const int frameWidth = lineEdit->style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
    lineEdit->setStyleSheet(QString("QLineEdit { padding-right: %1px; } ").arg(clearIcon.width() - frameWidth - 1));
    const int width = qMax(lineEdit->minimumSizeHint().width(), clearButton->width() + frameWidth * 2);
    const int height = qMax(lineEdit->minimumSizeHint().height(), clearButton->height() + frameWidth * 2);
    lineEdit->setMinimumSize(width, height);

    pimpl = new QSearchFieldPrivate(lineEdit, clearButton);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->addWidget(lineEdit);
}

void QSearchField::setText(const QString &text)
{
    pimpl->clearButton->setVisible(!text.isEmpty());

    if (text != this->text())
        pimpl->lineEdit->setText(text);
}

void QSearchField::setPlaceholderText(const QString &text)
{
    pimpl->lineEdit->setPlaceholderText(text);
}

void QSearchField::clear()
{
    pimpl->lineEdit->clear();
}

QString QSearchField::text() const
{
    return pimpl->lineEdit->text();
}

void QSearchField::resizeEvent(QResizeEvent *resizeEvent)
{
    QWidget::resizeEvent(resizeEvent);
    const int frameWidth = pimpl->lineEdit->style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
    const int x = sizeHint().width() - pimpl->clearButton->width() - frameWidth;
    const int y = sizeHint().height() - pimpl->clearButton->height()/2 - frameWidth*2;
    pimpl->clearButton->move(x, y);
}
