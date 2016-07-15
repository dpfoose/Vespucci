/*******************************************************************************
    Copyright (C) 2014-2016 Wright State University - All Rights Reserved
    Daniel P. Foose - Maintainer/Lead Developer

    This file is part of Vespucci.

    Vespucci is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Vespucci is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Vespucci.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/
#include "plotwidget.h"
#include "ui_plotwidget.h"

PlotWidget::PlotWidget(QWidget *parent, QSharedPointer<VespucciWorkspace> ws) :
    QWidget(parent),
    ui(new Ui::PlotWidget)
{
    workspace_ = ws;
    transient_graph_ = 0;
    ui->setupUi(this);
    ui->customPlot->setInteraction(QCP::iRangeDrag);
    ui->customPlot->setInteraction(QCP::iRangeZoom);
    ui->customPlot->setInteraction(QCP::iSelectItems);
    ui->customPlot->setInteraction(QCP::iMultiSelect);
    ui->customPlot->setInteraction(QCP::iSelectLegend);
    ui->customPlot->setInteraction(QCP::iSelectOther);
    ui->customPlot->setInteraction(QCP::iSelectPlottables);
    offset_by_ = 0;

    colors_.append(QColor(228,26,28));
    colors_.append(QColor(55,126,184));
    colors_.append(QColor(77,175,74));
    colors_.append(QColor(152,78,163));
    colors_.append(QColor(255,127,0));
    colors_.append(QColor(255,255,51));
    colors_.append(QColor(166,86,40));
    colors_.append(QColor(247,129,191));
    colors_.append(QColor(153,153,153));

    offset_plots_ = false;
}

PlotWidget::~PlotWidget()
{
    delete ui;
}

void PlotWidget::AddPlot(const mat & paired_data)
{
    if (paired_data.n_cols < 2)
        return;
    QVector<double> abscissa =
            QVector<double>::fromStdVector(conv_to<stdvec>::from(paired_data.col(0)));
    QVector<double> data =
            QVector<double>::fromStdVector(conv_to<stdvec>::from(paired_data.col(1)));
    int graph_count = ui->customPlot->graphCount();
    if (graph_count < 1){
        ui->customPlot->addGraph();
        ui->customPlot->graph(graph_count)->addData(abscissa, data);
        ui->customPlot->rescaleAxes();
        ui->customPlot->replot();
    }
    else{
        ui->customPlot->addGraph(ui->customPlot->graph(0)->keyAxis(), ui->customPlot->graph(0)->valueAxis());
        if (offset_plots_){
            RemoveOffset();
            DetermineOffset(data);
            ui->customPlot->graph(graph_count)->addData(abscissa, data);
            ApplyOffset();
            ui->customPlot->rescaleAxes();
            ui->customPlot->replot();
        }
        else{
            ui->customPlot->graph(graph_count)->addData(abscissa, data);
            ui->customPlot->rescaleAxes();
            ui->customPlot->replot();
        }

    }
}

void PlotWidget::AddPlot(const vec &abscissa, const vec &data)
{
    QVector<double> abscissa_qvec =
            QVector<double>::fromStdVector(conv_to<stdvec>::from(abscissa));
    QVector<double> data_qvec =
            QVector<double>::fromStdVector(conv_to<stdvec>::from(data));
    int graph_count = ui->customPlot->graphCount();
    if (graph_count < 1){
        ui->customPlot->addGraph();
        ui->customPlot->graph(graph_count)->addData(abscissa_qvec, data_qvec);
        ui->customPlot->rescaleAxes();
        ui->customPlot->replot();
    }
    else{
        ui->customPlot->addGraph(ui->customPlot->graph(0)->keyAxis(), ui->customPlot->graph(0)->valueAxis());
        if (offset_plots_){
            RemoveOffset();
            DetermineOffset(data_qvec);
            ui->customPlot->graph(graph_count)->addData(abscissa_qvec, data_qvec);
            ApplyOffset();
            ui->customPlot->rescaleAxes();
            ui->customPlot->replot();
        }
        else{
            ui->customPlot->graph(graph_count)->addData(abscissa_qvec, data_qvec);
            ui->customPlot->rescaleAxes();
            ui->customPlot->replot();
        }

    }
}

void PlotWidget::AddTransientPlot(const vec &abscissa, const vec &data)
{
    if (abscissa.n_rows != data.n_rows) return;

    if (transient_graph_){
        QVector<double> abscissa_qvec =
                QVector<double>::fromStdVector(conv_to<vector<double> >::from(abscissa));
        QVector<double> data_qvec =
                QVector<double>::fromStdVector(conv_to<vector<double> >::from(data));
        transient_graph_->setData(abscissa_qvec, data_qvec);
        ui->customPlot->rescaleAxes();
        ui->customPlot->replot();
    }
    else{
        QCPAxis *key_axis;
        QCPAxis *value_axis;
        if (ui->customPlot->graphCount()){
            key_axis = ui->customPlot->graph(0)->keyAxis();
            value_axis = ui->customPlot->graph(0)->valueAxis();
        }
        else{
            key_axis = 0;
            value_axis = 0;
        }
        ui->customPlot->addGraph(key_axis, value_axis);
        transient_graph_ = ui->customPlot->graph(ui->customPlot->graphCount() - 1);
        QVector<double> abscissa_qvec =
                QVector<double>::fromStdVector(conv_to<vector<double> >::from(abscissa));
        QVector<double> data_qvec =
                QVector<double>::fromStdVector(conv_to<vector<double> >::from(data));
        transient_graph_->addData(abscissa_qvec, data_qvec);
        ui->customPlot->rescaleAxes();
        ui->customPlot->replot();
    }

}

void PlotWidget::AddTransientPlot(const mat & paired_data)
{
    if (paired_data.n_cols != 2) return;

    QCPAxis *key_axis;
    QCPAxis *value_axis;
    if (transient_graph_){
        QVector<double> abscissa_qvec =
                QVector<double>::fromStdVector(conv_to<vector<double> >::from(paired_data.col(0)));
        QVector<double> data_qvec =
                QVector<double>::fromStdVector(conv_to<vector<double> >::from(paired_data.col(1)));
        transient_graph_->setData(abscissa_qvec, data_qvec);
        ui->customPlot->rescaleAxes();
        ui->customPlot->replot();
    }
    else{
        if (ui->customPlot->graphCount()){
            key_axis = ui->customPlot->graph(0)->keyAxis();
            value_axis = ui->customPlot->graph(0)->valueAxis();
        }
        else{
            key_axis = 0;
            value_axis = 0;
        }
        ui->customPlot->addGraph(key_axis, value_axis);
        transient_graph_ = ui->customPlot->graph(ui->customPlot->graphCount() - 1);
        QVector<double> abscissa_qvec =
                QVector<double>::fromStdVector(conv_to<vector<double> >::from(paired_data.col(0)));
        QVector<double> data_qvec =
                QVector<double>::fromStdVector(conv_to<vector<double> >::from(paired_data.col(1)));
        transient_graph_->addData(abscissa_qvec, data_qvec);
        ui->customPlot->rescaleAxes();
        ui->customPlot->replot();
    }
}

void PlotWidget::RemoveTransientPlot()
{
    if (transient_graph_){
        ui->customPlot->removeGraph(transient_graph_);
        transient_graph_ = 0;
        ui->customPlot->rescaleAxes();
        ui->customPlot->replot();
    }
}

void PlotWidget::StackPlots(bool stack)
{
    if (!ui->customPlot->graphCount()){
        offset_plots_ = true;
        return;
    }
    if (offset_plots_ && !stack){
        RemoveOffset();
    }
    if (!offset_plots_ && stack){
        RemoveOffset();
        DetermineOffset();
        ApplyOffset();
    }

    offset_plots_ = stack;
    ui->customPlot->rescaleAxes();
    ui->customPlot->replot();

}

bool PlotWidget::offset_plots() const
{
    return offset_plots_;
}

bool PlotWidget::TransientOnly() const
{
    return (transient_graph_ && ui->customPlot->graphCount() == 1);
}



void PlotWidget::DetermineOffset()
{
    if (!ui->customPlot->graphCount()){
        offset_by_ = 0;
        return;
    }
    stdvec first_y = GetData(ui->customPlot->graph(0)->data()->values()).toStdVector();
    double min = *min_element(first_y.begin(), first_y.end());
    double max = *max_element(first_y.begin(), first_y.end());
    for (int i = 1; i < ui->customPlot->graphCount(); ++i){
        stdvec y = GetData(ui->customPlot->graph(i)->data()->values()).toStdVector();
        double current_min = *min_element(y.begin(), y.end());
        double current_max = *max_element(y.begin(), y.end());
        min = current_min < min ? current_min : min;
        max = current_max > max ? current_max : max;
    }
    //increase offset by 10% to prevent graphs touching
    offset_by_ = 1.1*fabs(max - min);
}

void PlotWidget::DetermineOffset(const QVector<double> &new_data)
{
    //determine starting values for min and max
    stdvec new_y = new_data.toStdVector();
    double min = *min_element(new_y.begin(), new_y.end());
    double max = *max_element(new_y.begin(), new_y.end());

    for (int i = 0; i < ui->customPlot->graphCount(); ++i){
        stdvec y = GetData(ui->customPlot->graph(i)->data()->values()).toStdVector();
        double current_min = *min_element(y.begin(), y.end());
        double current_max = *max_element(y.begin(), y.end());
        min = std::min(current_min, min);
        max = std::max(current_max, max);
    }
    offset_by_ = 1.1*fabs(max - min);
}

void PlotWidget::ApplyOffset()
{
    for (int i = 0; i < ui->customPlot->graphCount(); ++i){
        QVector<double> data = GetData(ui->customPlot->graph(i)->data()->values());
        QVector<double> abscissa = GetAbscissa(ui->customPlot->graph(i)->data()->values());
        for (int i = 0; i < data.size(); ++i){data[i] = data[i] + offset_by_;}
        ui->customPlot->graph(i)->setData(abscissa, data);
        ui->customPlot->replot();
    }
}

void PlotWidget::RemoveOffset()
{
    for (int i = 0; i < ui->customPlot->graphCount(); ++i){
        QVector<double> data = GetData(ui->customPlot->graph(i)->data()->values());
        QVector<double> abscissa = GetAbscissa(ui->customPlot->graph(i)->data()->values());
        for (int i = 0; i < data.size(); ++i){data[i] = data[i] - offset_by_;}
        ui->customPlot->graph(i)->setData(abscissa, data);
        ui->customPlot->replot();
    }
}

QVector<double> PlotWidget::GetData(const QList<QCPData> &values) const
{
    QVector<double> data;
    for (auto value: values) data.push_back(value.value);
    return data;
}

QVector<double> PlotWidget::GetAbscissa(const QList<QCPData> &keys) const
{
    QVector<double> values;
    for (auto data: keys) values.push_back(data.key);
    return values;
}

QColor PlotWidget::GetNextColor()
{
    //there are nine colors in the list
    //we alternate between them then rotate back to the first
    int color_index = ui->customPlot->graphCount();
    while (color_index > 8){color_index -= 9;}
    return colors_[color_index];
}
