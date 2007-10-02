#ifndef PARAGRAPHFORMATCOLLECTION_H
#define PARAGRAPHFORMATCOLLECTION_H

#include <QMap>
#include <QString>
#include <QTextBlockFormat>
#include <QTextCharFormat>

class QTextCursor;

class ParagraphFormat {
public:
    ParagraphFormat();

    void apply( QTextCursor& );

    QTextBlockFormat& blockFormat();

    QTextCharFormat& charFormat();

    const QString& name() const;

    void setName( const QString& );

protected:
    QString m_name;
    QTextBlockFormat m_blockFormat;
    QTextCharFormat m_charFormat;
};

class ParagraphFormatCollection
{
public:
    ParagraphFormatCollection();

    ParagraphFormat format( const QString& name ) const;

    void add ( ParagraphFormat );
private:
    QMap<QString, ParagraphFormat> m_formats;
};

#endif
