#ifndef XMLSERIALIZATIONTESTS_H
#define XMLSERIALIZATIONTESTS_H

#include <QObject>

class XmlSerializationTests : public QObject
{
    Q_OBJECT

public:
    XmlSerializationTests();

private slots:
    void testEventSerialization();
    void testQDateTimeToFromString();
};

#endif
