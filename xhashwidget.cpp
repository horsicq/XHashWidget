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
#include "xhashwidget.h"
#include "ui_xhashwidget.h"

XHashWidget::XHashWidget(QWidget *pParent) :
    QWidget(pParent),
    ui(new Ui::XHashWidget)
{
    ui->setupUi(this);

    this->g_pParent=pParent;

    hashData={};

    const QSignalBlocker blocker(ui->comboBoxMethod);

    QList<XBinary::HASH> listHashMethods=XBinary::getHashMethodsAsList();

    int nNumberOfMethods=listHashMethods.count();

    for(int i=0;i<nNumberOfMethods;i++)
    {
        XBinary::HASH hash=listHashMethods.at(i);
        ui->comboBoxMethod->addItem(XBinary::hashIdToString(hash),hash);
    }
}

XHashWidget::~XHashWidget()
{
    delete ui;
}

void XHashWidget::setData(QIODevice *pDevice, qint64 nOffset, qint64 nSize, bool bAuto, QWidget *pParent)
{
    this->g_pDevice=pDevice;
    this->nOffset=nOffset;
    this->nSize=nSize;
    this->g_pParent=pParent;

    if(this->nSize==-1)
    {
        this->nSize=(pDevice->size())-(this->nOffset);
    }

    ui->lineEditOffset->setValue32_64(this->nOffset);
    ui->lineEditSize->setValue32_64(this->nSize);

    if(bAuto)
    {
        SubDevice subDevice(pDevice,this->nOffset,this->nSize);

        if(subDevice.open(QIODevice::ReadOnly))
        {
            QList<XBinary::FT> listFileTypes=XBinary::_getFileTypeListFromSet(XBinary::getFileTypes(&subDevice,true));

            XFormats::setFileTypeComboBox(ui->comboBoxType,&listFileTypes,XBinary::FT_UNKNOWN);

            reload();

            subDevice.close();
        }
    }
}

void XHashWidget::reload()
{
    hashData.hash=(XBinary::HASH)ui->comboBoxMethod->currentData().toInt();
    hashData.fileType=(XBinary::FT)(ui->comboBoxType->currentData().toInt());
    hashData.nOffset=nOffset;
    hashData.nSize=nSize;

    DialogHashProcess dhp(g_pParent,g_pDevice,&hashData);

    if(dhp.exec()==QDialog::Accepted)
    {
        ui->lineEditHash->setText(hashData.sHash);

        ui->tableWidgetRegions->clear();

        int nNumberOfMemoryRecords=hashData.listMemoryRecords.count();

        QStringList slHeader;
        slHeader.append(tr("Name"));
        slHeader.append(tr("Offset"));
        slHeader.append(tr("Size"));
        slHeader.append(tr("Hash"));

        ui->tableWidgetRegions->setRowCount(nNumberOfMemoryRecords);
        ui->tableWidgetRegions->setColumnCount(slHeader.size());

        ui->tableWidgetRegions->setHorizontalHeaderLabels(slHeader);
        ui->tableWidgetRegions->horizontalHeader()->setVisible(true);

        for(int i=0;i<nNumberOfMemoryRecords;i++)
        {
            QTableWidgetItem *pItemName=new QTableWidgetItem;

            pItemName->setText(hashData.listMemoryRecords.at(i).sName);
            pItemName->setData(Qt::UserRole+0,hashData.listMemoryRecords.at(i).nOffset);
            pItemName->setData(Qt::UserRole+1,hashData.listMemoryRecords.at(i).nSize);

            ui->tableWidgetRegions->setItem(i,0,pItemName);

            QTableWidgetItem *pItemOffset=new QTableWidgetItem;

            pItemOffset->setText(XLineEditHEX::getFormatString(hashData.mode,hashData.listMemoryRecords.at(i).nOffset));
            pItemOffset->setTextAlignment(Qt::AlignRight);
            ui->tableWidgetRegions->setItem(i,1,pItemOffset);

            QTableWidgetItem *pItemSize=new QTableWidgetItem;

            pItemSize->setText(XLineEditHEX::getFormatString(hashData.mode,hashData.listMemoryRecords.at(i).nSize));
            pItemSize->setTextAlignment(Qt::AlignRight);
            ui->tableWidgetRegions->setItem(i,2,pItemSize);

            QTableWidgetItem *pItemHash=new QTableWidgetItem;

            QString sHash=hashData.listMemoryRecords.at(i).sHash;

            pItemHash->setText(sHash);
            pItemHash->setTextAlignment(Qt::AlignLeft);
            ui->tableWidgetRegions->setItem(i,3,pItemHash);
        }

        ui->tableWidgetRegions->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Interactive);
        ui->tableWidgetRegions->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Interactive);
        ui->tableWidgetRegions->horizontalHeader()->setSectionResizeMode(2,QHeaderView::Interactive);
        ui->tableWidgetRegions->horizontalHeader()->setSectionResizeMode(3,QHeaderView::Stretch);

        qint32 nColumnSize=XLineEditHEX::getWidthFromMode(this,hashData.mode);

        ui->tableWidgetRegions->setColumnWidth(0,150);
        ui->tableWidgetRegions->setColumnWidth(1,nColumnSize);
        ui->tableWidgetRegions->setColumnWidth(2,nColumnSize);
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
