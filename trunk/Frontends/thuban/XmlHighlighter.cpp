// code cut-pasted from syntaxhighlighter example in Nokia website
// Alberto Tonda, 2012

#include <QtGui>
#include "XmlHighlighter.h"

XmlHighlighter::XmlHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    keywordFormat.setForeground(Qt::darkMagenta);
    keywordFormat.setFontWeight(QFont::Bold);
    QStringList keywordPatterns;
    
    keywordPatterns << "\\b?xml\\b" << "/>" << ">" << "<" << "\\?>" << "<\\?";

    foreach (const QString &pattern, keywordPatterns) 
    {
        rule.pattern = QRegExp(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    // xml element syntax highlighting
    xmlElementFormat.setFontWeight(QFont::Bold);
    xmlElementFormat.setForeground(Qt::green);
    rule.pattern = QRegExp("\\b[A-Za-z0-9_\\-:]+(?=[\\s/>])");
    rule.format = xmlElementFormat;
    highlightingRules.append(rule);

    // xml attribute syntax highlighting
    xmlAttributeFormat.setFontItalic(true);
    xmlAttributeFormat.setForeground(Qt::blue);
    rule.pattern = QRegExp("\\b[A-Za-z0-9_\\-:]+(?=\\=)");
    rule.format = xmlAttributeFormat;
    highlightingRules.append(rule);

    valueFormat.setForeground(Qt::red);

    valueStartExpression = QRegExp("\"");
    valueEndExpression = QRegExp("\"(?=[\\s></\?])"); // modified here

    singleLineCommentFormat.setForeground(Qt::gray);
    rule.pattern = QRegExp("<!--[^\\n]*-->");
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);
}

void XmlHighlighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, highlightingRules) 
    {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) 
	{
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }
    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = valueStartExpression.indexIn(text);

    while (startIndex >= 0) 
    {
        int endIndex = valueEndExpression.indexIn(text, startIndex);
        int valueLength;
        if (endIndex == -1) 
	{
            setCurrentBlockState(1);
            valueLength = text.length() - startIndex;
        } 
	else 
	{
            valueLength = endIndex - startIndex
                            + valueEndExpression.matchedLength();
        }
        setFormat(startIndex, valueLength, valueFormat);
        startIndex = valueStartExpression.indexIn(text, startIndex + valueLength);
    }
}
