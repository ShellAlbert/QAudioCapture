#ifndef ZLISTAUDIOINPUTDIALOG_H
#define ZLISTAUDIOINPUTDIALOG_H

#include <QDialog>
#include <QToolButton>
#include <QListWidget>
#include <QHBoxLayout>
#include <QAudioDeviceInfo>
#include <QMap>
class ZListAudioInputDialog : public QDialog
{
    Q_OBJECT
public:
    ZListAudioInputDialog();
    ~ZListAudioInputDialog();

    QAudioDeviceInfo ZGetSelectedDevice() const;
private slots:
    void ZSlotShowDetail(QListWidgetItem*item);
    void ZSlotOkay();
    void ZSlotCancel();
private:
    QListWidget *m_list;
    QToolButton *m_tbOkay;
    QToolButton *m_tbCancel;
    QHBoxLayout *m_hLayout;
    QVBoxLayout *m_vLayout;
    QMap<QString,QAudioDeviceInfo> m_mapDev;
};

#endif // ZLISTAUDIOINPUTDIALOG_H
