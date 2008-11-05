#ifndef TESTHELPERS_H
#define TESTHELPERS_H

namespace TestHelpers {

    QList<QDomElement> retrieveTestCases( QString path, QString type )
    {
        const QString tagName( "testcase" );
        QStringList filenamePatterns;
        filenamePatterns << "*.xml";

        QDir dataDir( path );
        if ( !dataDir.exists() ) {
            throw CharmException( "path to test case data does not exist" );
        }

        QFileInfoList dataSets = dataDir.entryInfoList( filenamePatterns, QDir::Files, QDir::Name );

        QList<QDomElement> result;
        Q_FOREACH( QFileInfo fileinfo, dataSets ) {
            QDomDocument doc( "charmtests" );
            QFile file( fileinfo.filePath() );
            if ( ! file.open( QIODevice::ReadOnly ) ) {
                throw CharmException( "unable to open input file" );
            }

            if ( !doc.setContent( &file ) ) {
                throw CharmException( "invalid DOM document, cannot load" );
            }

            QDomElement root = doc.firstChildElement();
            if ( root.tagName() != "testcases" ) {
                throw CharmException( "root element (testcases) not found" );
            }

            qDebug() << "Loading test cases from" << file.fileName();

            for ( QDomElement child = root.firstChildElement( tagName ); !child.isNull();
                  child = child.nextSiblingElement( tagName ) ) {
                if ( child.attribute( "type" ) == type ) {
                    result << child;
                }
            }
        }
        return result;
    }

    bool attribute( const QString& name, const QDomElement& element )
    {
        QString text = element.attribute( "expectedResult" );
        if ( text != "false" && text != "true" ) {
            throw CharmException( "attribute does not represent a boolean" );
        }
        return ( text == "true" );
    }

}

#endif
