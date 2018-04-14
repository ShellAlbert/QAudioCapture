#include "zcfgdialog.h"

ZCfgDialog::ZCfgDialog()
{
    this->setWindowTitle(tr("选项"));
    this->m_cbSplitBySize=new QCheckBox(tr("按数据量拆分"));
    this->m_spBySize=new QSpinBox;//KB.
    this->m_spBySize->setRange(100,1024*2);//100K~2MB.
    this->m_llSizeUnit=new QLabel(tr("(KB 千字节)"));

    this->m_cbSplitByTime=new QCheckBox(tr("按时间拆分"));
    this->m_spByTime=new QSpinBox;
    this->m_spByTime->setRange(1000,60000);//1s~60s.
    this->m_llTimeUnit=new QLabel(tr("(ms 毫秒)"));

    this->m_btnGrp=new QButtonGroup;
    this->m_btnGrp->addButton(this->m_cbSplitBySize);
    this->m_btnGrp->addButton(this->m_cbSplitByTime);
    this->m_btnGrp->setExclusive(true);
    this->m_cbSplitBySize->setChecked(true);

    this->m_gLayout=new QGridLayout;
    this->m_gLayout->addWidget(this->m_cbSplitBySize,0,0,1,1);
    this->m_gLayout->addWidget(this->m_spBySize,0,1,1,1);
    this->m_gLayout->addWidget(this->m_llSizeUnit,0,2,1,1);
    this->m_gLayout->addWidget(this->m_cbSplitByTime,1,0,1,1);
    this->m_gLayout->addWidget(this->m_spByTime,1,1,1,1);
    this->m_gLayout->addWidget(this->m_llTimeUnit,1,2,1,1);

    this->m_tbOkay=new QToolButton;
    this->m_tbOkay->setText(tr("Okay"));
    this->m_tbCancel=new QToolButton;
    this->m_tbCancel->setText(tr("Cancel"));
    this->m_hLayout=new QHBoxLayout;
    this->m_hLayout->addStretch(1);
    this->m_hLayout->addWidget(this->m_tbOkay);
    this->m_hLayout->addWidget(this->m_tbCancel);

    this->m_vLayout=new QVBoxLayout;
    this->m_vLayout->addLayout(this->m_gLayout);
    this->m_vLayout->addLayout(this->m_hLayout);
    this->setLayout(this->m_vLayout);

    connect(this->m_tbOkay,SIGNAL(clicked(bool)),this,SLOT(ZSlotOkay()));
    connect(this->m_tbCancel,SIGNAL(clicked(bool)),this,SLOT(ZSlotCancel()));
}
ZCfgDialog::~ZCfgDialog()
{
    delete this->m_btnGrp;
    delete this->m_cbSplitBySize;
    delete this->m_spBySize;
    delete this->m_llSizeUnit;
    delete this->m_cbSplitByTime;
    delete this->m_spByTime;
    delete this->m_llTimeUnit;
    delete this->m_gLayout;

    delete this->m_tbOkay;
    delete this->m_tbCancel;
    delete this->m_hLayout;

    delete this->m_vLayout;
}
qint32 ZCfgDialog::ZGetSplitByWhat()
{
    if(this->m_cbSplitBySize->isChecked())
    {
        return 1;
    }else if(this->m_cbSplitByTime->isChecked())
    {
        return 2;
    }
}
qint32 ZCfgDialog::ZGetSplitSize()
{
    return this->m_spBySize->value();
}
qint32 ZCfgDialog::ZGetSplitMSec()
{
    return this->m_spByTime->value();
}

void ZCfgDialog::ZSetSplitSize(qint32 size)
{
    this->m_spBySize->setValue(size);
}
void ZCfgDialog::ZSetSplitMSec(qint32 msec)
{
    this->m_spByTime->setValue(msec);
}
void ZCfgDialog::ZSlotOkay()
{
    this->accept();
}
void ZCfgDialog::ZSlotCancel()
{
    this->reject();
}
