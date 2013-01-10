#ifndef BillDialog_H
#define BillDialog_H

#include <QDialog>

class BillDialog : public QDialog
{
    Q_OBJECT
public:
    enum BillResponse
    {
        Later,
        AsYouWish,
        AlreadyDone,
    };
    explicit BillDialog(QWidget* parent = 0, Qt::WindowFlags f = 0);
    void setReport(int year, int week);
    int year() const;
    int week() const;
private slots:
    void slotAsYouWish();
    void slotAlreadyDone();
private:
    QPushButton *m_asYouWish;
    QPushButton *m_alreadyDone;
    int m_year;
    int m_week;
};

#endif
