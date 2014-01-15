#ifndef DATAENTRYSYNCER_H_
#define DATAENTRYSYNCER_H_

#include <QObject>

class QSpinBox;
class QDateEdit;

class DateEntrySyncer : public QObject
{
    Q_OBJECT
public:
    DateEntrySyncer( QSpinBox* weekNumberSB, QSpinBox* yearSB, QDateEdit* dateedit, int weekDay=1, QObject* parent=nullptr );

private Q_SLOTS:
    void dateSelectionChanged();

private:
    QSpinBox* m_week;
    QSpinBox* m_year;
    QDateEdit* m_date;
    int m_weekDay;
};

#endif //DATAENTRYSYNCER_H_

