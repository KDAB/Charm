#include <QDomDocument>
#include <QCoreApplication>

#include "Core/XmlSerialization.h"

class AnonymizerException {
public:
    explicit AnonymizerException( QString what )
        : m_what( what ) {}

private:
    QString m_what;
};

int main( int argc, char** argv ) {
    QCoreApplication app( argc, argv );
    try {
        if ( argc != 2 )
        QFileInfo fileInfo( argv[1] );

}

