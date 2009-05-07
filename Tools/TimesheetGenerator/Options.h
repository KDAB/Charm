#ifndef OPTIONS_H
#define OPTIONS_H

#include <QString>
#include <QDate>

namespace TimesheetGenerator {

    class Options {
    public:
        explicit Options( int argc, char** argv );

        QString file() const;
        QDate date() const;

    private:
        QString mFile;
        QDate mDate;
    };

}

#endif
