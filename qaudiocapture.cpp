#include "qaudiocapture.h"
#include <QAudioFormat>
#include <QDebug>
#include "zlistaudioinputdialog.h"
#include <QMessageBox>
#include <QDateTime>
#include <zaudiofmtdialog.h>
#include <zcfgdialog.h>
#include <QDesktopServices>
#include <QApplication>
struct WAVHEADER
{
    // RIFF 头
    char RiffName[4];
    unsigned long nRiffLength;

    // 数据类型标识符
    char WavName[4];

    // 格式块中的块头
    char FmtName[4];
    unsigned long nFmtLength;

    // 格式块中的块数据
    unsigned short nAudioFormat;
    unsigned short nChannleNumber;
    unsigned long nSampleRate;
    unsigned long nBytesPerSecond;
    unsigned short nBytesPerSample;
    unsigned short nBitsPerSample;

    // 数据块中的块头
    char    DATANAME[4];
    unsigned long   nDataLength;
};
ZBuffer::ZBuffer(QAudioCapture *cap):m_cap(cap)
{
    this->m_fileNameIndex=0;
    this->m_outFile=new QFile;
    this->m_bNewFile=true;
    this->m_nWrBytes=0;
}
ZBuffer::~ZBuffer()
{
    delete this->m_outFile;
}
qint64 ZBuffer::writeData(const char *data, qint64 len)
{
    //dump data to file.
    //if we need to generate file name.
    if(this->m_bNewFile)
    {
        QString outFileName;
        outFileName=QString("QCAP-%1.WAV").arg(QString::number(this->m_fileNameIndex++,10));
        this->m_outFile->setFileName(QApplication::applicationDirPath()+"/CapWav/"+outFileName);
        if(!this->m_outFile->open(QIODevice::WriteOnly|QIODevice::Truncate))
        {
            emit this->ZSigMsg(tr("file:%1,err:%2.").arg(outFileName).arg(this->m_outFile->errorString()));
            return -1;
        }
        emit this->ZSigMsg(tr("创建新的录音文件:%1").arg(outFileName));
        //set flag.
        this->m_bNewFile=false;
    }

    //write data to file.
    this->m_outFile->write(data,len);
    this->m_nWrBytes+=len;
    qDebug()<<"wrBytes:"<<this->m_nWrBytes;
    //if the write bytes is enough.1MB.
    switch(this->m_cap->m_splitByWhat)
    {
    case CAP_SPLIT_BY_FILE_SIZE:
        if(this->m_nWrBytes>(this->m_cap->m_nSplitFileSize*1024))
        {
            //perpend wav header to file head section.
            WAVHEADER wavHeader;
            qstrcpy(wavHeader.RiffName,"RIFF");
            qstrcpy(wavHeader.WavName,"WAVE");
            qstrcpy(wavHeader.FmtName,"fmt ");
            qstrcpy(wavHeader.DATANAME,"data");

            wavHeader.nFmtLength = 16; //一般为16.
            wavHeader.nAudioFormat = 1;//PCM.
            wavHeader.nBitsPerSample=this->m_cap->audioFmt.sampleSize();
            wavHeader.nChannleNumber=this->m_cap->audioFmt.channelCount();
            wavHeader.nSampleRate=this->m_cap->audioFmt.sampleRate();
            wavHeader.nBytesPerSample = wavHeader.nChannleNumber * wavHeader.nBitsPerSample / 8;
            wavHeader.nBytesPerSecond = wavHeader.nSampleRate * wavHeader.nChannleNumber *  wavHeader.nBitsPerSample / 8;
            wavHeader.nRiffLength =this->m_nWrBytes - 8 + sizeof(WAVHEADER);
            wavHeader.nDataLength =this->m_nWrBytes;
            this->m_outFile->seek(0);
            this->m_outFile->write(reinterpret_cast<char*>(&wavHeader),sizeof(WAVHEADER));
            emit this->ZSigNewFileGenerated(this->m_outFile->fileName(),///<
                                            QString::number(this->m_outFile->size(),10),///<
                                            QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
            this->m_outFile->close();

            //reset.
            this->m_nWrBytes=0;

            //force to generate new file.
            this->m_bNewFile=true;
        }
        break;
    case CAP_SPLIT_BY_TIME_MSEC:
        if(this->m_cap->m_escapeMSec>=this->m_cap->m_nSplitTimeMSec)
        {
            //perpend wav header to file head section.
            WAVHEADER wavHeader;
            qstrcpy(wavHeader.RiffName,"RIFF");
            qstrcpy(wavHeader.WavName,"WAVE");
            qstrcpy(wavHeader.FmtName,"fmt ");
            qstrcpy(wavHeader.DATANAME,"data");

            wavHeader.nFmtLength = 16; //一般为16.
            wavHeader.nAudioFormat = 1;//PCM.
            wavHeader.nBitsPerSample=this->m_cap->audioFmt.sampleSize();
            wavHeader.nChannleNumber=this->m_cap->audioFmt.channelCount();
            wavHeader.nSampleRate=this->m_cap->audioFmt.sampleRate();
            wavHeader.nBytesPerSample = wavHeader.nChannleNumber * wavHeader.nBitsPerSample / 8;
            wavHeader.nBytesPerSecond = wavHeader.nSampleRate * wavHeader.nChannleNumber *  wavHeader.nBitsPerSample / 8;
            wavHeader.nRiffLength =this->m_nWrBytes - 8 + sizeof(WAVHEADER);
            wavHeader.nDataLength =this->m_nWrBytes;
            this->m_outFile->seek(0);
            this->m_outFile->write(reinterpret_cast<char*>(&wavHeader),sizeof(WAVHEADER));
            emit this->ZSigNewFileGenerated(this->m_outFile->fileName(),///<
                                            QString::number(this->m_outFile->size(),10),///<
                                            QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
            this->m_outFile->close();

            //reset.
            this->m_cap->m_escapeMSec=0;
            //reset.
            this->m_nWrBytes=0;

            //force to generate new file.
            this->m_bNewFile=true;
        }
        break;
    }

    //we return the data we've processed.
    return len;
    //return QBuffer::writeData(data,len);
}
QAudioCapture::QAudioCapture(QWidget *parent)
    : QWidget(parent)
{
    this->setWindowTitle(tr("大斌数据采集器FuckChina版"));
    this->m_tbAddDev=new QToolButton;
    this->m_tbAddDev->setText(tr("采样设备"));
    this->m_tbAddDev->setIconSize(QSize(48,48));
    this->m_tbAddDev->setIcon(QIcon(":/images/Hardware.png"));
    this->m_tbAddDev->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    this->m_tbStart=new QToolButton;
    this->m_tbStart->setText(tr("开始采集"));
    this->m_tbStart->setIconSize(QSize(48,48));
    this->m_tbStart->setIcon(QIcon(":/images/Start.png"));
    this->m_tbStart->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    this->m_tbCfg=new QToolButton;
    this->m_tbCfg->setText(tr("选项..."));
    this->m_tbCfg->setIconSize(QSize(48,48));
    this->m_tbCfg->setIcon(QIcon(":/images/Setup.png"));
    this->m_tbCfg->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    this->m_tbOpenDir=new QToolButton;
    this->m_tbOpenDir->setText(tr("存储目录"));
    this->m_tbOpenDir->setIconSize(QSize(48,48));
    this->m_tbOpenDir->setIcon(QIcon(":/images/Folder.png"));
    this->m_tbOpenDir->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    this->m_llEscapeMS=new QLabel;
    this->m_llEscapeMS->setAlignment(Qt::AlignCenter);
    this->m_llEscapeMS->setText("<font size=\"8\"><b>0</b></font><font size=\"4\">ms</font>");

    this->m_hLayout=new QHBoxLayout;
    this->m_hLayout->addWidget(this->m_tbAddDev);
    this->m_hLayout->addWidget(this->m_tbStart);
    this->m_hLayout->addWidget(this->m_tbCfg);
    this->m_hLayout->addWidget(this->m_tbOpenDir);
    this->m_hLayout->addStretch(1);
    this->m_hLayout->addWidget(this->m_llEscapeMS);

    this->m_tree=new QTreeWidget;
    this->m_tree->setColumnCount(3);
    QStringList headList;
    headList<<tr("录音文件");
    headList<<tr("文件大小");
    headList<<tr("生成时间");
    this->m_tree->setHeaderLabels(headList);
    this->m_tree->setIconSize(QSize(24,24));
    this->m_teLog=new QTextEdit;
    this->m_vLayout=new QVBoxLayout;
    this->m_vLayout->addLayout(this->m_hLayout);
    this->m_vLayout->addWidget(this->m_tree);
    this->m_vLayout->addWidget(this->m_teLog);
    this->setLayout(this->m_vLayout);

    connect(this->m_tbAddDev,SIGNAL(clicked(bool)),this,SLOT(ZSlotAddDev()));
    connect(this->m_tbStart,SIGNAL(clicked(bool)),this,SLOT(ZSlotStart()));
    connect(this->m_tbCfg,SIGNAL(clicked(bool)),this,SLOT(ZSlotCfg()));
    connect(this->m_tbOpenDir,SIGNAL(clicked(bool)),this,SLOT(ZSlotOpenDir()));

    this->m_bHWOkay=false;
    this->m_bCapturing=false;
    //default value.
    this->m_splitByWhat=CAP_SPLIT_BY_FILE_SIZE;
    this->m_nSplitFileSize=1024;//1024KB.
    this->m_nSplitTimeMSec=1000;//100ms.
}

QAudioCapture::~QAudioCapture()
{
    delete this->m_tbAddDev;
    delete this->m_tbStart;
    delete this->m_tbCfg;
    delete this->m_tbOpenDir;
    delete this->m_llEscapeMS;
    delete this->m_hLayout;
    delete this->m_tree;
    delete this->m_teLog;
    delete this->m_vLayout;
}
QSize QAudioCapture::sizeHint() const
{
    return QSize(700,350);
}
void QAudioCapture::ZSlotAddDev()
{
    if(this->m_bCapturing)
    {
        QMessageBox::critical(this,tr("提示"),tr("正在采集中,无法配置!\n请先停止采集,再进行配置!"));
        return;
    }

    ZListAudioInputDialog lstAudioInDia;
    if(lstAudioInDia.exec()==QDialog::Accepted)
    {
        this->audioDevInfo=lstAudioInDia.ZGetSelectedDevice();
        this->setWindowTitle(this->audioDevInfo.deviceName());
        this->m_HWName=this->audioDevInfo.deviceName();

        //set audio fmt.
        ZAudioFmtDialog fmtDia;
        QStringList codecList=this->audioDevInfo.supportedCodecs();
        for(qint32 i=0;i<codecList.size();i++)
        {
            fmtDia.m_cbCodec->addItem(codecList.at(i));
        }
        QList<int> rateList=this->audioDevInfo.supportedSampleRates();
        for(qint32 i=0;i<rateList.size();i++)
        {
            fmtDia.m_cbSampleRate->addItem(QString::number(rateList.at(i),10));
        }
        QList<int> sizeList=this->audioDevInfo.supportedSampleSizes();
        for(qint32 i=0;i<sizeList.size();i++)
        {
            fmtDia.m_cbSampleSize->addItem(QString::number(sizeList.at(i),10));
        }
        QList<int> chCountList=this->audioDevInfo.supportedChannelCounts();
        for(qint32 i=0;i<chCountList.size();i++)
        {
            fmtDia.m_cbChannelCount->addItem(QString::number(chCountList.at(i),10));
        }
        if(fmtDia.exec()==QDialog::Accepted)
        {
            this->audioFmt.setCodec(fmtDia.m_cbCodec->currentText());
            this->audioFmt.setChannelCount(fmtDia.m_cbChannelCount->currentText().toInt());
            this->audioFmt.setSampleRate(fmtDia.m_cbSampleRate->currentText().toInt());
            this->audioFmt.setSampleSize(fmtDia.m_cbSampleSize->currentText().toInt());
            this->audioFmt.setByteOrder(QAudioFormat::LittleEndian);
            this->audioFmt.setSampleType(QAudioFormat::UnSignedInt);
            this->m_bHWOkay=true;

            this->ZAddLog(tr("已选用设备:%1").arg(this->audioDevInfo.deviceName()));
            this->ZAddLog(tr("编码器:%1,通道数:%2,采样频率:%3,采样位数:%4.")///<
                          .arg(fmtDia.m_cbCodec->currentText())///<
                          .arg(fmtDia.m_cbChannelCount->currentText())///<
                          .arg(fmtDia.m_cbSampleRate->currentText()).arg(fmtDia.m_cbSampleSize->currentText()));

            //adjust audio format.
            if(!this->audioDevInfo.isFormatSupported(this->audioFmt))
            {
                this->ZAddLog(tr("您选择的音频格式不支持，自动调整!"));
                this->audioFmt=this->audioDevInfo.nearestFormat(this->audioFmt);
                this->ZAddLog(tr("编码器:%1,通道数:%2,采样频率:%3,采样位数:%4.")///<
                              .arg(this->audioFmt.codec())///<
                              .arg(this->audioFmt.channelCount())///<
                              .arg(this->audioFmt.sampleRate())///<
                              .arg(this->audioFmt.sampleSize()));
            }
        }
    }
}
void QAudioCapture::ZSlotStart()
{

    if(this->m_tbStart->text()==tr("开始采集"))
    {
        //check if hardware is okay.
        if(!this->m_bHWOkay)
        {
            QMessageBox::critical(this,tr("错误"),tr("未配置硬件采集接口!"));
            return;
        }
        
        if(this->m_tree->topLevelItemCount()>0)
        {
            if(QMessageBox::question(this,tr("警告"),tr("开启新的采集将覆盖历史文件!\n您确认要开始吗?"),QMessageBox::Ok,QMessageBox::Cancel)==QMessageBox::Cancel)
            {
                return;
            }
        }
        
        //create output buffer.
        this->bufDevice=new ZBuffer(this);
        this->baPCMData=new QByteArray;
        this->bufDevice->setBuffer(this->baPCMData);
        connect(this->bufDevice,SIGNAL(ZSigMsg(QString)),this,SLOT(ZSlotShowMsg(QString)));
        connect(this->bufDevice,SIGNAL(ZSigNewFileGenerated(QString,QString,QString)),this,SLOT(ZSlotNewFileGenerated(QString,QString,QString)));
        if(!this->bufDevice->open(QIODevice::WriteOnly|QIODevice::Truncate))
        {
            this->ZAddLog(this->audioDevInfo.deviceName()+this->bufDevice->errorString());
            delete this->bufDevice;
            this->bufDevice=NULL;
            delete this->baPCMData;
            this->baPCMData=NULL;
            return;
        }
        this->audioIn=new QAudioInput(this->audioDevInfo,this->audioFmt);
        this->m_escapeMSec=0;
        this->audioIn->setNotifyInterval(1);
        connect(this->audioIn,SIGNAL(notify()),this,SLOT(ZSlotNotify()));
        //start.
        this->audioIn->start(this->bufDevice);
        this->ZAddLog(this->audioDevInfo.deviceName()+"开始采样...");

        this->setWindowTitle(tr("设备:%1,状态:采集中").arg(this->m_HWName));
        this->m_bCapturing=true;

        this->m_tbStart->setText(tr("结束采集"));
        this->m_tbStart->setIcon(QIcon(":/images/Stop.png"));
    }else if(this->m_tbStart->text()==tr("结束采集"))
    {
        //stop capture.
        this->audioIn->stop();
        delete this->audioIn;
        this->audioIn=NULL;

        this->bufDevice->close();
        delete this->bufDevice;
        this->bufDevice=NULL;
        delete this->baPCMData;
        this->baPCMData=NULL;

        this->ZAddLog(tr("采样结束"));
        this->setWindowTitle(tr("设备:%1,状态:未采集").arg(this->m_HWName));
        this->m_bCapturing=false;

        this->m_tbStart->setText(tr("开始采集"));
        this->m_tbStart->setIcon(QIcon(":/images/Start.png"));
    }
}

void QAudioCapture::ZSlotNotify()
{
    this->m_escapeMSec++;
    this->m_llEscapeMS->setText(QString("<font size=\"8\"><b>%1</b></font><font size=\"4\">ms</font>").arg(QString::number(this->m_escapeMSec,10)));
}
void QAudioCapture::ZSlotCfg()
{
    if(this->m_bCapturing)
    {
        QMessageBox::critical(this,tr("提示"),tr("正在采集中,无法配置!\n请先停止采集,再进行配置!"));
        return;
    }
    ZCfgDialog cfgDia;
    cfgDia.ZSetSplitSize(this->m_nSplitFileSize);
    cfgDia.ZSetSplitMSec(this->m_nSplitTimeMSec);
    if(cfgDia.exec()==QDialog::Accepted)
    {
        switch(cfgDia.ZGetSplitByWhat())
        {
        case 1:
            this->m_splitByWhat=CAP_SPLIT_BY_FILE_SIZE;
            this->m_nSplitFileSize=cfgDia.ZGetSplitSize();
            this->ZAddLog(tr("文件将根据数据量大小拆分,每%1KB生成一个新的文件.").arg(QString::number(this->m_nSplitFileSize,10)));
            break;
        case 2:
            this->m_splitByWhat=CAP_SPLIT_BY_TIME_MSEC;
            this->m_nSplitTimeMSec=cfgDia.ZGetSplitMSec();
            this->ZAddLog(tr("文件将根据时间拆分,每隔%1ms生成一个新的文件.").arg(QString::number(this->m_nSplitTimeMSec,10)));
            break;
        default:
            break;
        }
    }
}
void QAudioCapture::ZSlotOpenDir()
{
    QDesktopServices::openUrl(QUrl(QString("file:///")+QApplication::applicationDirPath()+"/CapWav"));
}
void QAudioCapture::ZAddLog(QString log)
{
    QString timeStr=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString logHistory=this->m_teLog->toPlainText();
    logHistory.prepend(timeStr+","+log+"\r\n");
    this->m_teLog->setPlainText(logHistory);
}
void QAudioCapture::ZSlotShowMsg(QString msg)
{
    this->ZAddLog(msg);
}
void QAudioCapture::ZSlotNewFileGenerated(QString fileName,QString fileSize,QString createTime)
{
    QString newFileName=fileName.right(fileName.length()-fileName.lastIndexOf("/")-1);
    QTreeWidgetItem *item=new QTreeWidgetItem;
    item->setText(0,newFileName);
    item->setIcon(0,QIcon(":/images/FileName.png"));
    item->setText(1,fileSize);
    item->setIcon(1,QIcon(":/images/FileSize.png"));
    item->setText(2,createTime);
    item->setIcon(2,QIcon(":/images/Time.png"));
    this->m_tree->addTopLevelItem(item);
}
