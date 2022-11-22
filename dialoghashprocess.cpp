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
#include "dialoghashprocess.h"

DialogHashProcess::DialogHashProcess(QWidget *pParent) : XDialogProcess(pParent)
{
    g_pHashProcess = new HashProcess;
    g_pThread = new QThread;

    g_pHashProcess->moveToThread(g_pThread);

    connect(g_pThread, SIGNAL(started()), g_pHashProcess, SLOT(process()));
    connect(g_pHashProcess, SIGNAL(completed(qint64)), this, SLOT(onCompleted(qint64)));
    connect(g_pHashProcess, SIGNAL(errorMessage(QString)), this, SLOT(errorMessage(QString)));
}

DialogHashProcess::DialogHashProcess(QWidget *pParent, QIODevice *pDevice, HashProcess::DATA *pData) : DialogHashProcess(pParent)
{
    setData(pDevice, pData);
}

DialogHashProcess::~DialogHashProcess()
{
    stop();
    waitForFinished();

    g_pThread->quit();
    g_pThread->wait();

    delete g_pThread;
    delete g_pHashProcess;
}

void DialogHashProcess::setData(QIODevice *pDevice, HashProcess::DATA *pData)
{
    g_pHashProcess->setData(pDevice, pData, getPdStruct());
    g_pThread->start();
}
