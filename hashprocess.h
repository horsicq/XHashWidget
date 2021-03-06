// copyright (c) 2020-2021 hors<horsicq@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
#ifndef HASHPROCESS_H
#define HASHPROCESS_H

#include "xlineedithex.h"
#include "xformats.h"

class HashProcess : public QObject
{
    Q_OBJECT

public:
    static const int N_MAX_GRAPH=100;

    struct MEMORY_RECORD
    {
        QString sName;
        qint64 nOffset;
        qint64 nSize;
        QString sHash;
    };

    struct DATA
    {
        qint64 nOffset;
        qint64 nSize;
        QString sHash;
        XBinary::HASH hash;
        XBinary::FT fileType;
        XLineEditHEX::MODE mode;
        QList<MEMORY_RECORD> listMemoryRecords;
    };

    explicit HashProcess(QObject *pParent=nullptr);
    void setData(QIODevice *pDevice,DATA *pData);

signals:
    void errorMessage(QString sText);
    void completed(qint64 nElapsed);
    void progressValueChanged(qint32 nValue);
    void progressValueMinimum(qint32 nValue);
    void progressValueMaximum(qint32 nValue);

public slots:
    void stop();
    void process();

private:
    QIODevice *g_pDevice;
    DATA *g_pData;
    bool g_bIsStop;
    XBinary g_binary;
};

#endif // HASHPROCESS_H
