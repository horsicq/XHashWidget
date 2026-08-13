/* Copyright (c) 2020-2026 hors<horsicq@gmail.com>
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

XHashWidget::XHashWidget(QWidget *pParent) : XShortcutsWidget(pParent), ui(new Ui::XHashWidget), m_pDevice(nullptr), m_nOffset(0), m_nSize(0), m_hashData()
{
    ui->setupUi(this);

    XOptions::adjustToolButton(ui->toolButtonReload, XOptions::ICONTYPE_RELOAD);
    XOptions::adjustToolButton(ui->toolButtonSave, XOptions::ICONTYPE_SAVE);

    ui->comboBoxType->setToolTip(tr("Type"));
    ui->comboBoxMethod->setToolTip(tr("Method"));
    ui->comboBoxMapMode->setToolTip(tr("Mode"));
    ui->lineEditOffset->setToolTip(tr("Offset"));
    ui->lineEditSize->setToolTip(tr("Size"));
    ui->lineEditHash->setToolTip(tr("Hash"));
    ui->tableViewRegions->setToolTip(tr("Regions"));
    ui->toolButtonReload->setToolTip(tr("Reload"));
    ui->toolButtonSave->setToolTip(tr("Save"));

    ui->comboBoxType->setAccessibleName(tr("Type"));
    ui->comboBoxMapMode->setAccessibleName(tr("Mode"));
    ui->comboBoxMethod->setAccessibleName(tr("Method"));
    ui->lineEditOffset->setAccessibleName(tr("Offset"));
    ui->lineEditSize->setAccessibleName(tr("Size"));
    ui->lineEditHash->setAccessibleName(tr("Hash"));
    ui->tableViewRegions->setAccessibleName(tr("Regions"));
    ui->toolButtonReload->setAccessibleName(tr("Reload"));
    ui->toolButtonSave->setAccessibleName(tr("Save"));
    ui->lineEditHash->setAccessibleDescription(tr("Calculated hash for the selected range"));
    ui->tableViewRegions->setAccessibleDescription(tr("Hashes for regions inside the selected range"));
    ui->labelStatus->setAccessibleName(tr("Status"));

    ui->lineEditHash->setValidatorMode(XLineEditValidator::MODE_TEXT);

    populateHashMethods();

    m_hashData.mode = XLineEditValidator::MODE_HEX_32;
    clearResults();
    setDataControlsEnabled(false);
    setStatus(tr("No readable data is available for hashing."));
}

XHashWidget::~XHashWidget()
{
    delete ui;
}

void XHashWidget::clearResults()
{
    m_hashData.sHash.clear();
    m_hashData.listMemoryRecords.clear();
    ui->lineEditHash->setValue_String(QString());
    fillRegionsModel();
    ui->toolButtonSave->setEnabled(false);
}

void XHashWidget::invalidateData(const QString &sStatus)
{
    m_pDevice.clear();
    m_nOffset = 0;
    m_nSize = 0;
    m_hashData = {};
    m_hashData.mode = XLineEditValidator::MODE_HEX_32;

    const bool bTypeBlocked = ui->comboBoxType->blockSignals(true);
    const bool bMapModeBlocked = ui->comboBoxMapMode->blockSignals(true);
    ui->comboBoxType->clear();
    ui->comboBoxMapMode->clear();
    ui->comboBoxType->blockSignals(bTypeBlocked);
    ui->comboBoxMapMode->blockSignals(bMapModeBlocked);

    ui->lineEditOffset->clear();
    ui->lineEditSize->clear();
    clearResults();
    setDataControlsEnabled(false);
    setStatus(sStatus);
}

bool XHashWidget::isDataReady() const
{
    QIODevice *pDevice = m_pDevice.data();

    if (!pDevice || !pDevice->isOpen() || !pDevice->isReadable() || pDevice->isSequential() || (m_nOffset < 0) || (m_nSize <= 0)) {
        return false;
    }

    const qint64 nDeviceSize = pDevice->size();

    return (nDeviceSize >= 0) && (m_nOffset <= nDeviceSize) && (m_nSize <= (nDeviceSize - m_nOffset));
}

void XHashWidget::setDataControlsEnabled(bool bState)
{
    const bool bHasType = bState && (ui->comboBoxType->count() > 0);
    const bool bHasMapMode = bState && (ui->comboBoxMapMode->count() > 0);
    const bool bHasMethod = bState && (ui->comboBoxMethod->count() > 0);

    ui->comboBoxType->setEnabled(bHasType);
    ui->comboBoxMapMode->setEnabled(bHasMapMode);
    ui->comboBoxMethod->setEnabled(bHasMethod);
    ui->lineEditOffset->setEnabled(bState);
    ui->lineEditSize->setEnabled(bState);
    ui->lineEditHash->setEnabled(bState);
    ui->tableViewRegions->setEnabled(bState);
    ui->toolButtonReload->setEnabled(bHasType && bHasMapMode && bHasMethod);

    const QAbstractItemModel *pModel = ui->tableViewRegions->model();
    ui->toolButtonSave->setEnabled(bState && !m_hashData.sHash.isEmpty() && pModel && (pModel->rowCount() > 0));
}

void XHashWidget::setStatus(const QString &sStatus)
{
    ui->labelStatus->setText(sStatus);
}

void XHashWidget::populateHashMethods()
{
    const bool bBlocked = ui->comboBoxMethod->blockSignals(true);
    ui->comboBoxMethod->clear();

    QList<XBinary::HASH> listHashMethods = XBinary::getHashMethodsAsList();
    qint32 nNumberOfMethods = listHashMethods.count();

    for (qint32 i = 0; i < nNumberOfMethods; i++) {
        XBinary::HASH hash = listHashMethods.at(i);
        ui->comboBoxMethod->addItem(XBinary::hashIdToString(hash), hash);
    }

    if (nNumberOfMethods > 1) {
        ui->comboBoxMethod->setCurrentIndex(1);
    } else if (nNumberOfMethods == 1) {
        ui->comboBoxMethod->setCurrentIndex(0);
    }

    ui->comboBoxMethod->blockSignals(bBlocked);
}

void XHashWidget::applyTableHeaders(QStandardItemModel *pModel)
{
    pModel->setHeaderData(0, Qt::Horizontal, tr("Name"));
    pModel->setHeaderData(1, Qt::Horizontal, tr("Offset"));
    pModel->setHeaderData(2, Qt::Horizontal, tr("Size"));
    pModel->setHeaderData(3, Qt::Horizontal, tr("Hash"));
}

void XHashWidget::applyColumnWidths()
{
    qint32 nColumnSize = XLineEditHEX::getWidthFromMode(this, m_hashData.mode);

    ui->tableViewRegions->setColumnWidth(0, COLUMN_NAME_WIDTH);
    ui->tableViewRegions->setColumnWidth(1, nColumnSize);
    ui->tableViewRegions->setColumnWidth(2, nColumnSize);
    ui->tableViewRegions->setColumnWidth(3, COLUMN_FIXED_WIDTH);
}

void XHashWidget::fillRegionsModel()
{
    qint32 nNumberOfMemoryRecords = m_hashData.listMemoryRecords.count();
    QStandardItemModel *pModel = new QStandardItemModel(nNumberOfMemoryRecords, 4, ui->tableViewRegions);

    applyTableHeaders(pModel);

    for (qint32 i = 0; i < nNumberOfMemoryRecords; i++) {
        const HashProcess::MEMORY_RECORD &record = m_hashData.listMemoryRecords.at(i);

        QStandardItem *pItemName = new QStandardItem(record.sName);
        pModel->setItem(i, 0, pItemName);

        if (record.nOffset != -1) {
            QStandardItem *pItemOffset = new QStandardItem(XLineEditHEX::getFormatString(m_hashData.mode, record.nOffset));
            pModel->setItem(i, 1, pItemOffset);
        }

        if (record.nSize != -1) {
            QStandardItem *pItemSize = new QStandardItem(XLineEditHEX::getFormatString(m_hashData.mode, record.nSize));
            pModel->setItem(i, 2, pItemSize);
        }

        QStandardItem *pItemHash = new QStandardItem(record.sHash);
        pModel->setItem(i, 3, pItemHash);
    }

    XOptions::setModelTextAlignment(pModel, 0, Qt::AlignLeft | Qt::AlignVCenter);
    XOptions::setModelTextAlignment(pModel, 1, Qt::AlignRight | Qt::AlignVCenter);
    XOptions::setModelTextAlignment(pModel, 2, Qt::AlignRight | Qt::AlignVCenter);
    XOptions::setModelTextAlignment(pModel, 3, Qt::AlignLeft | Qt::AlignVCenter);

    ui->tableViewRegions->setCustomModel(pModel, true);
    ui->tableViewRegions->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
    ui->tableViewRegions->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Interactive);
    ui->tableViewRegions->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Interactive);
    ui->tableViewRegions->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);

    applyColumnWidths();
}

void XHashWidget::setData(QIODevice *pDevice, XBinary::FT fileType, qint64 nOffset, qint64 nSize, bool bAuto)
{
    if (!pDevice || !pDevice->isOpen() || !pDevice->isReadable() || pDevice->isSequential() || (nOffset < 0) || (nSize < -1)) {
        invalidateData(tr("A readable random-access device is required."));
        return;
    }

    const qint64 nDeviceSize = pDevice->size();

    if ((nDeviceSize <= 0) || (nOffset >= nDeviceSize)) {
        invalidateData(tr("The selected range does not contain data."));
        return;
    }

    const qint64 nAvailableSize = nDeviceSize - nOffset;

    if ((nSize == -1) || (nSize > nAvailableSize)) {
        nSize = nAvailableSize;
    }

    if (nSize <= 0) {
        invalidateData(tr("The selected range does not contain data."));
        return;
    }

    m_pDevice = pDevice;
    m_nOffset = nOffset;
    m_nSize = nSize;

    clearResults();

    ui->lineEditOffset->setValue32_64(m_nOffset);
    ui->lineEditSize->setValue32_64(m_nSize);

    const qint64 nOriginalPosition = pDevice->pos();
    SubDevice subDevice(pDevice, m_nOffset, m_nSize);

    if (subDevice.open(QIODevice::ReadOnly)) {
        const XBinary::FT detectedFileType = XFormats::setFileTypeComboBox(fileType, &subDevice, ui->comboBoxType);
        XFormats::getMapModesList(detectedFileType, ui->comboBoxMapMode);
        subDevice.close();
    } else {
        if (nOriginalPosition >= 0) {
            pDevice->seek(nOriginalPosition);
        }

        invalidateData(tr("The selected range cannot be read."));
        return;
    }

    if (nOriginalPosition >= 0) {
        pDevice->seek(nOriginalPosition);
    }

    setDataControlsEnabled(true);

    if (!ui->toolButtonReload->isEnabled()) {
        setStatus(tr("No compatible hashing options are available."));
        return;
    }

    if (bAuto) {
        reload();
    } else {
        setStatus(tr("Ready. Press Reload to calculate hashes."));
    }
}

void XHashWidget::reload()
{
    if (!isDataReady()) {
        invalidateData(tr("A readable random-access device and a non-empty range are required."));
        return;
    }

    if ((ui->comboBoxMethod->currentIndex() < 0) || (ui->comboBoxType->currentIndex() < 0) || (ui->comboBoxMapMode->currentIndex() < 0)) {
        clearResults();
        setDataControlsEnabled(true);
        setStatus(tr("No compatible hashing options are available."));
        return;
    }

    m_hashData.hash = static_cast<XBinary::HASH>(ui->comboBoxMethod->currentData().toInt());
    m_hashData.fileType = static_cast<XBinary::FT>(ui->comboBoxType->currentData().toInt());
    m_hashData.mapMode = static_cast<XBinary::MAPMODE>(ui->comboBoxMapMode->currentData().toInt());
    m_hashData.nOffset = 0;
    m_hashData.nSize = m_nSize;

    QPointer<QIODevice> pDevice = m_pDevice;
    const qint64 nOriginalPosition = pDevice->pos();
    bool bSuccess = false;

    SubDevice subDevice(pDevice.data(), m_nOffset, m_nSize);

    if (!subDevice.open(QIODevice::ReadOnly)) {
        if (pDevice && (nOriginalPosition >= 0)) {
            pDevice->seek(nOriginalPosition);
        }

        invalidateData(tr("The selected range cannot be read."));
        return;
    }

    {
        HashProcess hashProcess;
        XDialogProcess dhp(XOptions::getMainWidget(this), &hashProcess);
        dhp.setGlobal(getShortcuts(), getGlobalOptions());
        hashProcess.setData(&subDevice, &m_hashData, dhp.getPdStruct());
        dhp.start();
        dhp.showDialogDelay();
        bSuccess = dhp.isSuccess();
    }

    subDevice.close();

    if (pDevice && (nOriginalPosition >= 0)) {
        pDevice->seek(nOriginalPosition);
    }

    if (!isDataReady()) {
        invalidateData(tr("The source device or selected range became unavailable."));
        return;
    }

    if (!bSuccess || m_hashData.sHash.isEmpty()) {
        clearResults();
        setDataControlsEnabled(true);
        setStatus(tr("Hash calculation was canceled or failed."));
        return;
    }

    ui->lineEditHash->setValue_String(m_hashData.sHash);

    fillRegionsModel();
    setDataControlsEnabled(true);
    setStatus(tr("Calculated %1 for %2 bytes.").arg(ui->comboBoxMethod->currentText(), QString::number(m_nSize)));
}

void XHashWidget::adjustView()
{
}

void XHashWidget::reloadData(bool bSaveSelection)
{
    Q_UNUSED(bSaveSelection)
    reload();
}

void XHashWidget::on_toolButtonReload_clicked()
{
    reload();
}

void XHashWidget::on_comboBoxType_currentIndexChanged(int nIndex)
{
    Q_UNUSED(nIndex)

    XBinary::FT fileType = (XBinary::FT)(ui->comboBoxType->currentData().toInt());
    XFormats::getMapModesList(fileType, ui->comboBoxMapMode);

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

void XHashWidget::on_toolButtonSave_clicked()
{
    QAbstractItemModel *pModel = ui->tableViewRegions->getProxyModel();

    if (isDataReady() && pModel && (pModel->rowCount() > 0)) {
        XShortcutsWidget::saveTableModel(pModel, XBinary::getResultFileName(m_pDevice.data(), QString("%1.txt").arg(tr("Hash"))));
    }
}

void XHashWidget::on_tableViewRegions_customContextMenuRequested(const QPoint &pos)
{
    qint32 nRow = ui->tableViewRegions->currentIndex().row();

    if (nRow != -1) {
        QMenu contextMenu(this);

        QList<XShortcuts::MENUITEM> listMenuItems;

        getShortcuts()->_addMenuItem_CopyRow(&listMenuItems, ui->tableViewRegions);

        getShortcuts()->adjustContextMenu(&contextMenu, &listMenuItems);

        contextMenu.exec(ui->tableViewRegions->viewport()->mapToGlobal(pos));
    }
}

void XHashWidget::on_comboBoxMapMode_currentIndexChanged(int nIndex)
{
    Q_UNUSED(nIndex)

    reload();
}
