/*

class XMLXmlHighlighter(QtGui.QSyntaxXmlHighlighter):
 
    #INIT THE STUFF
    def __init__(self, parent=None):
        super(XMLXmlHighlighter, self).__init__(parent)
 
        keywordFormat = QtGui.QTextCharFormat()
        keywordFormat.setForeground(QtCore.Qt.darkMagenta)
        keywordFormat.setFontWeight(QtGui.QFont.Bold)
 
        keywordPatterns = ["\\b?xml\\b", "/>", ">", "<"]
 
        self.highlightingRules = [(QtCore.QRegExp(pattern), keywordFormat)
                for pattern in keywordPatterns]
 
        xmlElementFormat = QtGui.QTextCharFormat()
        xmlElementFormat.setFontWeight(QtGui.QFont.Bold)
        xmlElementFormat.setForeground(QtCore.Qt.green)
        self.highlightingRules.append((QtCore.QRegExp("\\b[A-Za-z0-9_]+(?=[\s/>])"), xmlElementFormat))
 
        xmlAttributeFormat = QtGui.QTextCharFormat()
        xmlAttributeFormat.setFontItalic(True)
        xmlAttributeFormat.setForeground(QtCore.Qt.blue)
        self.highlightingRules.append((QtCore.QRegExp("\\b[A-Za-z0-9_]+(?=\\=)"), xmlAttributeFormat))
 
        self.valueFormat = QtGui.QTextCharFormat()
        self.valueFormat.setForeground(QtCore.Qt.red)
 
        self.valueStartExpression = QtCore.QRegExp("\"")
        self.valueEndExpression = QtCore.QRegExp("\"(?=[\s></])")
 
        singleLineCommentFormat = QtGui.QTextCharFormat()
        singleLineCommentFormat.setForeground(QtCore.Qt.gray)
        self.highlightingRules.append((QtCore.QRegExp("<!--[^\n]*-->"), singleLineCommentFormat))
 
    #VIRTUAL FUNCTION WE OVERRIDE THAT DOES ALL THE COLLORING
    def highlightBlock(self, text):
 
        #for every pattern
        for pattern, format in self.highlightingRules:
 
            #Create a regular expression from the retrieved pattern
            expression = QtCore.QRegExp(pattern)
 
            #Check what index that expression occurs at with the ENTIRE text
            index = expression.indexIn(text)
 
            #While the index is greater than 0
            while index >= 0:
 
                #Get the length of how long the expression is true, set the format from the start to the length with the text format
                length = expression.matchedLength()
                self.setFormat(index, length, format)
 
                #Set index to where the expression ends in the text
                index = expression.indexIn(text, index + length)
 
        #HANDLE QUOTATION MARKS NOW.. WE WANT TO START WITH " AND END WITH ".. A THIRD " SHOULD NOT CAUSE THE WORDS INBETWEEN SECOND AND THIRD TO BE COLORED
        self.setCurrentBlockState(0)
 
        startIndex = 0
        if self.previousBlockState() != 1:
            startIndex = self.valueStartExpression.indexIn(text)
 
        while startIndex >= 0:
            endIndex = self.valueEndExpression.indexIn(text, startIndex)
 
            if endIndex == -1:
                self.setCurrentBlockState(1)
                commentLength = len(text) - startIndex
            else:
                commentLength = endIndex - startIndex + self.valueEndExpression.matchedLength()
 
            self.setFormat(startIndex, commentLength, self.valueFormat)
 
            startIndex = self.valueStartExpression.indexIn(text, startIndex + commentLength);
*/


#ifndef XMLHIGHLIGHTER_H
#define XMLHIGHLIGHTER_H

#include <QSyntaxHighlighter>

#include <QHash>
#include <QTextCharFormat>

class QTextDocument;

class XmlHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    XmlHighlighter(QTextDocument *parent = 0);

protected:
    void highlightBlock(const QString &text);

private:
    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    QRegExp valueStartExpression;
    QRegExp valueEndExpression;

    QTextCharFormat keywordFormat;
    QTextCharFormat xmlElementFormat;
    QTextCharFormat xmlAttributeFormat;
    QTextCharFormat valueFormat;
    QTextCharFormat singleLineCommentFormat;

     QRegExp commentStartExpression;
     QRegExp commentEndExpression;

     QTextCharFormat classFormat;
     QTextCharFormat multiLineCommentFormat;
     QTextCharFormat quotationFormat;
     QTextCharFormat functionFormat;
};

#endif
