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

XHashWidget::XHashWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::XHashWidget)
{
    ui->setupUi(this);
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

    if(bAuto)
    {

    }
}

void XHashWidget::setSaveDirectory(QString sSaveDirectory)
{
    this->sSaveDirectory=sSaveDirectory;
}

void XHashWidget::reload()
{

}

void XHashWidget::on_pushButtonReload_clicked()
{
    reload();
}

void XHashWidget::updateRegions()
{

}

void XHashWidget::on_comboBoxType_currentIndexChanged(int index)
{
    Q_UNUSED(index)

    updateRegions();
}

void XHashWidget::on_tableWidgetRegions_itemSelectionChanged()
{

}

void XHashWidget::on_pushButtonSaveEntropy_clicked()
{

}

QString XHashWidget::getResultName()
{
    QString sResult;

    // TODO

    return sResult;
}
