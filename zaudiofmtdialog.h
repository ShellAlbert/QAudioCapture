#ifndef ZAUDIOFMTDIALOG_H
#define ZAUDIOFMTDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QComboBox>
#include <QGridLayout>
#include <QToolButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
class ZAudioFmtDialog : public QDialog
{
public:
    ZAudioFmtDialog();
    ~ZAudioFmtDialog();
public:
    QLabel *m_llCodec;
    QComboBox *m_cbCodec;

    QLabel *m_llSampleRate;
    QComboBox *m_cbSampleRate;

    QLabel *m_llSampleSize;
    QComboBox *m_cbSampleSize;

    QLabel *m_llChannelCount;
    QComboBox *m_cbChannelCount;

    QGridLayout *m_gLayout;

    QHBoxLayout *m_hLayout;
    QToolButton *m_tbOkay;
    QToolButton *m_tbCancel;

    QVBoxLayout *m_vLayout;
};

#endif // ZAUDIOFMTDIALOG_H
