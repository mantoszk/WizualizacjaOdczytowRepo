#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include "chartview.h"

QT_BEGIN_NAMESPACE

class QLabel;

namespace Ui {
class MainWindow;
}

QT_END_NAMESPACE

class Console;
class SettingsDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openSerialPort();
    void closeSerialPort();
    void writeData(const QByteArray &data);
    void readData();
    void handleError(QSerialPort::SerialPortError error);
    void erasePlot();

private:
    void initActionsConnections();
    void showStatusMessage(const QString &message);

    Ui::MainWindow *m_ui = nullptr;
    QLabel *m_status = nullptr;
    SettingsDialog *m_settings = nullptr;
    QSerialPort *m_serial = nullptr;

    int angleIterator = 0;
    const uint header = 0x069;
    const int hexLength = 3;
    QByteArray uartBuffer = "";
    QVector <uint> sensorsData[SensorsCount];

    const double SharpSmall1A = 70588.94;
    const double SharpSmall1B = -2.06;
    const double SharpSmall2A = 73797.4;
    const double SharpSmall2B = -5.89;
    const double SharpBigA = 1031273.32;
    const double SharpBigB = -340.58;

};

#endif // MAINWINDOW_H
