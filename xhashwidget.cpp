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
#include "xhashwidget.h"
#include "ui_xhashwidget.h"

XHashWidget::XHashWidget(QWidget *pParent) :
    QWidget(pParent),
    ui(new Ui::XHashWidget)
{
    ui->setupUi(this);

    const QSignalBlocker blocker(ui->comboBoxMethod);

    QList<XBinary::HASH> listHashMethodes=XBinary::getHashMethodsAsList();

    int nCount=listHashMethodes.count();

    for(int i=0;i<nCount;i++)
    {
        XBinary::HASH hash=listHashMethodes.at(i);
        ui->comboBoxMethod->addItem(XBinary::hashIdToString(hash),hash);
    }
}

XHashWidget::~XHashWidget()
{
    delete ui;
}

void XHashWidget::setData(QIODevice *pDevice,qint64 nOffset,qint64 nSize,bool bAuto)
{
    this->pDevice=pDevice;
    this->nOffset=nOffset;
    this->nSize=nSize;

    if(this->nSize==-1)
    {
        this->nSize=(pDevice->size())-(this->nOffset);
    }

    ui->lineEditOffset->setValue32_64(this->nOffset);
    ui->lineEditSize->setValue32_64(this->nSize);

    if(bAuto)
    {
        const QSignalBlocker blocker(ui->comboBoxType);

        ui->comboBoxType->clear();

        SubDevice subDevice(pDevice,this->nOffset,this->nSize);

        if(subDevice.open(QIODevice::ReadOnly))
        {
            QList<XBinary::FT> listFileTypes=XBinary::_getFileTypeListFromSet(XBinary::getFileTypes(&subDevice));

            int nCount=listFileTypes.count();

            for(int i=0;i<nCount;i++)
            {
                XBinary::FT fileType=listFileTypes.at(i);
                ui->comboBoxType->addItem(XBinary::fileTypeIdToString(fileType),fileType);
            }

            if(nCount)
            {
                ui->comboBoxType->setCurrentIndex(nCount-1);
                reload();
            }

            subDevice.close();
        }
    }
}

void XHashWidget::reload()
{
    XBinary::HASH hash=(XBinary::HASH)ui->comboBoxMethod->currentData().toInt();
    XBinary::FT fileType=(XBinary::FT)(ui->comboBoxType->currentData().toInt());

    XBinary binary(pDevice);

    ui->lineEditHash->setText(binary.getHash(hash,nOffset,nSize));

    SubDevice subDevice(pDevice,nOffset,nSize);

    if(subDevice.open(QIODevice::ReadOnly))
    {
        XBinary::_MEMORY_MAP memoryMap=XFormats::getMemoryMap(fileType,&subDevice);

        XLineEditHEX::MODE mode;

        if(memoryMap.mode==XBinary::MODE_16)
        {
            mode=XLineEditHEX::MODE_16;
        }
        else if((memoryMap.mode==XBinary::MODE_16SEG)||(memoryMap.mode==XBinary::MODE_32))
        {
            mode=XLineEditHEX::MODE_32;
        }
        else if(memoryMap.mode==XBinary::MODE_64)
        {
            mode=XLineEditHEX::MODE_64;
        }
        else if(memoryMap.mode==XBinary::MODE_UNKNOWN)
        {
            mode=XLineEditHEX::getModeFromSize(memoryMap.nRawSize);
        }

        ui->tableWidgetRegions->clear();

        ui->tableWidgetRegions->setRowCount(XBinary::getNumberOfPhysicalRecords(&memoryMap));
        ui->tableWidgetRegions->setColumnCount(4);

        QStringList slHeader;
        slHeader.append(tr("Name"));
        slHeader.append(tr("Offset"));
        slHeader.append(tr("Size"));
        slHeader.append(tr("Hash"));

        ui->tableWidgetRegions->setHorizontalHeaderLabels(slHeader);
        ui->tableWidgetRegions->horizontalHeader()->setVisible(true);

        int nCount=memoryMap.listRecords.count();

        for(int i=0,j=0;i<nCount;i++)
        {
            bool bIsVirtual=memoryMap.listRecords.at(i).bIsVirtual;

            if(!bIsVirtual)
            {
                QTableWidgetItem *itemName=new QTableWidgetItem;

                itemName->setText(memoryMap.listRecords.at(i).sName);
                itemName->setData(Qt::UserRole+0,memoryMap.listRecords.at(i).nOffset);
                itemName->setData(Qt::UserRole+1,memoryMap.listRecords.at(i).nSize);

                ui->tableWidgetRegions->setItem(j,0,itemName);

                QTableWidgetItem *itemOffset=new QTableWidgetItem;

                itemOffset->setText(XLineEditHEX::getFormatString(mode,memoryMap.listRecords.at(i).nOffset));
                itemOffset->setTextAlignment(Qt::AlignRight);
                ui->tableWidgetRegions->setItem(j,1,itemOffset);

                QTableWidgetItem *itemSize=new QTableWidgetItem;

                itemSize->setText(XLineEditHEX::getFormatString(mode,memoryMap.listRecords.at(i).nSize));
                itemSize->setTextAlignment(Qt::AlignRight);
                ui->tableWidgetRegions->setItem(j,2,itemSize);

                QTableWidgetItem *itemHash=new QTableWidgetItem;

                QString sHash=binary.getHash(hash,nOffset+memoryMap.listRecords.at(i).nOffset,memoryMap.listRecords.at(i).nSize);

                itemHash->setText(sHash);
                itemHash->setTextAlignment(Qt::AlignLeft);
                ui->tableWidgetRegions->setItem(j,3,itemHash);

                j++;
            }
        }

        ui->tableWidgetRegions->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Interactive);
        ui->tableWidgetRegions->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Interactive);
        ui->tableWidgetRegions->horizontalHeader()->setSectionResizeMode(2,QHeaderView::Interactive);
        ui->tableWidgetRegions->horizontalHeader()->setSectionResizeMode(3,QHeaderView::Stretch);

        qint32 nColumnSize=XLineEditHEX::getWidthFromMode(this,mode);

        ui->tableWidgetRegions->setColumnWidth(0,150);
        ui->tableWidgetRegions->setColumnWidth(1,nColumnSize);
        ui->tableWidgetRegions->setColumnWidth(2,nColumnSize);

        subDevice.close();
    }
}

void XHashWidget::on_pushButtonReload_clicked()
{
    reload();
}

void XHashWidget::on_comboBoxType_currentIndexChanged(int nIndex)
{
    Q_UNUSED(nIndex)

    reload();
}

void XHashWidget::on_comboBoxMethod_currentIndexChanged(int nIndex)
{
    Q_UNUSED(nIndex)

    reload();
}
