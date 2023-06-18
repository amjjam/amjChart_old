#include "amjChartView.H"
#include "ui_amjChartView.h"

#include <iostream>

#include <QSharedPointer>

amjChartView::amjChartView(QWidget *parent)
  : QWidget(parent), ui(new Ui::amjChartView),t1(-60),t2(1){
  ui->setupUi(this);

  QSharedPointer<QCPAxisTickerDateTime> timeTicker(new QCPAxisTickerDateTime);
  timeTicker->setDateTimeFormat("h:mm:ss.zzz");
  ui->plot->xAxis->setTicker(timeTicker);

  connect(&updateTimer,&QTimer::timeout,this,&amjChartView::_update);
  updateTimer.start(100);
}

amjChartView::~amjChartView(){
  delete ui;
}

void amjChartView::ngraphs(int n){
  if(ui->plot->graphCount()!=n){
    ui->plot->clearGraphs();
    for(int i=0;i<n;i++)
      ui->plot->addGraph();
    }
}

void amjChartView::append(QDateTime t, QVector<double> d){
  if(d.size()!=ui->plot->graphCount()){
    std::cout << "amjChart::append: count mismatch: d.size()=" << d.size() << ", graphCount="
              << ui->plot->graphCount() << std::endl;
    exit(EXIT_FAILURE);
  }
  for(int i=0;i<ui->plot->graphCount();i++)
    ui->plot->graph(i)->addData(t.toMSecsSinceEpoch()/1000,d[i]);
}

void amjChartView::_update(){
  double now=(double)QDateTime::currentMSecsSinceEpoch()/1000;
  ui->plot->xAxis->setRange(now+t1,now+t2);
  ui->plot->replot();
}
