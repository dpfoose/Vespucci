#ifndef PLOTVIEWER_H
#define PLOTVIEWER_H

#include <QDialog>
#include "qcustomplot.h"
#include <mlpack/core.hpp>
#include <GUI/Display/plotwidget.h>
#include <GUI/mainwindow.h>

namespace Ui {
class PlotViewer;
}
using namespace std;
using namespace arma;
class MainWindow;
class PlotViewer : public QDockWidget
{
    Q_OBJECT

public:
    explicit PlotViewer(MainWindow *parent, QSharedPointer<VespucciWorkspace> workspace);
    ~PlotViewer();
    void AddPlot(const mat& paired_data, const QString &tab_title);
    void AddPlot(const vec& abscissa, const vec& data, const QString &tab_title);
    void AddTransientPlot(const vec &abscissa, const vec &data, const QString &tab_title);
    void AddTransientPlot(const mat & paired_data, const QString &tab_title);
    void AddScatterPlot(const mat &paired_data, const QString &tab_title);
    void AddScatterPlot(const vec &abscissa, const vec &data, const QString &tab_title);
    void AddTab(const QString &tab_title);
    void SetHoldCheckBoxChecked(bool checked);
    void CloseTransientTab();
signals:
    void SetActionChecked(bool checked);
protected:
    void closeEvent(QCloseEvent *ev);
public slots:
private slots:

    void on_tabWidget_tabCloseRequested(int index);


    void on_exportPushButton_clicked();

private:
    Ui::PlotViewer *ui;
    QSharedPointer<VespucciWorkspace> workspace_;
};

#endif // PLOTVIEWER_H
