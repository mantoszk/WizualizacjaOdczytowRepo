#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "console.h"
#include "settingsdialog.h"

#include <QLabel>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), m_ui(new Ui::MainWindow),
    m_status(new QLabel), m_settings(new SettingsDialog), m_serial(new QSerialPort(this))
{
    m_ui->setupUi(this);
    m_ui->m_console->setEnabled(false);
    m_ui->actionConnect->setEnabled(true);
    m_ui->actionDisconnect->setEnabled(false);
    m_ui->actionQuit->setEnabled(true);
    m_ui->actionConfigure->setEnabled(true);
    m_ui->actionLaser->setEnabled(false);
    m_ui->actionSharpSmall1->setEnabled(false);
    m_ui->actionSharpSmall2->setEnabled(false);
    m_ui->actionSharpBig->setEnabled(false);
    m_ui->actionTFMini->setEnabled(false);
    m_ui->actionVL53L0X->setEnabled(false);
    m_ui->actionVL53L1->setEnabled(false);
    m_ui->actionSequential->setEnabled(false);
    m_ui->actionSimultaneous->setEnabled(false);
    m_ui->statusBar->addWidget(m_status);

    initActionsConnections();
    connect(m_serial, &QSerialPort::errorOccurred, this, &MainWindow::handleError);
    connect(m_serial, &QSerialPort::readyRead, this, &MainWindow::readData);
    connect(m_ui->m_console, &Console::getData, this, &MainWindow::writeData);

    m_ui->m_chart->addSeries();
}

MainWindow::~MainWindow()
{
    delete m_settings;
    delete m_ui;
}

void MainWindow::openSerialPort()
{
    const SettingsDialog::Settings p = m_settings->settings();
    m_serial->setPortName(p.name);
    m_serial->setBaudRate(p.baudRate);
    m_serial->setDataBits(p.dataBits);
    m_serial->setParity(p.parity);
    m_serial->setStopBits(p.stopBits);
    m_serial->setFlowControl(p.flowControl);

    if (m_serial->open(QIODevice::ReadWrite))
    {
        m_ui->m_console->setEnabled(true);
        m_ui->m_console->setLocalEchoEnabled(p.localEchoEnabled);
        m_ui->actionConnect->setEnabled(false);
        m_ui->actionDisconnect->setEnabled(true);
        m_ui->actionConfigure->setEnabled(false);
        m_ui->actionLaser->setEnabled(true);
        m_ui->actionSharpSmall1->setEnabled(true);
        m_ui->actionSharpSmall2->setEnabled(true);
        m_ui->actionSharpBig->setEnabled(true);
        m_ui->actionTFMini->setEnabled(true);
        m_ui->actionVL53L0X->setEnabled(true);
        m_ui->actionVL53L1->setEnabled(true);
        m_ui->actionSequential->setEnabled(true);
        m_ui->actionSimultaneous->setEnabled(true);

        showStatusMessage(tr("Połączono z %1 : %2, %3, %4, %5, %6").arg(p.name).arg(p.stringBaudRate).arg(p.stringDataBits)
                          .arg(p.stringParity).arg(p.stringStopBits).arg(p.stringFlowControl));
    }
    else
    {
        QMessageBox::critical(this, tr("Błąd"), m_serial->errorString());
        showStatusMessage(tr("Błąd połączenia"));
    }
}

void MainWindow::closeSerialPort()
{
    if (m_serial->isOpen())
    {
        m_serial->close();
    }

    m_ui->m_console->setEnabled(false);
    m_ui->actionConnect->setEnabled(true);
    m_ui->actionDisconnect->setEnabled(false);
    m_ui->actionQuit->setEnabled(true);
    m_ui->actionConfigure->setEnabled(true);
    m_ui->actionLaser->setEnabled(false);
    m_ui->actionSharpSmall1->setEnabled(false);
    m_ui->actionSharpSmall2->setEnabled(false);
    m_ui->actionSharpBig->setEnabled(false);
    m_ui->actionTFMini->setEnabled(false);
    m_ui->actionVL53L0X->setEnabled(false);
    m_ui->actionVL53L1->setEnabled(false);
    m_ui->actionSequential->setEnabled(false);
    m_ui->actionSimultaneous->setEnabled(false);

    showStatusMessage(tr("Rozłączono"));
}

void MainWindow::writeData(const QByteArray &data)
{
    m_serial->write(data);
}

void MainWindow::readData()
{
    const QByteArray dataBuffer = m_serial->readLine();
    m_ui->m_console->putData(dataBuffer);
    uartBuffer.append(dataBuffer);

    if(uartBuffer.contains("\r\n"))
    {
        QString myString(uartBuffer);

        if(myString.mid(0,hexLength).toUInt(nullptr,16) == header)
        {
            for(int i = hexLength, sensor = SharpSmall1; sensor < SensorsCount; i+=hexLength, ++sensor)
            {
                uint temp = myString.mid(i,hexLength).toUInt(nullptr,16);
                qDebug() << temp;
                if(temp != 0 )
                {
                    if(sensor == SharpSmall1)
                        m_ui->m_chart->charts->at(sensor)->append(angleIterator,SharpSmall1A/temp+SharpSmall1B);
                    else if(sensor == SharpSmall2)
                        m_ui->m_chart->charts->at(sensor)->append(angleIterator,SharpSmall2A/temp+SharpSmall2B);
                    else if(sensor == SharpBig)
                        m_ui->m_chart->charts->at(sensor)->append(angleIterator,SharpBigA/temp+SharpBigB);
                    else if(sensor == TFMini)
                        m_ui->m_chart->charts->at(sensor)->append(angleIterator,temp);
                    else if(sensor == VL53L0X)
                        m_ui->m_chart->charts->at(sensor)->append(angleIterator,temp/10.0);
                    else if(sensor == VL53L1)
                        m_ui->m_chart->charts->at(sensor)->append(angleIterator,temp/10.0);
                    else if(sensor == Sonar)
                        m_ui->m_chart->charts->at(sensor)->append(angleIterator,temp);
                }
            }
            ++angleIterator;
        }
        uartBuffer = "";
    }
}

void MainWindow::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError)
    {
        QMessageBox::critical(this, tr("Błąd krytyczny"), m_serial->errorString());
        closeSerialPort();
    }
}

void MainWindow::erasePlot()
{
    m_ui->m_chart->clearPlot();
}

void MainWindow::initActionsConnections()
{
    connect(m_ui->actionConnect, &QAction::triggered, this, &MainWindow::openSerialPort);
    connect(m_ui->actionDisconnect, &QAction::triggered, this, &MainWindow::closeSerialPort);
    connect(m_ui->actionQuit, &QAction::triggered, this, &MainWindow::close);
    connect(m_ui->actionConfigure, &QAction::triggered, m_settings, &SettingsDialog::show);
    connect(m_ui->actionClear, &QAction::triggered, m_ui->m_console, &Console::clear);
    connect(m_ui->actionLaser, &QAction::triggered, this, [this]{writeData(QByteArrayLiteral("x"));});
    connect(m_ui->actionSequential, &QAction::triggered, this, [this]{writeData(QByteArrayLiteral("z"));});
    connect(m_ui->actionSimultaneous, &QAction::triggered, this, [this]{writeData(QByteArrayLiteral("c"));});
    connect(m_ui->actionSharpSmall1, &QAction::triggered, this, [this]{writeData(QByteArrayLiteral("q"));});
    connect(m_ui->actionSharpSmall2, &QAction::triggered, this, [this]{writeData(QByteArrayLiteral("w"));});
    connect(m_ui->actionSharpBig, &QAction::triggered, this, [this]{writeData(QByteArrayLiteral("e"));});
    connect(m_ui->actionTFMini, &QAction::triggered, this, [this]{writeData(QByteArrayLiteral("r"));});
    connect(m_ui->actionVL53L0X, &QAction::triggered, this, [this]{writeData(QByteArrayLiteral("t"));});
    connect(m_ui->actionVL53L1, &QAction::triggered, this, [this]{writeData(QByteArrayLiteral("y"));});
    connect(m_ui->actionClearPlot, &QAction::triggered, this, &MainWindow::erasePlot);

}

void MainWindow::showStatusMessage(const QString &message)
{
    m_status->setText(message);
}
