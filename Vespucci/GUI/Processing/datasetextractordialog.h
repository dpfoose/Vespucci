/*******************************************************************************
    Copyright (C) 2014-2017 Wright State University - All Rights Reserved
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
#ifndef DATASETEXTRACTORDIALOG_H
#define DATASETEXTRACTORDIALOG_H

#include <QDialog>
#include "Global/vespucciworkspace.h"

namespace Ui {
class DatasetExtractorDialog;
}

class DatasetExtractorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DatasetExtractorDialog(QWidget *parent, QSharedPointer<VespucciWorkspace> ws, const QStringList &matrix_keys);
    ~DatasetExtractorDialog();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::DatasetExtractorDialog *ui;
    QSharedPointer<VespucciWorkspace> workspace_;
    QStringList matrix_keys_;
};

#endif // DATASETEXTRACTORDIALOG_H
