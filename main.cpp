#include "qaudiocapture.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyleSheet("QTextEdit {"
                    "border: 1px solid #32435E;   "
                    "border-radius: 3px; color: #BBBBBB; "
                    "background: #222222;"
                    "selection-background-color: #0A246A;}");
    QAudioCapture w;
    w.show();
    return a.exec();
}
