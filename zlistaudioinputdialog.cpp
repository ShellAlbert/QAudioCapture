#include "zlistaudioinputdialog.h"
#include <QAudioDeviceInfo>
#include <QMessageBox>
#include "zaudiofmtdialog.h"
ZListAudioInputDialog::ZListAudioInputDialog()
{
    this->setWindowTitle(tr("选择音频输入设备"));
    this->m_list=new QListWidget;

    this->m_tbOkay=new QToolButton;
    this->m_tbOkay->setText(tr("确定"));

    this->m_tbCancel=new QToolButton;
    this->m_tbCancel->setText(tr("取消"));

    this->m_hLayout=new QHBoxLayout;
    this->m_hLayout->addStretch(1);
    this->m_hLayout->addWidget(this->m_tbOkay);
    this->m_hLayout->addWidget(this->m_tbCancel);

    this->m_vLayout=new QVBoxLayout;
    this->m_vLayout->addWidget(this->m_list);
    this->m_vLayout->addLayout(this->m_hLayout);
    this->setLayout(this->m_vLayout);

    connect(this->m_list,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(ZSlotShowDetail(QListWidgetItem*)));

    connect(this->m_tbOkay,SIGNAL(clicked(bool)),this,SLOT(ZSlotOkay()));
    connect(this->m_tbCancel,SIGNAL(clicked(bool)),this,SLOT(ZSlotCancel()));

    QList<QAudioDeviceInfo> audioDevList=QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
    for(qint32 i=0;i<audioDevList.size();i++)
    {
        QAudioDeviceInfo audioDev=audioDevList.at(i);
        if(!audioDev.isNull())
        {
            //add to list.
            QListWidgetItem *item=new QListWidgetItem;
            item->setText(audioDev.deviceName());
            this->m_list->addItem(item);

            //add to map.
            this->m_mapDev.insert(audioDev.deviceName(),audioDev);
        }
    }
}
ZListAudioInputDialog::~ZListAudioInputDialog()
{
    this->m_mapDev.clear();

    delete this->m_list;
    delete this->m_tbOkay;
    delete this->m_tbCancel;
    delete this->m_hLayout;
    delete this->m_vLayout;
}
void ZListAudioInputDialog::ZSlotShowDetail(QListWidgetItem*item)
{
    if(this->m_mapDev.contains(item->text()))
    {
        QAudioDeviceInfo devInfo=this->m_mapDev.value(item->text());
        QString detailMsg;
        detailMsg+="Supported Channel Counts:";
        QList<int> channelList=devInfo.supportedChannelCounts();
        for(qint32 i=0;i<channelList.size();i++)
        {
           detailMsg+=QString::number(channelList.at(i),10)+",";
        }
        detailMsg+="\r\nSupported Codec:";
        QStringList codecList=devInfo.supportedCodecs();
        for(qint32 i=0;i<codecList.size();i++)
        {
            detailMsg+=codecList.at(i)+",";
        }
        detailMsg+="\r\nSupported Sample Rates:";
        QList<int> rateList=devInfo.supportedSampleRates();
        for(qint32 i=0;i<rateList.size();i++)
        {
            detailMsg+=QString::number(rateList.at(i),10)+",";
        }
        detailMsg+="\r\nSupported Sample Size:";
        QList<int> sizeList=devInfo.supportedSampleSizes();
        for(qint32 i=0;i<sizeList.size();i++)
        {
            detailMsg+=QString::number(sizeList.at(i),10)+",";
        }
        QMessageBox::information(this,tr("详细数据"),detailMsg);
    }
}
void ZListAudioInputDialog::ZSlotOkay()
{
    if(NULL==this->m_list->currentItem())
    {
        return;
    }

    this->accept();
}
void ZListAudioInputDialog::ZSlotCancel()
{
    this->reject();
}
QAudioDeviceInfo ZListAudioInputDialog::ZGetSelectedDevice() const
{
    QString selectDev=this->m_list->currentItem()->text();
    return this->m_mapDev.value(selectDev);
}
