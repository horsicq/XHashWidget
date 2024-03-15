/* Copyright (c) 2020-2024 hors<horsicq@gmail.com>
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
#include <QStandardItemModel>

#include "dialoghashprocess.h"
#include "xformats.h"
#include "xshortcutswidget.h"

namespace Ui {
class XHashWidget;
}

class XHashWidget : public XShortcutsWidget {
    Q_OBJECT

public:
    explicit XHashWidget(QWidget *pParent = nullptr);
    ~XHashWidget();

    void setData(QIODevice *pDevice, XBinary::FT fileType, qint64 nOffset, qint64 nSize, bool bAuto = false);
    void reload();

private slots:
    void on_pushButtonReload_clicked();
    void on_comboBoxType_currentIndexChanged(int nIndex);
    void on_comboBoxMethod_currentIndexChanged(int nIndex);
    void on_pushButtonSave_clicked();
    void on_tableViewRegions_customContextMenuRequested(const QPoint &pos);
    void on_comboBoxMapMode_currentIndexChanged(int nIndex);

protected:
    virtual void registerShortcuts(bool bState);

private:
    Ui::XHashWidget *ui;
    QIODevice *g_pDevice;
    qint64 g_nOffset;
    qint64 g_nSize;
    HashProcess::DATA g_hashData;
};

#endif  // XHASHWIDGET_H
