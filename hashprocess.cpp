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
#include "hashprocess.h"

HashProcess::HashProcess(QObject *pParent) : XThreadObject(pParent)
{
    m_pDevice = nullptr;
    m_pData = nullptr;
    m_pPdStruct = nullptr;
}

void HashProcess::setData(QIODevice *pDevice, DATA *pData, XBinary::PDSTRUCT *pPdStruct)
{
    this->m_pDevice = pDevice;
    this->m_pData = pData;
    this->m_pPdStruct = pPdStruct;
}

void HashProcess::process()
{
    // TODO the second ProgressBar

    qint32 _nFreeIndex = XBinary::getFreeIndex(m_pPdStruct);
    XBinary::setPdStructInit(m_pPdStruct, _nFreeIndex, 0);  // TODO Total / Check

    XBinary binary(this->m_pDevice);

    connect(&binary, SIGNAL(errorMessage(QString)), this, SIGNAL(errorMessage(QString)));

    m_pData->sHash = binary.getHash(m_pData->hash, m_pData->nOffset, m_pData->nSize, this->m_pPdStruct);

    m_pData->listMemoryRecords.clear();

    XBinary::_MEMORY_MAP memoryMap = XFormats::getMemoryMap(m_pData->fileType, m_pData->mapMode, this->m_pDevice);

    m_pData->mode = XLineEditValidator::MODE_HEX_32;

    XBinary::MODE _mode = XBinary::getWidthModeFromMemoryMap(&memoryMap);

    // mb TODO a function !!! TODO move to Widget Check
    if (_mode == XBinary::MODE_8) m_pData->mode = XLineEditValidator::MODE_HEX_8;
    else if (_mode == XBinary::MODE_16) m_pData->mode = XLineEditValidator::MODE_HEX_16;
    else if (_mode == XBinary::MODE_32) m_pData->mode = XLineEditValidator::MODE_HEX_32;
    else if (_mode == XBinary::MODE_64) m_pData->mode = XLineEditValidator::MODE_HEX_64;

    qint32 nNumberOfRecords = memoryMap.listRecords.count();

    for (qint32 i = 0, j = 0; (i < nNumberOfRecords) && XBinary::isPdStructNotCanceled(m_pPdStruct); i++) {
        bool bIsVirtual = memoryMap.listRecords.at(i).bIsVirtual;

        if (!bIsVirtual) {
            MEMORY_RECORD memoryRecord = {};

            if ((memoryMap.listRecords.at(i).nOffset == 0) && (memoryMap.listRecords.at(i).nSize == m_pData->nSize)) {
                memoryRecord.sHash = m_pData->sHash;
            } else {
                memoryRecord.sHash =
                    binary.getHash(m_pData->hash, m_pData->nOffset + memoryMap.listRecords.at(i).nOffset, memoryMap.listRecords.at(i).nSize, this->m_pPdStruct);
            }

            memoryRecord.sName = memoryMap.listRecords.at(i).sName;
            memoryRecord.nOffset = memoryMap.listRecords.at(i).nOffset;
            memoryRecord.nSize = memoryMap.listRecords.at(i).nSize;

            m_pData->listMemoryRecords.append(memoryRecord);

            j++;
        }
    }

    if (XBinary::checkFileType(XBinary::FT_PE, m_pData->fileType)) {
        XPE pe(this->m_pDevice);

        if (pe.isValid(m_pPdStruct)) {
            QList<XPE::IMPORT_RECORD> listImportRecords = pe.getImportRecords(&memoryMap);

            {
                MEMORY_RECORD memoryRecord = {};

                memoryRecord.sName = QString("%1 32(CRC)").arg(tr("Import"));
                memoryRecord.nOffset = -1;
                memoryRecord.nSize = -1;
                memoryRecord.sHash = XBinary::valueToHex(pe.getImportHash32(&listImportRecords, m_pPdStruct));

                m_pData->listMemoryRecords.append(memoryRecord);
            }

            {
                MEMORY_RECORD memoryRecord = {};

                memoryRecord.sName = QString("%1 64(CRC)").arg(tr("Import"));
                memoryRecord.nOffset = -1;
                memoryRecord.nSize = -1;
                memoryRecord.sHash = XBinary::valueToHex(pe.getImportHash64(&listImportRecords, m_pPdStruct));

                m_pData->listMemoryRecords.append(memoryRecord);
            }

            QList<XPE::IMPORT_HEADER> listImports = pe.getImports(&memoryMap);
            QList<quint32> listHashes = pe.getImportPositionHashes(&listImports);

            qint32 nNumberOfImports = listImports.count();

            for (qint32 i = 0; (i < nNumberOfImports) && XBinary::isPdStructNotCanceled(m_pPdStruct); i++) {
                MEMORY_RECORD memoryRecord = {};

                memoryRecord.sName = QString("%1(%2)(CRC)['%3']").arg(tr("Import"), QString::number(i), listImports.at(i).sName);
                memoryRecord.nOffset = -1;
                memoryRecord.nSize = -1;
                memoryRecord.sHash = XBinary::valueToHex(listHashes.at(i));

                m_pData->listMemoryRecords.append(memoryRecord);
            }
        }
    }

    XBinary::setPdStructFinished(m_pPdStruct, _nFreeIndex);
}
