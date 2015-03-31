/*
  XmlSerialization.h

  This file is part of Charm, a task-based time tracking application.

  Copyright (C) 2008-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

  Author: Mirko Boehm <mirko.boehm@kdab.com>
  Author: Frank Osterfeld <frank.osterfeld@kdab.com>

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

#ifndef CHARM_XMLSERIALIZATION_H
#define CHARM_XMLSERIALIZATION_H

#include <QDomDocument>
#include <QHash>
#include <QString>

#include "Task.h"

namespace XmlSerialization {

    QDomDocument createXmlTemplate(const QString &docClass );

    QDomElement reportElement( const QDomDocument& doc );

    QDomElement metadataElement( const QDomDocument& doc );

    QDateTime creationTime( const QDomElement& metaDataElement );
    QString userName( const QDomElement& metaDataElement );
}

class TaskExport {
public:
    // the only method that deals with writing:
    static void writeTo( const QString& filename, const TaskList& tasks );

    void readFrom( const QString& filename );
    void readFrom( QIODevice* device );

    TaskList tasks() const;
    QString metadata( const QString& key ) const;
    static QString reportType();

    QDateTime exportTime() const;

private:

    TaskList m_tasks;
    QHash<QString,QString> m_metadata;
    QDateTime m_exportTime;
};

#endif
