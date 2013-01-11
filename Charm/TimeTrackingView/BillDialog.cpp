#include "BillDialog.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLayout>
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
    connect(m_asYouWish, SIGNAL(pressed()), SLOT(slotAsYouWish()));
    m_alreadyDone = new QPushButton("Already done");
    connect(m_alreadyDone, SIGNAL(pressed()), SLOT(slotAlreadyDone()));
    m_later = new QPushButton("Later");
    connect(m_later, SIGNAL(pressed()), SLOT(slotLater()));

    QVBoxLayout *layout = new QVBoxLayout(this);
    QDialogButtonBox *buttonBox = new QDialogButtonBox();
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

#include "BillDialog.moc"
