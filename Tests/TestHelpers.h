/*
  TestHelpers.h

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2008-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Mirko Boehm <mirko.boehm@kdab.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef TESTHELPERS_H
#define TESTHELPERS_H

#include "Core/CharmExceptions.h"

#include <QDebug>
#include <QDir>

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
