#ifndef ZCFGDIALOG_H
#define ZCFGDIALOG_H

#include <QDialog>
#include <QToolButton>
#include <QCheckBox>
#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include <QHBoxLayout>
#include <QToolButton>
#include <QGridLayout>
#include <QButtonGroup>
class ZCfgDialog : public QDialog
{
    Q_OBJECT
public:
    ZCfgDialog();
    ~ZCfgDialog();

    qint32 ZGetSplitByWhat();
    qint32 ZGetSplitSize();
    qint32 ZGetSplitMSec();

    void ZSetSplitSize(qint32 size);
    void ZSetSplitMSec(qint32 msec);
private slots:
    void ZSlotOkay();
    void ZSlotCancel();
private:
    QCheckBox *m_cbSplitBySize;
    QSpinBox *m_spBySize;
    QLabel *m_llSizeUnit;
    QCheckBox *m_cbSplitByTime;
    QSpinBox *m_spByTime;
    QLabel *m_llTimeUnit;
    QButtonGroup *m_btnGrp;

    QGridLayout *m_gLayout;
    QToolButton *m_tbOkay;
    QToolButton *m_tbCancel;
    QHBoxLayout *m_hLayout;
    QVBoxLayout *m_vLayout;
};

#endif // ZCFGDIALOG_H
