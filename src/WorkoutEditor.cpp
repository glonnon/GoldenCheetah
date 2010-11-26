#include "WorkoutEditor.h"

#include <iostream>

#include <QFileDialog>
#include <QDebug>

#include <boost/shared_array.hpp>
#include <Settings.h>
#include <QTextStream>

#include <qwt_plot.h>
#include <qwt_plot_curve.h>



class WorkoutItem : public QTableWidgetItem
{
public:
    void setData(int role, const QVariant &value)
    {
        QTableWidgetItem::setData(role,value);
    }
    WorkoutItem *clone()
    {
        qDebug() << "cloning";
        return new WorkoutItem();
    }

};

void WorkoutEditor::setup()
{
    // wire up the widget...  I think this can be done in Designer, but I
    // am a Designer noob...

    // if the Units change, make sure to change the colms
    connect(UnitButtonGroup,SIGNAL(buttonClicked(QAbstractButton *)),
            this, SLOT(unitsChanged(QAbstractButton *)));

    // if the WorkoutType changes, reset the workout segments and
    // update the colmun
    connect(WorkoutTypeButtonGroup, SIGNAL(buttonClicked(QAbstractButton *)),
            this, SLOT(workoutTypeChanged(QAbstractButton *)));

    // Update the Workout Metrics on every add/delete to the Workout Segments
    //connect(workoutTable,SIGNAL(currentItemChanged(QTableWidgetItem *,QTableWidgetItem *)),this,SLOT(validateCell(QTableWidgetItem *, QTableWidgetItem *)));

    // update the Plot on every add/delete to the workout Segments
    workoutTable->setItemPrototype(new WorkoutItem());

    connect(addRowButton,SIGNAL(clicked()),this,SLOT(addRow()));
    connect(deleteRowButton,SIGNAL(clicked()),this,SLOT(deleteRow()));

    connect(insertRowButton,SIGNAL(clicked()),this,SLOT(insertRow()));

    // on save/cancel/reset do the right thing...
    connect(SaveCancelButtonBox, SIGNAL(accepted()), this, SLOT(saveWorkout()));
    connect(SaveCancelButtonBox, SIGNAL(rejected()), this, SLOT(close()));
    connect((QObject*)SaveCancelButtonBox->button(QDialogButtonBox::Reset), SIGNAL(clicked()), this, SLOT(reset()));



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

void WorkoutEditor::validateCell(QTableWidgetItem *cur , QTableWidgetItem *prev)
{
    bool ok;
    cur->data(Qt::EditRole).toDouble(&ok);
    if(!ok)
    {
        qDebug() << "invalid data";
    }
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
    if(isEnglish == true)
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
    std::cout << "update " << workoutType << ":" << isEnglish << std::endl;
    QStringList colms;


    // update colm headers
    if(workoutType != WT_CRS)
    {
        colms.append("Minutes");
    }
    else
    {
        if(isEnglish)
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

    QwtPlotCurve *workoutCurve = new QwtPlotCurve("Watts");
    QVector<double> xData;
    QVector<double> yData;
    xData.append(0);
    yData.append(0);
    int row = 0;
    int numRows = workoutTable->rowCount();
    double currentX = 0;

    while(row < numRows)
    {
        QTableWidgetItem *item = workoutTable->item(row,0);

        if(item)
        {
            xData.append(currentX);
            currentX += item->text().toDouble();
            xData.append(currentX);
        }

        item = workoutTable->item(row,1);
        if(item)
        {
            double y = item->text().toDouble();
            yData.append(y);
            yData.append(y);
        }
        row++;
    }
    xData.append(0);
    yData.append(0);
    QColor brush_color = QColor(124, 91, 31);
    brush_color.setAlpha(64);
    workoutCurve->setBrush(brush_color);   // fill below the line

    workoutCurve->setData(xData,yData);
    workoutCurve->attach(workoutPlot);
    workoutPlot->replot();
}

void WorkoutEditor::unitsChanged(QAbstractButton *button)
{
    bool oldValue = isEnglish;

    if(button->text() == QString("English"))
        isEnglish = true;
    else
        isEnglish = false;

    if(oldValue != isEnglish)
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
        update();
}
