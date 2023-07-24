#include "amjChart.H"
#include "ui_amjChartView.h"

#include <iostream>

#include <QSharedPointer>
#include <QString>

#include <cmath>

amjChartView::amjChartView(QWidget *parent)
  : QWidget(parent), ui(new Ui::amjChartView),t1d(-100),t2d(1),tscale(1),
    tref((double)QDateTime::currentDateTimeUtc().toMSecsSinceEpoch()/1000),
    t1(t1d),t2(t2d),nPanels(0),nGraphs(0),
    _showLegend(false),_legendWrap(5),_wheelFactor(0.05),_updateInterval(100){
  ui->setupUi(this);

  npanels(1);

  _colors << Qt::red << Qt::green << Qt::blue << Qt::black << Qt::darkRed << Qt::darkGreen << Qt::darkBlue;

  connect(ui->plot,&QCustomPlot::mouseWheel,this,&amjChartView::wheelEvent);
  connect(&updateTimer,&QTimer::timeout,this,&amjChartView::_update);
  updateTimer.start(_updateInterval);
  connect(ui->toolButton_pause,&QToolButton::clicked,this,&amjChartView::pauseClicked);
  connect(ui->toolButton_reset,&QToolButton::clicked,this,&amjChartView::resetClicked);
}

amjChartView::~amjChartView(){
  delete ui;
}

void amjChartView::clear(){
  ui->plot->plotLayout()->clear();
  ui->plot->clearGraphs();
}

void amjChartView::npanels(int n){
  nPanels=n;
  format();
}

void amjChartView::ngraphs(int n){
  nGraphs=n;
  format();
}

int amjChartView::ngraphs(){
  return ui->plot->graphCount();
}

void amjChartView::panelnames(std::vector<std::string> &panelnames){
  _panelnames.resize(panelnames.size());
  for(int i=0;i<_panelnames.size();i++)
    _panelnames[i]=QString::fromStdString(panelnames[i]);
  format();
}

void amjChartView::graphnames(std::vector<std::string> &graphnames){
  _graphnames.resize(graphnames.size());
  for(int i=0;i<_graphnames.size();i++)
    _graphnames[i]=QString::fromStdString(graphnames[i]);
  format();
}

void amjChartView::append(QDateTime t, QVector<double> d){
  if(d.size()!=ui->plot->graphCount()){
    std::cout << "amjChart::append: count mismatch: d.size()=" << d.size() << ", graphCount="
              << ui->plot->graphCount() << std::endl;
    exit(EXIT_FAILURE);
  }
  for(int i=0;i<ui->plot->graphCount();i++)
    ui->plot->graph(i)->addData((double)t.toMSecsSinceEpoch()/1000,d[i]);
}

void amjChartView::legendList(std::vector<int> l){// Adds listed graphs to legend
  _legend_graphs=QList<int>::fromVector(QVector<int>(l.begin(),l.end()));
  format();
}

void amjChartView::legendShow(bool t){
  _showLegend=t;
  format();
}

void amjChartView::legendWrap(int w){
  _legendWrap=w;
  format();
}

void amjChartView::toolbarShow(bool v){
  ui->toolbar->setVisible(v);
}

void amjChartView::updateInterval(int u){
  _updateInterval=u;
  run(updateTimer.isActive()); // restart with new interval if it is running
}

void amjChartView::run(bool r){
  if(r)
    updateTimer.start(_updateInterval);
  else
    updateTimer.stop();
}

void amjChartView::_update(){
  if(updateTimer.isActive()){
    tref=(double)QDateTime::currentDateTimeUtc().toMSecsSinceEpoch()/1000;
    t1=t1d*tscale;//*=(1.0+(double)event->angleDelta().y()/120*_wheelFactor);
    t2=t2d*tscale;//(1.0+(double)event->angleDelta().y()/120*_wheelFactor);
  }
  QCPAxisRect *r=ui->plot->axisRect(ui->plot->axisRectCount()-1);
  r->axis(QCPAxis::atBottom,0)->setRange(tref+t1,tref+t2);
  for(int i=0;i<ui->plot->axisRectCount();i++)
    if(_autoscale[i])
      ui->plot->axisRect(i)->axis(QCPAxis::atLeft,0)->rescale();
  ui->plot->replot();
}

void amjChartView::format(){
  // Clear the plot
  ui->plot->plotLayout()->clear();

  // Enable axis selection and zoom interaction
  ui->plot->setInteraction(QCP::iSelectAxes);
  ui->plot->setInteraction(QCP::iRangeZoom);

  // Initialize variables
  _autoscale.resize(nPanels);
  for(int i=0;i<_autoscale.size();i++) _autoscale[i]=true;

  ui->plot->plotLayout()->setRowSpacing(0);
  QCPMarginGroup *mgroup=new QCPMarginGroup(ui->plot); // Create a margin group for the sides
  for(int i=0;i<nPanels;i++){
     // Create panel
     QCPAxisRect *r=new QCPAxisRect(ui->plot);
     QMargins margins;
     QCP::MarginSides automarginsides=QCP::MarginSide::msLeft|QCP::MarginSide::msRight;
     r->setMarginGroup(automarginsides,mgroup);
     if(i>0)
       margins.setTop(0);
     if(i<nPanels)
       margins.setBottom(0);
     if(i==0)
       automarginsides|=QCP::MarginSide::msTop;
     if(i==nPanels-1)
       automarginsides|=QCP::MarginSide::msBottom;
     r->setAutoMargins(automarginsides);
     r->setMargins(margins);

     // Set x-axis to be time
     QSharedPointer<QCPAxisTickerDateTime> timeTicker(new QCPAxisTickerDateTime);
     timeTicker->setDateTimeFormat("h:mm:ss.zzz");
     timeTicker->setDateTimeSpec(Qt::UTC);
     r->axis(QCPAxis::atBottom,0)->setTicker((timeTicker));

     // Make right axis visible to accommodate a panel title, but without ticks
     r->axis(QCPAxis::atRight,0)->setVisible(true);
     r->axis(QCPAxis::atRight,0)->setTicks(false);

     // Place the panel in the layout
     ui->plot->plotLayout()->addElement(i,0,r);

     // Only x-axis tick labels on bottom panel
     if(i<nPanels-1)
       r->axis(QCPAxis::atBottom,0)->setTickLabels(false); // Only tick labels on bottom axis

     // Default to axes not zoomable because they default to not selected
     r->setRangeZoomAxes(nullptr,nullptr);

     // Configure only bottom x-axis to be selectable and make other x-axes follow it
     r->axis(QCPAxis::atRight,0)->setSelectableParts(QCPAxis::spNone);
     if(i==nPanels-1){
       r->axis(QCPAxis::atBottom,0)->setSelectableParts(QCPAxis::spAxis);
       connect(r->axis(QCPAxis::atBottom,0),&QCPAxis::selectionChanged,this,&amjChartView::axisSelectionChanged); // who to call when axis selction changes
       for(int j=0;j<nPanels-1;j++)
         connect(r->axis(QCPAxis::atBottom,0),static_cast<void (QCPAxis::*)(const QCPRange &)>(&QCPAxis::rangeChanged),
                 ui->plot->axisRect(j)->axis(QCPAxis::atBottom,0),static_cast<void (QCPAxis::*)(const QCPRange &)>(&QCPAxis::setRange));
     }
     else
       r->axis(QCPAxis::atBottom,0)->setSelectableParts(QCPAxis::spNone);

     // Configure all y-axes to be selectable
     r->axis(QCPAxis::atLeft,0)->setSelectableParts(QCPAxis::spAxis);
     connect(r->axis(QCPAxis::atLeft,0),&QCPAxis::selectionChanged,this,&amjChartView::axisSelectionChanged); // who to call when axis selection changes

     // Assign the panel name
     if(i<_panelnames.size())
       r->axis(QCPAxis::atRight)->setLabel(_panelnames[i]);
     else
       r->axis(QCPAxis::atRight)->setLabel("");
  }

  // Update the number of graphs if necessary
  if(ui->plot->graphCount()>nGraphs){
    for(int i=ui->plot->graphCount()-1;i>=nGraphs;i--){
      std::cout << "removing " << i << std::endl;
      ui->plot->removeGraph(i);
    }
  }
  else if(nGraphs>ui->plot->graphCount()){
    for(int i=ui->plot->graphCount();i<nGraphs;i++){
      ui->plot->addGraph(nullptr,nullptr); // The graph is assigned axes further down
      ui->plot->graph(i)->setPen(QPen(_colors[i%_colors.size()]));
    }
  }

  // Assign names to graphs
  for(int i=0;i<ui->plot->graphCount();i++)
    ui->plot->graph(i)->setName(i<_graphnames.size()?_graphnames[i]:"");

  // Assign graphs to panels
  int j;
  for(int i=0;i<ui->plot->graphCount();i++){
    int nGraphsPerPanel=(int)ceil((double)ui->plot->graphCount()/(double)ui->plot->axisRectCount());
    j=i/nGraphsPerPanel;
    ui->plot->graph(i)->setKeyAxis(ui->plot->axisRect(j)->axis(QCPAxis::atBottom,0));
    ui->plot->graph(i)->setValueAxis(ui->plot->axisRect(j)->axis(QCPAxis::atLeft,0));
  }

  // Create the legend
  if(_showLegend){
    ui->plot->legend=new QCPLegend();
    ui->plot->legend->setFillOrder(QCPLayoutGrid::foColumnsFirst);
    ui->plot->legend->setWrap(_legendWrap);
    ui->plot->plotLayout()->addElement(ui->plot->axisRectCount(),0,ui->plot->legend);
    ui->plot->plotLayout()->setRowStretchFactor(ui->plot->axisRectCount(),0.00001);
    for(int i=0;i<_legend_graphs.size();i++)
      ui->plot->graph(_legend_graphs[i])->addToLegend();
    ui->plot->legend->setVisible(true);
  }

  // Replot
  _update();
  //  ui->plot->replot();
}

void amjChartView::axisSelectionChanged(const QCPAxis::SelectableParts &a){
  QCPAxisRect *r=ui->plot->axisRect(ui->plot->axisRectCount()-1);
  if(r->axis(QCPAxis::atBottom,0)==sender()){
    std::cout << "x-axis " << std::endl;
    if(a==QCPAxis::spNone)
      r->setRangeZoomAxes(nullptr,nullptr);
    //else
      //r->setRangeZoomAxes(r->axis(QCPAxis::atBottom,0),nullptr);
    return;
  }
  for(int i=0;i<ui->plot->axisRectCount();i++){
     r=ui->plot->axisRect(i);
    if(r->axis(QCPAxis::atLeft,0)==sender()){
      std::cout << "axis in panel " << i << " clicked" << std::endl;
      if(a==QCPAxis::spNone)
        r->setRangeZoomAxes(nullptr,nullptr);
      else
        r->setRangeZoomAxes(nullptr,r->axis(QCPAxis::atLeft,0));
      return;
    }
   }
}

void amjChartView::pauseClicked(bool checked){
  if(updateTimer.isActive()){
      updateTimer.stop();
      ui->toolButton_pause->setIcon(QIcon(":/icons/play"));
    }
  else{
      updateTimer.start();
      ui->toolButton_pause->setIcon(QIcon(":/icons/pause"));
    }
}

void amjChartView::wheelEvent(QWheelEvent *event){
  QCPAxis *a=ui->plot->axisRect(ui->plot->axisRectCount()-1)->axis(QCPAxis::atBottom,0);
  if(a->selectedParts()==QCPAxis::spAxis){
    event->accept();
    double oldtscale=tscale;
    tscale*=(1.0+(double)event->angleDelta().y()/128*_wheelFactor);
    if(updateTimer.isActive())
      return;
    double oldtref=tref;
    tref=a->pixelToCoord(event->position().x());
    t1=(t1+oldtref-tref)/oldtscale*tscale;
    t2=(t2+oldtref-tref)/oldtscale*tscale;
  }
  else{
    for(int i=0;i<ui->plot->axisRectCount();i++)
      if(ui->plot->axisRect(i)->axis(QCPAxis::atLeft,0)->selectedParts()==QCPAxis::spAxis)
        _autoscale[i]=false;
  }
  _update();
}

void amjChartView::resetClicked(bool){
  tscale=1;
  for(int i=0;i<_autoscale.size();i++)
    _autoscale[i]=true;
  _update();
}

