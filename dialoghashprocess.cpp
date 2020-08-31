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
#include "dialoghashprocess.h"
#include "ui_dialoghashprocess.h"

DialogHashProcess::DialogHashProcess(QWidget *pParent, QIODevice *pDevice, HashProcess::DATA *pData) :
    QDialog(pParent),
    ui(new Ui::DialogHashProcess)
{
    ui->setupUi(this);

    bIsStop=false;

    pHashProcess=new HashProcess;
    pThread=new QThread;

    pHashProcess->moveToThread(pThread);

    connect(pThread, SIGNAL(started()), pHashProcess, SLOT(process()));
    connect(pHashProcess, SIGNAL(completed(qint64)), this, SLOT(onCompleted(qint64)));
    connect(pHashProcess, SIGNAL(errorMessage(QString)), this, SLOT(errorMessage(QString)));
    connect(pHashProcess, SIGNAL(progressValueChanged(qint32)), this, SLOT(progressValueChanged(qint32)));
    connect(pHashProcess, SIGNAL(progressValueMinimum(qint32)), this, SLOT(progressValueMinimum(qint32)));
    connect(pHashProcess, SIGNAL(progressValueMaximum(qint32)), this, SLOT(progressValueMaximum(qint32)));

    pHashProcess->setData(pDevice,pData);
    pThread->start();
}

DialogHashProcess::~DialogHashProcess()
{
    pHashProcess->stop();

    pThread->quit();
    pThread->wait();

    delete ui;

    delete pThread;
    delete pHashProcess;
}

void DialogHashProcess::on_pushButtonCancel_clicked()
{
    bIsStop=true;

    pHashProcess->stop();
}

void DialogHashProcess::errorMessage(QString sText)
{
    QMessageBox::critical(this,tr("Error"),sText);
}

void DialogHashProcess::onCompleted(qint64 nElapsed)
{
    Q_UNUSED(nElapsed)

    if(!bIsStop)
    {
        accept();
    }
    else
    {
        reject();
    }
}

void DialogHashProcess::progressValueChanged(qint32 nValue)
{
    ui->progressBar->setValue(nValue);
}

void DialogHashProcess::progressValueMaximum(qint32 nValue)
{
    ui->progressBar->setMaximum(nValue);
}

void DialogHashProcess::progressValueMinimum(qint32 nValue)
{
    ui->progressBar->setMinimum(nValue);
}
