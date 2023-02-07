/* Copyright (c) 2020-2023 hors<horsicq@gmail.com>
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
#include "dialoghash.h"

#include "ui_dialoghash.h"

DialogHash::DialogHash(QWidget *pParent) : QDialog(pParent), ui(new Ui::DialogHash)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Window);  // TODO options
}

DialogHash::~DialogHash()
{
    delete ui;
}

void DialogHash::setData(QIODevice *pDevice, XBinary::FT fileType, qint64 nOffset, qint64 nSize)
{
    ui->widgetHash->setData(pDevice, fileType, nOffset, nSize, true);
}

void DialogHash::setGlobal(XShortcuts *pShortcuts, XOptions *pXOptions)
{
    ui->widgetHash->setGlobal(pShortcuts, pXOptions);
}

void DialogHash::on_pushButtonClose_clicked()
{
    this->close();
}
