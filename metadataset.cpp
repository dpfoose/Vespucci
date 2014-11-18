/*******************************************************************************
    Copyright (C) 2014 Wright State University - All Rights Reserved
    Daniel P. Foose - Author

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
#include "metadataset.h"


///
/// \brief MetaDataset::MetaDataset
/// \param name Name of this dataset as displayed to user
/// \param main_window Main window of the program
/// \param log_file The log file
/// \param directory The current global working directory
/// \param method_description The description of the method used
/// \param endmember_selection The string used by the user to select endmembers
/// \param method An enum specifying the method
/// \param parent_datasets The datasets from which this dataset is extracted
///
MetaDataset::MetaDataset(QString name,
                         MainWindow *main_window,
                         QFile *log_file,
                         QString *directory,
                         QString method_description,
                         MetaMethod::Method method,
                         QList<QSharedPointer<VespucciDataset> > parent_datasets)
    : VespucciDataset(name, main_window, directory, log_file)
{
    parent_datasets_ = parent_datasets;

    if(!ParentsValid())
        throw std::runtime_error("Improper input to MetaDataset constructor");


    QDateTime datetime = QDateTime::currentDateTimeUtc();
    log_stream_ << "Dataset " << name << "created "
                << datetime.date().toString("yyyy-MM-dd") << "T"
                << datetime.time().toString("hh:mm:ss") << "Z" << endl;
    log_stream_ << "Created from previous datasets:" << endl;
    for(int i = 0; i < parent_datasets_.size(); ++i){
        log_stream_ << "  " << parent_datasets_[i]->name() << endl;
    }

    log_stream_ << endl;

    method_ = method;
    method_description_ = method_description;
    mat spectra;
    rowvec wavelength = parent_datasets_[0]->wavelength();

    switch(method_) {
    case MetaMethod::AverageSpectra :
        try{
            spectra = ProcessAverage();
        }
        catch(std::exception e){
            throw std::runtime_error("MetaDataset::ProcessAverage");
        }

        break;
    case MetaMethod::ConcatenateDatasets :
        try{
            spectra = Concatenate();
        }
        catch(std::exception e){
            throw std::runtime_error("MetaDataset::Concatenate");
        }
        break;
    default:
        throw std::runtime_error("Improper input to MetaDataset");
    }
    //we don't preserve spatal data
    colvec x = ones(spectra.n_rows);
    colvec y = ones(spectra.n_rows);
    try{
        SetData(spectra, wavelength, x, y);
    }
    catch(std::exception e){
        throw std::runtime_error("Failure to set data in MetaDataset constructor");
    }
}


///
/// \brief MetaDataset::ProcessAverage
/// \return
/// Create a matrix with the average spectrum of each parent dataset on each row
mat MetaDataset::ProcessAverage()
{
    mat spectra(parent_datasets_.size(), parent_datasets_[0]->wavelength().n_elem);

    for (int i = 0; i < parent_datasets_.size(); ++i)
        spectra.row(i) = parent_datasets_[0]->AverageSpectrum(false);

    return spectra;
}

///
/// \brief MetaDataset::Concatenate
/// \return
/// Create a matrix with one dataset after another
mat MetaDataset::Concatenate()
{
    uword num_rows = 0;
    for (int i = 0; i < parent_datasets_.size(); ++i)
        num_rows += parent_datasets_[i]->spectra_ptr()->n_rows;
    mat spectra(num_rows, parent_datasets_[0]->wavelength_ptr()->n_elem);\
    uword start_index = 0;
    uword diff;
    uword end_index = parent_datasets_[0]->spectra_ptr()->n_rows - 1;
    for (int i = 0; i < parent_datasets_.size(); ++i){
        spectra.rows(start_index, end_index) = parent_datasets_[i]->spectra();
        diff = end_index - start_index;
        start_index += diff;
        end_index += diff;
    }

    return spectra;

}

///
/// \brief MetaDataset::ParentsValid
/// \return
/// Checks the validity of the parent datasets to make sure that they have the
/// same (or at least compatible) spectral resolution.
bool MetaDataset::ParentsValid()
{
    uword size = parent_datasets_[0]->wavelength_ptr()->n_elem;
    for (int i = 0; i<parent_datasets_.size(); ++i)
        if (parent_datasets_[i]->wavelength_ptr()->n_elem != size)
            return false;

    return true;
}
