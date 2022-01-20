/* Copyright (c) 2020-2022 hors<horsicq@gmail.com>
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
#include "hashprocess.h"

HashProcess::HashProcess(QObject *pParent) : QObject(pParent)
{
    g_bIsStop=false;

    connect(&g_binary,SIGNAL(errorMessage(QString)),this,SIGNAL(errorMessage(QString)));
    connect(&g_binary,SIGNAL(hashProgressValueChanged(qint32)),this,SIGNAL(progressValueChanged(qint32)));
    connect(&g_binary,SIGNAL(hashProgressValueChanged(qint32)),this,SIGNAL(progressValueChanged(qint32)));
    connect(&g_binary,SIGNAL(hashProgressMinimumChanged(qint32)),this,SIGNAL(progressValueMinimum(qint32)));
    connect(&g_binary,SIGNAL(hashProgressMaximumChanged(qint32)),this,SIGNAL(progressValueMaximum(qint32)));
}

void HashProcess::setData(QIODevice *pDevice, DATA *pData)
{
    this->g_pDevice=pDevice;
    this->g_pData=pData;
}

void HashProcess::stop()
{
    g_binary.setHashProcessEnable(false);
    g_bIsStop=true;
}

void HashProcess::process()
{
    QElapsedTimer scanTimer;
    scanTimer.start();

    g_bIsStop=false;

    g_binary.setDevice(this->g_pDevice);

    g_pData->sHash=g_binary.getHash(g_pData->hash,g_pData->nOffset,g_pData->nSize);

    g_pData->listMemoryRecords.clear();

    XBinary::_MEMORY_MAP memoryMap=XFormats::getMemoryMap(g_pData->fileType,this->g_pDevice);

    g_pData->mode=XLineEditHEX::MODE_32;

    XBinary::MODE _mode=XBinary::getWidthModeFromMemoryMap(&memoryMap);

    // mb TODO a function !!!
    if      (_mode==XBinary::MODE_8)    g_pData->mode=XLineEditHEX::MODE_8;
    else if (_mode==XBinary::MODE_16)   g_pData->mode=XLineEditHEX::MODE_16;
    else if (_mode==XBinary::MODE_32)   g_pData->mode=XLineEditHEX::MODE_32;
    else if (_mode==XBinary::MODE_64)   g_pData->mode=XLineEditHEX::MODE_64;

    qint32 nNumberOfRecords=memoryMap.listRecords.count();

    for(qint32 i=0,j=0;i<nNumberOfRecords;i++)
    {
        bool bIsVirtual=memoryMap.listRecords.at(i).bIsVirtual;

        if(!bIsVirtual)
        {
            MEMORY_RECORD memoryRecord={};

            if((memoryMap.listRecords.at(i).nOffset==0)&&(memoryMap.listRecords.at(i).nSize==g_pData->nSize))
            {
                memoryRecord.sHash=g_pData->sHash;
            }
            else
            {
                memoryRecord.sHash=g_binary.getHash(g_pData->hash,g_pData->nOffset+memoryMap.listRecords.at(i).nOffset,memoryMap.listRecords.at(i).nSize);
            }

            memoryRecord.sName=memoryMap.listRecords.at(i).sName;
            memoryRecord.nOffset=memoryMap.listRecords.at(i).nOffset;
            memoryRecord.nSize=memoryMap.listRecords.at(i).nSize;

            g_pData->listMemoryRecords.append(memoryRecord);

            j++;
        }
    }

    if(XBinary::checkFileType(XBinary::FT_PE,g_pData->fileType))
    {
        XPE pe(this->g_pDevice);

        if(pe.isValid())
        {
            QList<XPE::IMPORT_RECORD> listImportRecords=pe.getImportRecords(&memoryMap);

            {
                MEMORY_RECORD memoryRecord={};

                memoryRecord.sName=QString("%1 32(CRC)").arg(tr("Import"));
                memoryRecord.nOffset=-1;
                memoryRecord.nSize=-1;
                memoryRecord.sHash=XBinary::valueToHex(pe.getImportHash32(&listImportRecords));

                g_pData->listMemoryRecords.append(memoryRecord);
            }

            {
                MEMORY_RECORD memoryRecord={};

                memoryRecord.sName=QString("%1 64(CRC)").arg(tr("Import"));
                memoryRecord.nOffset=-1;
                memoryRecord.nSize=-1;
                memoryRecord.sHash=XBinary::valueToHex(pe.getImportHash64(&listImportRecords));

                g_pData->listMemoryRecords.append(memoryRecord);
            }

            QList<XPE::IMPORT_HEADER> listImports=pe.getImports(&memoryMap);
            QList<quint32> listHashes=pe.getImportPositionHashes(&listImports);

            qint32 nNumberOfImports=listImports.count();

            for(qint32 i=0;i<nNumberOfImports;i++)
            {
                MEMORY_RECORD memoryRecord={};

                memoryRecord.sName=QString("%1(%2)(CRC)['%3']").arg(tr("Import"),QString::number(i),listImports.at(i).sName);
                memoryRecord.nOffset=-1;
                memoryRecord.nSize=-1;
                memoryRecord.sHash=XBinary::valueToHex(listHashes.at(i));

                g_pData->listMemoryRecords.append(memoryRecord);
            }
        }
    }

    g_bIsStop=false;

    emit completed(scanTimer.elapsed());
}
