#ifndef AMJCHARTVIEW_H
#define AMJCHARTVIEW_H

#include <QWidget>
#include <QTimer>
#include <QDateTime>
#include <QVector>

QT_BEGIN_NAMESPACE
namespace Ui { class amjChartView; }
QT_END_NAMESPACE

class amjChartView : public QWidget
{
  Q_OBJECT

public:
  amjChartView(QWidget *parent = nullptr);
  ~amjChartView();
  void ngraphs(int n);
  // update chart or set or cancel update interval. To update now call without parameter (does not affect an update timer)
  // called with parameer 0 stops the update timer. Called with paramter>0 in ms, sets update timer to that many ms
  void update(int T=-1){if(T<=0) updateTimer.stop(); else updateTimer.start(T);};
  void range(double,double t2=0); // range of time to plot, in seconds before current time
  void append(QDateTime t, QVector<double> d); // Append a data point
private:
  Ui::amjChartView *ui;
  QTimer updateTimer;
  double t1,t2; // time range in seconds since reference time
  void _update();
};
#endif // AMJCHARTVIEW_H
