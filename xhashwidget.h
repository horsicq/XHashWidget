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
#ifndef XHASHWIDGET_H
#define XHASHWIDGET_H

#include <QFileDialog>
#include <QImageWriter>
#include <QItemSelection>
#include <QPointer>
#include <QStandardItemModel>

#include "hashprocess.h"
#include "xdialogprocess.h"
#include "xformats.h"
#include "xshortcutswidget.h"

namespace Ui {
class XHashWidget;
}

class XHashWidget : public XShortcutsWidget {
    Q_OBJECT

public:
    explicit XHashWidget(QWidget *pParent = nullptr);
    ~XHashWidget() override;

    void setData(QIODevice *pDevice, XBinary::FT fileType, qint64 nOffset, qint64 nSize, bool bAuto = false);
    void reload();
    void adjustView() override;
    void reloadData(bool bSaveSelection) override;

private slots:
    void on_toolButtonReload_clicked();
    void on_comboBoxType_currentIndexChanged(int nIndex);
    void on_comboBoxMethod_currentIndexChanged(int nIndex);
    void on_toolButtonSave_clicked();
    void on_tableViewRegions_customContextMenuRequested(const QPoint &pos);
    void on_comboBoxMapMode_currentIndexChanged(int nIndex);

protected:
    void registerShortcuts(bool bState) override;

private:
    void clearResults();
    void invalidateData(const QString &sStatus);
    bool isDataReady() const;
    void setDataControlsEnabled(bool bState);
    void setStatus(const QString &sStatus);
    void populateHashMethods();
    void fillRegionsModel();
    void applyTableHeaders(QStandardItemModel *pModel);
    void applyColumnWidths();

    static const qint32 COLUMN_NAME_WIDTH = 150;
    static const qint32 COLUMN_FIXED_WIDTH = 80;

private:
    Ui::XHashWidget *ui;
    QPointer<QIODevice> m_pDevice;
    qint64 m_nOffset;
    qint64 m_nSize;
    HashProcess::DATA m_hashData;
};

#endif  // XHASHWIDGET_H
