#ifndef QAUDIOCAPTURE_H
#define QAUDIOCAPTURE_H

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolButton>
#include <QAudioInput>
#include <QFile>
#include <QTextEdit>
#include <QListWidget>
#include <QSplitter>
#include <QMap>
#include <QBuffer>
#include <QTreeWidget>
#include <QIODevice>
#include <QLabel>
enum
{
    CAP_SPLIT_BY_FILE_SIZE=0,
    CAP_SPLIT_BY_TIME_MSEC=1,
};
class QAudioCapture;
class ZBuffer:public QBuffer
{
    Q_OBJECT
public:
    ZBuffer(QAudioCapture *cap);
    ~ZBuffer();
protected:
    qint64 writeData(const char *data, qint64 len);
signals:
    void ZSigMsg(QString msg);
    void ZSigNewFileGenerated(QString fileName,QString fileSize,QString createTime);
private:
    QFile *m_outFile;
    QString m_outFileName;
    qint32 m_fileNameIndex;
    bool m_bNewFile;
    qint32 m_nWrBytes;
private:
    QAudioCapture *m_cap;
};
class QAudioCapture : public QWidget
{
    Q_OBJECT
public:
    QAudioCapture(QWidget *parent = 0);
    ~QAudioCapture();
protected:
    QSize sizeHint() const;
private slots:
    void ZSlotAddDev();
    void ZSlotStart();
    void ZSlotNotify();
    void ZSlotCfg();
    void ZSlotOpenDir();
    void ZSlotShowMsg(QString msg);
    void ZSlotNewFileGenerated(QString fileName,QString fileSize,QString createTime);
private:
    void ZAddLog(QString log);
private:
    QHBoxLayout *m_hLayout;
    QToolButton *m_tbAddDev;
    QToolButton *m_tbStart;
    QToolButton *m_tbCfg;
    QToolButton *m_tbOpenDir;
    QLabel *m_llEscapeMS;

    QTreeWidget *m_tree;
    QTextEdit *m_teLog;
    QVBoxLayout *m_vLayout;
public:
    bool m_bHWOkay;
    QAudioDeviceInfo audioDevInfo;
    QAudioFormat audioFmt;
    QAudioInput *audioIn;
    QFile *outFile;
    ZBuffer *bufDevice;
    QByteArray *baPCMData;
    QString m_HWName;

public:
    qint32 m_escapeMSec;
    qint32 m_splitByWhat;
    qint32 m_nSplitFileSize;
    qint32 m_nSplitTimeMSec;
    bool m_bCapturing;
};

#endif // QAUDIOCAPTURE_H
