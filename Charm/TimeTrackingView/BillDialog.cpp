#include "BillDialog.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLayout>

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

    QVBoxLayout *layout = new QVBoxLayout(this);
    m_asYouWish = new QPushButton("As you wish");
    connect(m_asYouWish, SIGNAL(pressed()), SLOT(slotAsYouWish()));
    m_alreadyDone = new QPushButton("Already done");
    connect(m_alreadyDone, SIGNAL(pressed()), SLOT(slotAlreadyDone()));

    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->addWidget(m_asYouWish, 0, Qt::AlignLeft);
    hlayout->addWidget(m_alreadyDone, 0, Qt::AlignRight);
    hlayout->setAlignment(Qt::AlignBottom);
    layout->addLayout(hlayout);
}

void BillDialog::setReport(int year, int week)
{
    m_year = year;
    m_week = week;
    m_alreadyDone->setText(QString("Week %1 (%2) is already done").arg(week).arg(year));
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

#include "BillDialog.moc"
