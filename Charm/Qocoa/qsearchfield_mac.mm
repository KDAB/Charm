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

#include "qocoa_mac.h"

#import "Foundation/NSAutoreleasePool.h"
#import "Foundation/NSNotification.h"
#import "AppKit/NSSearchField.h"

class QSearchFieldPrivate : public QObject
{
public:
    QSearchFieldPrivate(QSearchField *qSearchField, NSSearchField *nsSearchField)
        : QObject(qSearchField), qSearchField(qSearchField), nsSearchField(nsSearchField) {}

    void textDidChange(const QString &text)
    {
        if (qSearchField)
            emit qSearchField->textChanged(text);
    }

    void textDidEndEditing()
    {
        if (qSearchField)
            emit qSearchField->editingFinished();
    }

    QPointer<QSearchField> qSearchField;
    NSSearchField *nsSearchField;
};

@interface QSearchFieldDelegate : NSObject<NSTextFieldDelegate>
{
@public
    QPointer<QSearchFieldPrivate> pimpl;
}
-(void)controlTextDidChange:(NSNotification*)notification;
-(void)controlTextDidEndEditing:(NSNotification*)notification;
@end

@implementation QSearchFieldDelegate
-(void)controlTextDidChange:(NSNotification*)notification {
    Q_ASSERT(pimpl);
    if (pimpl)
        pimpl->textDidChange(toQString([[notification object] stringValue]));
}

-(void)controlTextDidEndEditing:(NSNotification*)notification {
    Q_UNUSED(notification);
    // No Q_ASSERT here as it is called on destruction.
    if (pimpl)
        pimpl->textDidEndEditing();
}
@end

QSearchField::QSearchField(QWidget *parent) : QWidget(parent)
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

    NSSearchField *search = [[NSSearchField alloc] init];

    QSearchFieldDelegate *delegate = [[QSearchFieldDelegate alloc] init];
    delegate->pimpl = new QSearchFieldPrivate(this, search);
    [search setDelegate:delegate];

    setupLayout(search, this);

    setFixedHeight(24);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    [search release];

    [pool drain];
}

void QSearchField::setText(const QString &text)
{
    Q_ASSERT(pimpl);
    if (!pimpl)
        return;

    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    [pimpl->nsSearchField setStringValue:fromQString(text)];
    [pool drain];
}

void QSearchField::setPlaceholderText(const QString &text)
{
    Q_ASSERT(pimpl);
    if (!pimpl)
        return;

    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    [[pimpl->nsSearchField cell] setPlaceholderString:fromQString(text)];
    [pool drain];
}

void QSearchField::clear()
{
    Q_ASSERT(pimpl);
    if (!pimpl)
        return;

    [pimpl->nsSearchField setStringValue:@""];
    emit textChanged(QString());
}

QString QSearchField::text() const
{
    Q_ASSERT(pimpl);
    if (!pimpl)
        return QString();

    return toQString([pimpl->nsSearchField stringValue]);
}

void QSearchField::resizeEvent(QResizeEvent *resizeEvent)
{
    QWidget::resizeEvent(resizeEvent);
}
