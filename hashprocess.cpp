// copyright (c) 2020 hors<horsicq@gmail.com>
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
#include "hashprocess.h"

HashProcess::HashProcess(QObject *pParent) : QObject(pParent)
{
    bIsStop=false;

    connect(&binary,SIGNAL(hashProgressValueChanged(qint32)),this,SIGNAL(progressValueChanged(qint32)));
    connect(&binary,SIGNAL(hashProgressMinimumChanged(qint32)),this,SIGNAL(progressValueMinimum(qint32)));
    connect(&binary,SIGNAL(hashProgressMaximumChanged(qint32)),this,SIGNAL(progressValueMaximum(qint32)));
}

void HashProcess::setData(QIODevice *pDevice, DATA *pData)
{
    this->pDevice=pDevice;
    this->pData=pData;
}

void HashProcess::stop()
{
    binary.setHashProcessEnable(false);
    bIsStop=true;
}

void HashProcess::process()
{
    QElapsedTimer scanTimer;
    scanTimer.start();

    bIsStop=false;

    binary.setDevice(this->pDevice);

    pData->sHash=binary.getHash(pData->hash,pData->nOffset,pData->nSize);

    pData->listMemoryRecords.clear();

    XBinary::_MEMORY_MAP memoryMap=XFormats::getMemoryMap(pData->fileType,this->pDevice);

    pData->mode=XLineEditHEX::MODE_32;

    if(memoryMap.mode==XBinary::MODE_16)
    {
        pData->mode=XLineEditHEX::MODE_16;
    }
    else if((memoryMap.mode==XBinary::MODE_16SEG)||(memoryMap.mode==XBinary::MODE_32))
    {
        pData->mode=XLineEditHEX::MODE_32;
    }
    else if(memoryMap.mode==XBinary::MODE_64)
    {
        pData->mode=XLineEditHEX::MODE_64;
    }
    else if(memoryMap.mode==XBinary::MODE_UNKNOWN)
    {
        pData->mode=XLineEditHEX::getModeFromSize(memoryMap.nRawSize);
    }

    int nNumberOfRecords=memoryMap.listRecords.count();

    for(int i=0,j=0;i<nNumberOfRecords;i++)
    {
        bool bIsVirtual=memoryMap.listRecords.at(i).bIsVirtual;

        if(!bIsVirtual)
        {
            MEMORY_RECORD memoryRecord={};

            if((memoryMap.listRecords.at(i).nOffset==0)&&(memoryMap.listRecords.at(i).nSize==pData->nSize))
            {
                memoryRecord.sHash=pData->sHash;
            }
            else
            {
                memoryRecord.sHash=binary.getHash(pData->hash,pData->nOffset+memoryMap.listRecords.at(i).nOffset,memoryMap.listRecords.at(i).nSize);
            }

            memoryRecord.sName=memoryMap.listRecords.at(i).sName;
            memoryRecord.nOffset=memoryMap.listRecords.at(i).nOffset;
            memoryRecord.nSize=memoryMap.listRecords.at(i).nSize;

            pData->listMemoryRecords.append(memoryRecord);

            j++;
        }
    }

    bIsStop=false;

    emit completed(scanTimer.elapsed());
}
