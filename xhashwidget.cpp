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

XHashWidget::XHashWidget(QWidget *pParent) : XShortcutsWidget(pParent), ui(new Ui::XHashWidget)
{
    ui->setupUi(this);

    g_pDevice = nullptr;
    g_nOffset = 0;
    g_nSize = 0;
    g_hashData = {};

    const bool bBlocked1 = ui->comboBoxMethod->blockSignals(true);

    QList<XBinary::HASH> listHashMethods = XBinary::getHashMethodsAsList();

    qint32 nNumberOfMethods = listHashMethods.count();

    for (qint32 i = 0; i < nNumberOfMethods; i++) {
        XBinary::HASH hash = listHashMethods.at(i);
        ui->comboBoxMethod->addItem(XBinary::hashIdToString(hash), hash);
    }

    if (nNumberOfMethods > 1)  // TODO Check
    {
        ui->comboBoxMethod->setCurrentIndex(1);  // MD5 default TODO consts !!!
    }

    ui->comboBoxMethod->blockSignals(bBlocked1);
}

XHashWidget::~XHashWidget()
{
    delete ui;
}

void XHashWidget::setData(QIODevice *pDevice, XBinary::FT fileType, qint64 nOffset, qint64 nSize, bool bAuto)
{
    this->g_pDevice = pDevice;
    this->g_nOffset = nOffset;
    this->g_nSize = nSize;

    if (this->g_nSize == -1) {
        this->g_nSize = (pDevice->size()) - (this->g_nOffset);
    }

    ui->lineEditOffset->setValue32_64(this->g_nOffset);
    ui->lineEditSize->setValue32_64(this->g_nSize);

    SubDevice subDevice(pDevice, this->g_nOffset, this->g_nSize);

    if (subDevice.open(QIODevice::ReadOnly)) {
        XFormats::setFileTypeComboBox(fileType, &subDevice, ui->comboBoxType);

        subDevice.close();
    }

    if (bAuto) {
        reload();
    }
}

void XHashWidget::reload()
{
    // TODO QTableView
    g_hashData.hash = (XBinary::HASH)ui->comboBoxMethod->currentData().toInt();
    g_hashData.fileType = (XBinary::FT)(ui->comboBoxType->currentData().toInt());
    g_hashData.nOffset = g_nOffset;
    g_hashData.nSize = g_nSize;

    DialogHashProcess dhp(XOptions::getMainWidget(this), g_pDevice, &g_hashData);

    dhp.showDialogDelay(1000);

    if (dhp.isSuccess()) {
        ui->lineEditHash->setText(g_hashData.sHash);

        QAbstractItemModel *pOldModel = ui->tableViewRegions->model();

        qint32 nNumberOfMemoryRecords = g_hashData.listMemoryRecords.count();

        QStandardItemModel *pModel = new QStandardItemModel(nNumberOfMemoryRecords, 4);

        pModel->setHeaderData(0, Qt::Horizontal, tr("Name"));
        pModel->setHeaderData(1, Qt::Horizontal, tr("Offset"));
        pModel->setHeaderData(2, Qt::Horizontal, tr("Size"));
        pModel->setHeaderData(3, Qt::Horizontal, tr("Hash"));

        for (qint32 i = 0; i < nNumberOfMemoryRecords; i++) {
            QStandardItem *pItemName = new QStandardItem;

            pItemName->setText(g_hashData.listMemoryRecords.at(i).sName);
            pItemName->setData(g_hashData.listMemoryRecords.at(i).nOffset, Qt::UserRole + 0);
            pItemName->setData(g_hashData.listMemoryRecords.at(i).nSize, Qt::UserRole + 1);

            pModel->setItem(i, 0, pItemName);

            if (g_hashData.listMemoryRecords.at(i).nOffset != -1) {
                QStandardItem *pItemOffset = new QStandardItem;

                pItemOffset->setText(XLineEditHEX::getFormatString(g_hashData.mode, g_hashData.listMemoryRecords.at(i).nOffset));
                pItemOffset->setTextAlignment(Qt::AlignRight);
                pModel->setItem(i, 1, pItemOffset);
            }

            if (g_hashData.listMemoryRecords.at(i).nSize != -1) {
                QStandardItem *pItemSize = new QStandardItem;

                pItemSize->setText(XLineEditHEX::getFormatString(g_hashData.mode, g_hashData.listMemoryRecords.at(i).nSize));
                pItemSize->setTextAlignment(Qt::AlignRight);
                pModel->setItem(i, 2, pItemSize);
            }

            QStandardItem *pItemHash = new QStandardItem;

            QString sHash = g_hashData.listMemoryRecords.at(i).sHash;

            pItemHash->setText(sHash);
            pItemHash->setTextAlignment(Qt::AlignLeft);
            pModel->setItem(i, 3, pItemHash);
        }

        ui->tableViewRegions->setModel(pModel);

        deleteOldAbstractModel(&pOldModel);

        ui->tableViewRegions->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
        ui->tableViewRegions->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Interactive);
        ui->tableViewRegions->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Interactive);
        ui->tableViewRegions->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);

        qint32 nColumnSize = XLineEditHEX::getWidthFromMode(this, g_hashData.mode);

        ui->tableViewRegions->setColumnWidth(0, 150);  // TODO consts
        ui->tableViewRegions->setColumnWidth(1, nColumnSize);
        ui->tableViewRegions->setColumnWidth(2, nColumnSize);
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
    XShortcutsWidget::saveTableModel(ui->tableViewRegions->model(), XBinary::getResultFileName(g_pDevice, QString("%1.txt").arg(tr("Hash"))));
}
