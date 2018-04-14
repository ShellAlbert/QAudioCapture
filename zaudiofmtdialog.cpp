#include "zaudiofmtdialog.h"

ZAudioFmtDialog::ZAudioFmtDialog()
{
    this->setWindowTitle(tr("采样参数设置"));

    this->m_llCodec=new QLabel(tr("编码器:"));
    this->m_cbCodec=new QComboBox;

    this->m_llSampleRate=new QLabel(tr("采样率:"));
    this->m_cbSampleRate=new QComboBox;

    this->m_llSampleSize=new QLabel(tr("数据大小:"));
    this->m_cbSampleSize=new QComboBox;

    this->m_llChannelCount=new QLabel(tr("通道数:"));
    this->m_cbChannelCount=new QComboBox;

    this->m_gLayout=new QGridLayout;
    this->m_gLayout->addWidget(this->m_llCodec,0,0,1,1);
    this->m_gLayout->addWidget(this->m_cbCodec,0,1,1,1);

    this->m_gLayout->addWidget(this->m_llSampleRate,1,0,1,1);
    this->m_gLayout->addWidget(this->m_cbSampleRate,1,1,1,1);

    this->m_gLayout->addWidget(this->m_llSampleSize,2,0,1,1);
    this->m_gLayout->addWidget(this->m_cbSampleSize,2,1,1,1);

    this->m_gLayout->addWidget(this->m_llChannelCount,3,0,1,1);
    this->m_gLayout->addWidget(this->m_cbChannelCount,3,1,1,1);


    this->m_tbOkay=new QToolButton;
    this->m_tbOkay->setText(tr("确定"));

    this->m_tbCancel=new QToolButton;
    this->m_tbCancel->setText(tr("取消"));

    this->m_hLayout=new QHBoxLayout;
    this->m_hLayout->addStretch(1);
    this->m_hLayout->addWidget(this->m_tbOkay);
    this->m_hLayout->addWidget(this->m_tbCancel);

    this->m_vLayout=new QVBoxLayout;
    this->m_vLayout->addLayout(this->m_gLayout);
    this->m_vLayout->addLayout(this->m_hLayout);
    this->setLayout(this->m_vLayout);

    connect(this->m_tbOkay,SIGNAL(clicked(bool)),this,SLOT(accept()));
    connect(this->m_tbCancel,SIGNAL(clicked(bool)),this,SLOT(reject()));
}
ZAudioFmtDialog::~ZAudioFmtDialog()
{
    delete this->m_llCodec;
    delete this->m_cbCodec;

    delete this->m_llSampleRate;
    delete this->m_cbSampleRate;

    delete this->m_llSampleSize;
    delete this->m_cbSampleSize;

    delete this->m_llChannelCount;
    delete this->m_cbChannelCount;

    delete this->m_gLayout;


    delete this->m_tbOkay;
    delete this->m_tbCancel;
    delete this->m_hLayout;

    delete this->m_vLayout;
}


