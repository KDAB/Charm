#ifndef APPLICATIONFACTORY_H
#define APPLICATIONFACTORY_H

class QApplication;

class ApplicationFactory
{
public:
    static QApplication* localApplication( int& argc, char** argv );
private:
    ApplicationFactory();
};

#endif // APPLICATIONFACTORY_H
