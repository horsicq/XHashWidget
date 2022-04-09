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
#include "xhashwidget.h"
#include "ui_xhashwidget.h"

XHashWidget::XHashWidget(QWidget *pParent) :
    XShortcutsWidget(pParent),
    ui(new Ui::XHashWidget)
{
    ui->setupUi(this);

    g_pDevice=nullptr;
    g_nOffset=0;
    g_nSize=0;
    g_hashData={};

#if QT_VERSION >= QT_VERSION_CHECK(5,3,0)
    const QSignalBlocker blocker(ui->comboBoxMethod);
#else
    const bool bBlocked1=ui->comboBoxMethod->blockSignals(true);
#endif

    QList<XBinary::HASH> listHashMethods=XBinary::getHashMethodsAsList();

    qint32 nNumberOfMethods=listHashMethods.count();

    for(qint32 i=0;i<nNumberOfMethods;i++)
    {
        XBinary::HASH hash=listHashMethods.at(i);
        ui->comboBoxMethod->addItem(XBinary::hashIdToString(hash),hash);
    }

    if(nNumberOfMethods>1)
    {
        ui->comboBoxMethod->setCurrentIndex(1); // MD5 default TODO consts
    }

#if QT_VERSION < QT_VERSION_CHECK(5,3,0)
    ui->comboBoxMethod->blockSignals(bBlocked1);
#endif
}

XHashWidget::~XHashWidget()
{
    delete ui;
}

void XHashWidget::setData(QIODevice *pDevice,XBinary::FT fileType,qint64 nOffset,qint64 nSize,bool bAuto)
{
    this->g_pDevice=pDevice;
    this->g_nOffset=nOffset;
    this->g_nSize=nSize;

    if(this->g_nSize==-1)
    {
        this->g_nSize=(pDevice->size())-(this->g_nOffset);
    }

    ui->lineEditOffset->setValue32_64(this->g_nOffset);
    ui->lineEditSize->setValue32_64(this->g_nSize);

    SubDevice subDevice(pDevice,this->g_nOffset,this->g_nSize);

    if(subDevice.open(QIODevice::ReadOnly))
    {
        XFormats::setFileTypeComboBox(fileType,&subDevice,ui->comboBoxType);

        subDevice.close();
    }

    if(bAuto)
    {
        reload();
    }
}

void XHashWidget::reload()
{
    g_hashData.hash=(XBinary::HASH)ui->comboBoxMethod->currentData().toInt();
    g_hashData.fileType=(XBinary::FT)(ui->comboBoxType->currentData().toInt());
    g_hashData.nOffset=g_nOffset;
    g_hashData.nSize=g_nSize;

    DialogHashProcess dhp(XOptions::getMainWidget(this),g_pDevice,&g_hashData);

    if(dhp.exec()==QDialog::Accepted)
    {
        ui->lineEditHash->setText(g_hashData.sHash);

        ui->tableWidgetRegions->clear();

        qint32 nNumberOfMemoryRecords=g_hashData.listMemoryRecords.count();

        QStringList slHeader;
        slHeader.append(tr("Name"));
        slHeader.append(tr("Offset"));
        slHeader.append(tr("Size"));
        slHeader.append(tr("Hash"));

        ui->tableWidgetRegions->setRowCount(nNumberOfMemoryRecords);
        ui->tableWidgetRegions->setColumnCount(slHeader.size());

        ui->tableWidgetRegions->setHorizontalHeaderLabels(slHeader);
        ui->tableWidgetRegions->horizontalHeader()->setVisible(true);

        for(qint32 i=0;i<nNumberOfMemoryRecords;i++)
        {
            QTableWidgetItem *pItemName=new QTableWidgetItem;

            pItemName->setText(g_hashData.listMemoryRecords.at(i).sName);
            pItemName->setData(Qt::UserRole+0,g_hashData.listMemoryRecords.at(i).nOffset);
            pItemName->setData(Qt::UserRole+1,g_hashData.listMemoryRecords.at(i).nSize);

            ui->tableWidgetRegions->setItem(i,0,pItemName);

            if(g_hashData.listMemoryRecords.at(i).nOffset!=-1)
            {
                QTableWidgetItem *pItemOffset=new QTableWidgetItem;

                pItemOffset->setText(XLineEditHEX::getFormatString(g_hashData.mode,g_hashData.listMemoryRecords.at(i).nOffset));
                pItemOffset->setTextAlignment(Qt::AlignRight);
                ui->tableWidgetRegions->setItem(i,1,pItemOffset);
            }

            if(g_hashData.listMemoryRecords.at(i).nSize!=-1)
            {
                QTableWidgetItem *pItemSize=new QTableWidgetItem;

                pItemSize->setText(XLineEditHEX::getFormatString(g_hashData.mode,g_hashData.listMemoryRecords.at(i).nSize));
                pItemSize->setTextAlignment(Qt::AlignRight);
                ui->tableWidgetRegions->setItem(i,2,pItemSize);
            }

            QTableWidgetItem *pItemHash=new QTableWidgetItem;

            QString sHash=g_hashData.listMemoryRecords.at(i).sHash;

            pItemHash->setText(sHash);
            pItemHash->setTextAlignment(Qt::AlignLeft);
            ui->tableWidgetRegions->setItem(i,3,pItemHash);
        }

        ui->tableWidgetRegions->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Interactive);
        ui->tableWidgetRegions->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Interactive);
        ui->tableWidgetRegions->horizontalHeader()->setSectionResizeMode(2,QHeaderView::Interactive);
        ui->tableWidgetRegions->horizontalHeader()->setSectionResizeMode(3,QHeaderView::Stretch);

        qint32 nColumnSize=XLineEditHEX::getWidthFromMode(this,g_hashData.mode);

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

void XHashWidget::registerShortcuts(bool bState)
{
    Q_UNUSED(bState)
    // TODO !!!
}

void XHashWidget::on_pushButtonSave_clicked()
{
    XShortcutsWidget::saveModel(ui->tableWidgetRegions->model(),XBinary::getResultFileName(g_pDevice,QString("%1.txt").arg(tr("Hash"))));
}
