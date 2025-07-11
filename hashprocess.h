/* Copyright (c) 2020-2025 hors<horsicq@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef HASHPROCESS_H
#define HASHPROCESS_H

#include "xformats.h"
#include "xlineedithex.h"
#include "xthreadobject.h"

class HashProcess : public XThreadObject {
    Q_OBJECT

public:
    struct MEMORY_RECORD {
        QString sName;
        qint64 nOffset;
        qint64 nSize;
        QString sHash;
    };

    struct DATA {
        qint64 nOffset;
        qint64 nSize;
        QString sHash;
        XBinary::HASH hash;
        XBinary::FT fileType;
        XBinary::MAPMODE mapMode;
        XLineEditValidator::MODE mode;
        QList<MEMORY_RECORD> listMemoryRecords;
    };

    explicit HashProcess(QObject *pParent = nullptr);

    void setData(QIODevice *pDevice, DATA *pData, XBinary::PDSTRUCT *pPdStruct);
    void process();

private:
    QIODevice *g_pDevice;
    DATA *g_pData;
    XBinary::PDSTRUCT *g_pPdStruct;
};

#endif  // HASHPROCESS_H
