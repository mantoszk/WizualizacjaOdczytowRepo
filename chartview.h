#ifndef CHARTVIEW_H
#define CHARTVIEW_H

#include <QtCharts>
#include <QtCharts/QPolarChart>
#include <QVector>
#include <QtMath>

enum sensors
{
    SharpSmall1 = 0,
    SharpSmall2,
    SharpBig,
    TFMini,
    VL53L0X,
    VL53L1,
    Sonar,
    SensorsCount
};

class ChartView : public QChartView
{
    Q_OBJECT

private:
    qreal angularMin = 0.0;
    qreal angularMax = 0.0;
    QString sensorNames[SensorsCount] = {"SharpSmall1", "SharpSmall2", "SharpBig", "TFMini", "VL53L0X", "VL53L1", "Sonar"};

    QPolarChart *polarChart = nullptr;

    void initChart(const quint8 count);
    void setAngular(const qreal min, const qreal max);
    void freeChart();

public:
    explicit ChartView(QWidget *parent = nullptr);
    ~ChartView();

    QVector <QScatterSeries *> *charts = nullptr;
    void addSeries();
    void clearPlot();

};

qreal realMod(qreal a, qreal b);

#endif
