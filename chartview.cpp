#include "chartview.h"

ChartView::ChartView(QWidget *parent) : QChartView(parent)
{
    charts = new QVector <QScatterSeries *>;
    initChart(SensorsCount);
    polarChart = new QPolarChart;
    setAngular(0,360);
}

ChartView::~ChartView()
{
    freeChart();
    delete polarChart;
}

void ChartView::initChart(const quint8 count)
{
    for(int i = 0; i< count; ++i)
        charts->push_back(new QScatterSeries);
}

void ChartView::freeChart()
{
    for(auto &i: *charts)
        delete i;
}

void ChartView::addSeries()
{
    QValueAxis *angularAxis = new QValueAxis();
    angularAxis->setTickCount(11);
    angularAxis->setLabelFormat("%.1f");
    angularAxis->setShadesVisible(true);
    angularAxis->setShadesBrush(QBrush(QColor(249, 249, 255)));
    polarChart->addAxis(angularAxis, QPolarChart::PolarOrientationAngular);

    QValueAxis *radialAxis = new QValueAxis();
    radialAxis->setTickCount(9);
    radialAxis->setLabelFormat("%d");
    polarChart->addAxis(radialAxis, QPolarChart::PolarOrientationRadial);

    setAngular(0,360);
    angularAxis->setRange(this->angularMin, this->angularMax);

    charts->at(0)->append(0.0, 20.0);
    charts->at(0)->append(0.0, 500.0);
    for(int i=0; i<SensorsCount;++i)
    {
        polarChart->addSeries(charts->at(i));
        charts->at(i)->setName(sensorNames[i]);
        charts->at(i)->setMarkerSize(9);
        charts->at(i)->attachAxis(radialAxis);
        charts->at(i)->attachAxis(angularAxis);
    }
    this->setChart(polarChart);
    this->setRenderHint(QPainter::Antialiasing);
    charts->at(0)->clear();

}

void ChartView::setAngular(const qreal min, const qreal max)
{
    this->angularMin = realMod(min, 360.0 + 1e-6);
    this->angularMax = realMod(max, 360.0 + 1e-6);

    if(this->angularMin > this->angularMax)
    {
        qreal temp = this->angularMin;
        this->angularMin = this->angularMax;
        this->angularMax = temp;
        qDebug() << "Min i Max zamienione";
    }
    else if (this->angularMin == this->angularMax)
        qDebug() << "Min i Max są jednakowe";
}

void ChartView::clearPlot()
{
    for(int i = 0; i<SensorsCount; ++i)
    {
        charts->at(i)->clear();
    }
}

qreal realMod(qreal a, qreal b)
{
    return (a - b * qFloor(a/b));
}
