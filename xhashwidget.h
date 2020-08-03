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
#ifndef XHASHWIDGET_H
#define XHASHWIDGET_H

#include <QWidget>
#include <QStandardItemModel>
#include <QItemSelection>
#include <QImageWriter>
#include "xformats.h"

namespace Ui {
class XHashWidget;
}

class XHashWidget : public QWidget
{
    Q_OBJECT

public:
    explicit XHashWidget(QWidget *pParent=nullptr);
    ~XHashWidget();
    void setData(QIODevice *pDevice, qint64 nOffset, qint64 nSize, bool bAuto=false);
    void reload();

private slots:
    void on_pushButtonReload_clicked();
    void on_comboBoxType_currentIndexChanged(int nIndex);
    void on_comboBoxMethod_currentIndexChanged(int nIndex);

private:
    Ui::XHashWidget *ui;
    QIODevice *pDevice;
    qint64 nOffset;
    qint64 nSize;
};

#endif // XHASHWIDGET_H
