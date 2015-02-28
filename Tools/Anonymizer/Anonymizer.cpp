/*
  Anonymizer.cpp

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

