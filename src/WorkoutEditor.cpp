#include "WorkoutEditor.h"

#include <iostream>

#include <QFileDialog>
#include <QDebug>

#include <boost/shared_array.hpp>
#include <Settings.h>
#include <QTextStream>

#include <qwt_plot.h>
#include <qwt_plot_curve.h>

#include <Zones.h>

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

    // on save/cancel/reset do the right thing...
    connect(SaveCancelButtonBox, SIGNAL(accepted()), this, SLOT(saveWorkout()));
    connect(SaveCancelButtonBox, SIGNAL(rejected()), this, SLOT(close()));
    connect((QObject*)SaveCancelButtonBox->button(QDialogButtonBox::Reset), SIGNAL(clicked()), this, SLOT(reset()));

    workoutCurve = new QwtPlotCurve("Watts");
    workoutCurve->setBaseline(0);
    workoutCurve->setStyle(QwtPlotCurve::Steps);
    update();
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
}

void WorkoutEditor::cellChanged(int row, int colm)
{
    QTableWidgetItem *item = workoutTable->item(row,colm);

    double d = item->text().toDouble();
    QVariant data;
    data.setValue(d);
    item->setData(Qt::DisplayRole,data);
    update();
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
    if(useMetricUnits == true)
        stream << "ENGLISH";
    else
        stream << "METRIC";
    stream << endl;
    stream << "DESCRIPTION = " << descriptionText->text();
    stream << "FILE NAME = " << filename << endl;
    stream << "FTP = " << 300 << endl;  // TODO: use the zones to get the current CP???
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
    while(row < numRows)
    {
        stream << workoutTable->item(row,0)->text();
        stream << " " <<  workoutTable->item(row,1)->text();
        if(workoutType == WT_MRC) stream << "%";
        else if(workoutType == WT_CRS) stream << " 0";
        stream << endl;
        row++;
    }
    stream << "[END COURSE DATA]" << endl;
}

void WorkoutEditor::reset()
{
    workoutTable->clearContents();
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
    workoutPlot->setAxisTitle(QwtPlot::xBottom,"Duration");
    workoutPlot->setAxisTitle(QwtPlot::yLeft,"Effort");
    // update the plot
    std::vector<std::pair<double,double> > workoutData;
    QVector<double> xData;
    QVector<double> yData;
    xData.append(0);
    yData.append(0);
    int row = 0;
    int numRows = workoutTable->rowCount();
    double TotalPowerTime = 0;
    double TotalTime = 0;
    double currentX = 0;

    while(row < numRows)
    {
        QTableWidgetItem *itemX = workoutTable->item(row,0);
        QTableWidgetItem *itemY = workoutTable->item(row,1);
        if(itemX && itemY)
        {
            double x = itemX->text().toDouble();
            double y = itemY->text().toDouble();
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
    workoutCurve->setBrush(brush_color);   // fill below the line

    workoutCurve->setData(xData,yData);
    workoutCurve->attach(workoutPlot);
    workoutPlot->replot();

    this->avgPowerLabel->setText(QString::number(TotalPowerTime/TotalTime,'f',2));
    this->kJouleLabel->setText(QString::number(TotalPowerTime * 60 /1000,'f',2));
    // BikeScore is easy...  avgpower/ftp * minutes /60 * 100
    int BikeScore = TotalPowerTime/(ftp *60)* 100;
    this->bikeScoreLabel->setText(QString::number(BikeScore));
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
