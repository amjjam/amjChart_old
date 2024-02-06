#ifndef AMJCHARTVIEW_H
#define AMJCHARTVIEW_H

#include <QWidget>
#include <QTimer>
#include <QDateTime>
#include <QVector>
#include <qcustomplot.h>

QT_BEGIN_NAMESPACE
namespace Ui { class amjChartView; }
QT_END_NAMESPACE

class amjChartView : public QWidget
{
  Q_OBJECT

public:
  amjChartView(QWidget *parent = nullptr);
  ~amjChartView();
  void clear(); // Erase graphs and panels
  void npanels(int); // Number of stacked plot panels
  void ngraphs(int n); // Number of graphs in the plot, total across all panels
  int ngraphs();
  void colors(QVector<QColor> colors){_colors=colors;};
  void fill();
  void panelnames(std::vector<std::string> &);
  void ytitles(std::vector<std::string> &);
  void graphnames(std::vector<std::string> &);
  // update chart or set or cancel update interval. To update now call without parameter (does not affect an update timer)
  // called with parameer 0 stops the update timer. Called with paramter>0 in ms, sets update timer to that many ms
  void update(int T=-1){if(T<=0) updateTimer.stop(); else updateTimer.start(T);};
  void range(double,double t2=0); // range of time to plot, in seconds before current time
  void append(QDateTime t, QVector<double> d); // Append a data point
  void legendList(std::vector<int>); // Place legend at bottom for graphs in list
  void legendShow(bool s=true); // If true show the legend
  void legendWrap(int); // Max number of legend items in a row. Default is 5.
  void wheelFactor(double w){_wheelFactor=w;}; // fractional change in time interval with each wheel click (0.05 default). Make it negative to scroll opposite.
  void toolbarShow(bool);
  void updateInterval(int); // sets the run update interval in ms. Default is 100.
  void run(bool); // true run the display and update time, false do not. Default is true.
private:
  Ui::amjChartView *ui;
  QTimer updateTimer;
  double t1d,t2d; // default time range in seconds since reference time
  double tscale; // scale factor of time-axis. Extent will be (t2default-t1default)*timeaxisscale
  double tref; // UTC time since epoch in seconds
  double t1,t2; // current time range
  QVector<QColor> _colors;
  int nPanels;
  QVector<bool> _autoscale;
  QVector<QString> _panelnames;
  int nGraphs;
  QVector<QString> _graphnames;
  bool _showLegend; // true to show the legend
  int _legendWrap; // Number of legen items in a row
  QList<int> _legend_graphs; // List of graphs to put in the legend
  double _wheelFactor;
  int _updateInterval;
  void _update();
  void format();
  void distribute_graphs();
  virtual void wheelEvent(QWheelEvent *);
  void axisSelectionChanged(const QCPAxis::SelectableParts &);
  void pauseClicked(bool);
  void resetClicked(bool); // Set axis scales to their defaults
};
#endif // AMJCHARTVIEW_H
