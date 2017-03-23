/****************************************************************************
 **
 ** Copyright (C) 2016 The Qt Company Ltd.
 ** Contact: https://www.qt.io/licensing/
 **
 ** This file is part of the examples of the Qt Toolkit.
 **
 ** $QT_BEGIN_LICENSE:BSD$
 ** Commercial License Usage
 ** Licensees holding valid commercial Qt licenses may use this file in
 ** accordance with the commercial license agreement provided with the
 ** Software or, alternatively, in accordance with the terms contained in
 ** a written agreement between you and The Qt Company. For licensing terms
 ** and conditions see https://www.qt.io/terms-conditions. For further
 ** information use the contact form at https://www.qt.io/contact-us.
 **
 ** BSD License Usage
 ** Alternatively, you may use this file under the terms of the BSD license ** as follows:
 **
 ** "Redistribution and use in source and binary forms, with or without
 ** modification, are permitted provided that the following conditions are
 ** met:
 **   * Redistributions of source code must retain the above copyright
 **     notice, this list of conditions and the following disclaimer.
 **   * Redistributions in binary form must reproduce the above copyright
 **     notice, this list of conditions and the following disclaimer in
 **     the documentation and/or other materials provided with the
 **     distribution.
 **   * Neither the name of The Qt Company Ltd nor the names of its
 **     contributors may be used to endorse or promote products derived
 **     from this software without specific prior written permission.
 **
 **
 ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 ** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 ** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 ** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 ** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 ** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 ** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 ** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 ** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 ** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 ** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
 **
 ** $QT_END_LICENSE$
 **
 ****************************************************************************/
#include "CodeEditor.h"

CodeEditor::CodeEditor(QFile* sourceFile, bool open, QWidget *parent) : QPlainTextEdit(parent)
{
    lineNumberArea = new LineNumberArea(this);

    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();

    this->setFrameStyle(QFrame::NoFrame);
    this->setUndoRedoEnabled(true);
    
    highlighter = new Highlighter(this->document());
    normalFormat.setUnderlineStyle(QTextCharFormat::NoUnderline);
    errorFormat.setUnderlineColor(QColor(Qt::red));
    errorFormat.setUnderlineStyle( QTextCharFormat::WaveUnderline );
    warningFormat.setUnderlineColor(QColor(Qt::blue));
    warningFormat.setUnderlineStyle( QTextCharFormat::WaveUnderline );

    this->sourceFile = sourceFile;
    if(open == true){
        read();
    }

}

CodeEditor::~CodeEditor(){
    delete sourceFile;
    delete highlighter;
}

//read data on editor to file
void CodeEditor::read()
{
   if (!sourceFile->open(QIODevice::ReadOnly | QIODevice::Text)){
        exit(-1);
    }
    while (!sourceFile->atEnd()) {
        QByteArray line = sourceFile->readLine();
        this->insertPlainText(line);
    }
    sourceFile->close();
}

void CodeEditor::write()
{
    if (!sourceFile->open(QIODevice::WriteOnly | QIODevice::Text))
        exit(-1);
    QTextStream out(this->sourceFile);
    out << toPlainText();
    sourceFile->flush();
    sourceFile->close();
}

void CodeEditor::writeAs(QString fileName)
{
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    QTextStream out(&file);
    out << toPlainText();
    file.flush(); 
    file.close();
}

int CodeEditor::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 13 + fontMetrics().width(QLatin1Char('0')) * digits;

    return space;
}



void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}



void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}



void CodeEditor::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}



void CodeEditor::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(Qt::lightGray).lighter(120);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}



void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), Qt::lightGray);


    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1) + ".";
            painter.setPen(Qt::black);
            painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(),
                    Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}
void CodeEditor::mark(QList<Problem> problems){
    QTextDocument *doc = this->document();
    QTextCursor cur(doc);
    cur.movePosition(QTextCursor::Start);
    int currLine = 0;
    for(Problem problem : problems){
        cur.movePosition(QTextCursor::StartOfLine);
        int currWord = 0;
        while(problem.lineNumber > currLine){
            cur.movePosition(QTextCursor::Down);
            currLine++;
        }
        while(problem.wordNumber > currWord){
            cur.movePosition(QTextCursor::NextWord);
            currWord++;
        }
        cur.movePosition(QTextCursor::EndOfWord,QTextCursor::KeepAnchor);
        if(problem.type == ERROR){
            cur.mergeCharFormat(errorFormat);
        }
        else if(problem.type == WARNING){
            cur.mergeCharFormat(warningFormat);
        }
    }
}
void CodeEditor::clearMarks(){
    QTextDocument *doc = this->document();
    QTextCursor cur(doc);
    cur.movePosition(QTextCursor::Start);
    cur.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);     
    cur.mergeCharFormat(normalFormat);
}