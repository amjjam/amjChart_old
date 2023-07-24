#include "amjChart.H"

#include <QApplication>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  amjChartView w;
  w.show();
  return a.exec();
}
