#include <QTextBlockFormat>
#include <QTextCharFormat>
#include <QTextCursor>

#include "ParagraphFormatCollection.h"

ParagraphFormat::ParagraphFormat()
{
}

void ParagraphFormat::apply( QTextCursor& cursor )
{
    cursor.setBlockFormat( m_blockFormat );
    cursor.setCharFormat( m_charFormat );
}

QTextBlockFormat& ParagraphFormat::blockFormat()
{
    return m_blockFormat;
}

QTextCharFormat& ParagraphFormat::charFormat()
{
    return m_charFormat;
}

const QString& ParagraphFormat::name() const
{
    return m_name;
}

void ParagraphFormat::setName( const QString& name )
{
    m_name = name;
}

ParagraphFormatCollection::ParagraphFormatCollection()
{
}

ParagraphFormat ParagraphFormatCollection::format( const QString& name ) const
{
    return m_formats[name];
}

void ParagraphFormatCollection::add( ParagraphFormat format )
{
    m_formats[ format.name() ] = format;
}
