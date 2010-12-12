/*
 * Copyright (c) 2010 Greg Lonnon (greg.lonnon@gmail.com)
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "WorkoutEditor.h"


#include <QFileDialog>
#include <QDebug>
#include <Settings.h>
#include <QTextStream>

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <RideItem.h>
#include <Units.h>

// setup: initializes the form
void WorkoutEditor::setup()
{
    // set the ftp to be the current riders FTP
    ftpSpinBox->setValue(ftp);

    // wire up the widget...

    // if the Units change, make sure to change the colms
    connect(UnitButtonGroup,SIGNAL(buttonClicked(QAbstractButton *)),
            this, SLOT(unitsChanged(QAbstractButton *)));

    // if the WorkoutType changes, reset the workout segments and
    // update the colmun
    connect(WorkoutTypeButtonGroup, SIGNAL(buttonClicked(QAbstractButton *)),
            this, SLOT(workoutTypeChanged(QAbstractButton *)));

    // Update the Workout Metrics on every add/delete/edit to the Workout Segments
    connect(workoutTable,SIGNAL(cellChanged(int,int)),this,SLOT(cellChanged(int,int)));
    connect(addRowButton,SIGNAL(clicked()),this,SLOT(addRow()));
    connect(deleteRowButton,SIGNAL(clicked()),this,SLOT(deleteRow()));
    connect(insertRowButton,SIGNAL(clicked()),this,SLOT(insertRow()));
    connect(insertLapButton,SIGNAL(clicked()),this,SLOT(insertLap()));
    connect(workoutTable,SIGNAL(pressed),this,SLOT(tablePopupClicked));

    // on save/cancel/reset do the right thing...
    connect(SaveCancelButtonBox, SIGNAL(accepted()), this, SLOT(saveWorkout()));
    connect(SaveCancelButtonBox, SIGNAL(rejected()), this, SLOT(close()));
    connect((QObject*)SaveCancelButtonBox->button(QDialogButtonBox::Reset), SIGNAL(clicked()), this, SLOT(reset()));

    workoutCurve = new QwtPlotCurve("Watts");

    // add import to the SaveCancelButtonBox
    QPushButton *importButton = new QPushButton("Import");
    SaveCancelButtonBox->addButton(importButton,QDialogButtonBox::ActionRole);
    connect(importButton,SIGNAL(clicked()),this,SLOT(import()));
    update();

    connect(ftpSpinBox,SIGNAL(valueChanged(int)),this,SLOT(ftpChanged(int)));
}

void WorkoutEditor::addRow()
{
    workoutTable->insertRow(workoutTable->rowCount());
}

void WorkoutEditor::insertRow()
{
    workoutTable->insertRow(workoutTable->currentRow());
}

void WorkoutEditor::deleteRow()
{
    workoutTable->removeRow(workoutTable->currentRow());
    update();
}

void WorkoutEditor::ftpChanged(int x)
{
    ftp = x;
    update();
}

void WorkoutEditor::insertLap()
{
    workoutTable->insertRow(workoutTable->currentRow());
    QTableWidgetItem *lapItem = new QTableWidgetItem();
    lapItem->setText("LAP");
    lapItem->setFlags(Qt::ItemIsEnabled);
    workoutTable->setItem(workoutTable->currentRow(),0,lapItem);
    lapItem = new QTableWidgetItem();
    lapItem->setText("LAP");
    lapItem->setFlags(Qt::ItemIsEnabled);
    workoutTable->setItem(workoutTable->currentRow(),1,lapItem);
}

void WorkoutEditor::cellChanged(int row, int colm)
{
    QTableWidgetItem *item = workoutTable->item(row,colm);

    if(item->text().contains("LAP") || item->text() == "")
    {
        if(colm != 1)
        {
            item = workoutTable->item(row,1);
            if(item == NULL)
                item = new QTableWidgetItem();
            item->setText("");
        }
    }

    double d = item->text().toDouble();

    if(workoutType == WT_CRS)
    {
        // make sure the computrainer has a reasonable slope
        if(d < -15) d = -15;
        if(d > 15) d = 15;
    }
    else if(d < 0 ) d = 0;  // don't allow neg numbers
    QVariant data;
    data.setValue(d);
    item->setData(Qt::DisplayRole,data);
    update();
}

void WorkoutEditor::tablePopupClicked()
{
    QMenu *tablePopup = new QMenu(this);
    tablePopup->addMenu("Row");
     tablePopup->addMenu("Insert Row");
     tablePopup->addMenu("Delete Row");
     tablePopup->addMenu("Add Row");
     tablePopup->addMenu("Insert Lap Marker");

    tablePopup->exec(QCursor::pos());
}

void WorkoutEditor::saveWorkout()
{
    // get the filename
    boost::shared_ptr<QSettings> settings = GetApplicationSettings();
    QVariant workoutDir = settings->value(GC_WORKOUTDIR);

    QString filename = QFileDialog::getSaveFileName(this,QString("Save Workout"),
                                 workoutDir.toString(),"Computrainer Format *.erg *.crs *.mrc");
    // open the file
    QFile f(filename);
    f.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream stream(&f);
    // create the header
    stream << "[COURSE HEADER]" << endl;
    stream << "VERSION = 2" << endl;
    stream << "UNITS = ";
    if(useMetricUnits == false)
        stream << "ENGLISH";
    else
        stream << "METRIC";
    stream << endl;
    stream << "DESCRIPTION = " << descriptionText->text() << endl;
    stream << "FILE NAME = " << f.fileName() << endl;
    stream << "FTP = " << ftp << endl;
    if(workoutType == WT_ERG) {
        stream << "MINUTES WATTS" << endl;
    }
    else if(workoutType == WT_MRC)
    {
        stream << "MINUTES PERCENT" << endl;
    }
    else if(workoutType == WT_CRS)
    {
        stream << "DISTANCE GRADE WIND"  << endl;
    }
    stream << "[END COURSE HEADER]" << endl << endl;
    stream << "[COURSE DATA]" << endl;
    int numRows = workoutTable->rowCount();
    int row = 0;
    stream.setRealNumberPrecision(2);
    stream.setRealNumberNotation(QTextStream::FixedNotation);
    double currentX = 0;
    QString endStr = (workoutType == WT_MRC ? "%" : (workoutType == WT_CRS ? " 0" : ""));
    while(row < numRows)
    {
        QTableWidgetItem *colm1 = workoutTable->item(row,0);
        QTableWidgetItem *colm2 = workoutTable->item(row,1);

        if(colm1 && colm2)
        {
            if(colm1->text().contains("LAP"))
            {
                stream << "LAP" << endl;
                row++;
                continue;
            }
            double x = colm1->text().toDouble();
            double y = colm2->text().toDouble();
            if(workoutType != WT_CRS)
            {
                // start point
                stream << currentX << " ";
                stream << y << endStr << endl;
            }
            currentX += x;
            stream << currentX << " ";
            stream << y << endStr << endl;

        }

        row++;
    }
    stream << "[END COURSE DATA]" << endl;
}

void WorkoutEditor::reset()
{
    workoutTable->clearContents();
    workoutTable->setRowCount(2);
}

void WorkoutEditor::update()
{

    QStringList colms;
    // update colm headers
    if(workoutType != WT_CRS)
    {
        colms.append("Minutes");
    }
    else
    {
        if(!useMetricUnits)
            colms.append("Miles");
        else
            colms.append("KM");
    }

    if(workoutType == WT_CRS)
        colms.append("Slope");
    else if (workoutType == WT_MRC)
        colms.append("% of FTP");
    else if (workoutType == WT_ERG)
        colms.append("Watts");

    workoutTable->setHorizontalHeaderLabels(colms);
    workoutTable->horizontalHeader()->setVisible(true);

    if(workoutType == WT_CRS)
    {
        workoutCurve->setStyle(QwtPlotCurve::Lines);
        if(!useMetricUnits) {
              workoutPlot->setAxisTitle(QwtPlot::yLeft,"Elevation (feet)");
              workoutPlot->setAxisTitle(QwtPlot::xBottom,"Miles");
        }
        else
        {
              workoutPlot->setAxisTitle(QwtPlot::yLeft,"Elevation (meters)");
              workoutPlot->setAxisTitle(QwtPlot::xBottom,"KM");
        }
    }
    else
    {
        workoutCurve->setBaseline(0);
        workoutCurve->setStyle(QwtPlotCurve::Steps);
        workoutPlot->setAxisTitle(QwtPlot::yLeft,"Watts");
        workoutPlot->setAxisTitle(QwtPlot::xBottom,"Duration");
    }
    workoutPlot->setAxisAutoScale(QwtPlot::yLeft);

    // update the plot
    QVector<double> xData;
    QVector<double> yData;
    xData.append(0);
    yData.append(0);
    int row = 0;
    int numRows = workoutTable->rowCount();
    double TotalPowerTime = 0;
    double TotalTime = 0;
    double currentX = 0;
    double currentElevation = 0;
    double totalClimbed = 0;

    if(workoutType == WT_CRS)
        currentElevation = startAltitude * (useMetricUnits ? 1 : 1/METERS_PER_FOOT);

    while(row < numRows)
    {
        QTableWidgetItem *itemX = workoutTable->item(row,0);
        QTableWidgetItem *itemY = workoutTable->item(row,1);
        if(itemX && itemY)
        {
            double x = itemX->text().toDouble();
            double y = itemY->text().toDouble();

            if(workoutType == WT_MRC)
            {
                y = y * ftp /100; // if MRC, convert to abs power
            }
            else if(workoutType == WT_CRS)
            {
                y= x * (useMetricUnits ? 1000 : 5280) * y / 100;
                currentElevation += y;
                if(y > 0) totalClimbed += y;
                y = currentElevation;
            }
            currentX += x;
            xData.append(currentX);
            yData.append(y);
            TotalTime += x;
            TotalPowerTime += x * y;
        }
        row++;
    }
    QColor brush_color = QColor(124, 91, 31);
    brush_color.setAlpha(64);
    workoutCurve->setBrush(brush_color);
    workoutCurve->setData(xData,yData);
    workoutCurve->attach(workoutPlot);
    workoutPlot->replot();

    if(workoutType == WT_CRS)
    {
        QString distanceUnits = " Miles";
        QString climbUnits = " Feet";

        this->avgPowerOrElevationLabel->setText("Elevation");
        this->bikeScoreOrDistance->setText("Distance");
        if(useMetricUnits)
        {
            distanceUnits = " KM";
            climbUnits = " M";
        }
        // set the data...

        this->avgPowerLabel->setText(QString::number((int)totalClimbed) + climbUnits);
        this->kJouleLabel->setText("unknown");
        // BikeScore is easy...  We don't have to do the xPower, because power is constant
        this->bikeScoreLabel->setText(QString::number(TotalTime) + distanceUnits);
    }
    else
    {
       this->avgPowerLabel->setText(QString::number(TotalPowerTime/TotalTime,'f',2));
       this->kJouleLabel->setText(QString::number(TotalPowerTime * 60 /1000,'f',2));
       // BikeScore is easy...  We don't have to do the xPower, because power is constant
       int BikeScore = TotalPowerTime/(ftp *60)* 100;
       this->bikeScoreLabel->setText(QString::number(BikeScore));
    }
}

void WorkoutEditor::import()
{
    double prevAlt = 0;
    double alt = 0;
    double prevDistance = 0;
    double distance = 0;
    double startSmoothTime = 0;
    double smoothTime = 30;  // why 30 seconds...  I hate to have the load generator change every second...  30 sounded good...
    double joules = 0;
    double lastSec = 0;


    std::vector<std::pair<double,double> > rideData;

    // to make the ride smoother, let's average every 30 seconds...

    foreach(RideFilePoint *rfp, ride->ride()->dataPoints())
    {
        if(prevDistance == 0)
        {
            startAltitude = rfp->alt;
            prevDistance = rfp->km;
            prevAlt = rfp->alt;
            lastSec = startSmoothTime = rfp->secs;
            continue;
        }

        if(rfp->secs - startSmoothTime < smoothTime)
        {
            joules += rfp->watts * (rfp->secs - lastSec); // kJoules accumulator
            lastSec = rfp->secs;
            continue;
        }

        if(workoutType == WT_CRS)
        {
            // find out the slope.
            distance = rfp->km;
            alt = rfp->alt;
            double d = distance - prevDistance;
            double a = alt - prevAlt;
            double slope = a/(d *1000) * 100;  // slope is a percentage, need to convert to meters

            prevAlt = alt;
            prevDistance = distance;

            if(d < 0.0001) continue; // throw out too small of samples
            d = d / (useMetricUnits ? 1 : KM_PER_MILE);
            // make the ride, ridable...  max slope -8..8
            if(slope > 8) slope = 8;
            if(slope < -8) slope = -8;

            rideData.push_back(std::pair<double,double>(d,slope));

        }
        else
        {
            // use the smoothTime power avg.
            double avgPower = joules / (rfp->secs - startSmoothTime);
            double minutes = (rfp->secs - startSmoothTime) / 60;
            joules = 0;

            // make the ride, ridable... avgPower between ftp/2..2*ftp
            if(avgPower < ftp/2)
            {
                avgPower = ftp/2; // half of FTP is the min amount of power

            }

            if(avgPower > 2 * ftp)
            {
                avgPower = 2 *ftp;
            }

            if(workoutType == WT_ERG)
            {
                rideData.push_back((std::pair<double,double>(minutes,avgPower)));
            }
            else
            {
                // TODO: figure out the zones stuff to get the rider's ftp during the ride...
                double powerPercentage = avgPower / ftp * 100;
                rideData.push_back((std::pair<double,double>(minutes,powerPercentage)));
            }
        }
        startSmoothTime = rfp->secs;
    }
    disconnect(workoutTable,SIGNAL(cellChanged(int,int)),this,SLOT(cellChanged(int,int)));
    int row = 0;
    this->workoutTable->clearContents();
    workoutTable->setRowCount(rideData.size());

    for(std::vector<std::pair<double,double> >::const_iterator cur = rideData.begin();
        cur != rideData.end();
        ++cur)
    {
        QTableWidgetItem *item = new QTableWidgetItem(QString::number(cur->first));
        workoutTable->setItem(row,0,item);
        item = new QTableWidgetItem(QString::number(cur->second));
        workoutTable->setItem(row,1,item);
        item = workoutTable->item(row,0);
        row++;
    }
    connect(workoutTable,SIGNAL(cellChanged(int,int)),this,SLOT(cellChanged(int,int)));
    update();
}

void WorkoutEditor::unitsChanged(QAbstractButton *button)
{
    bool oldValue = useMetricUnits;

    if(button->text() == QString("English"))
        useMetricUnits = false;
    else
        useMetricUnits = true;

    if(oldValue != useMetricUnits)
        update();
}

void WorkoutEditor::workoutTypeChanged(QAbstractButton *button)
{
    WorkoutTypeEnum oldValue = workoutType;

    if(button->text().contains("Abs"))
        workoutType = WT_ERG;
    else if(button->text().contains("Per"))
        workoutType = WT_MRC;
    else
        workoutType = WT_CRS;
    if(oldValue != workoutType)
    {
        reset(); // the data is incorrect
        update();
    }
}
