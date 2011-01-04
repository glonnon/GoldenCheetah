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

#ifndef _GC_WORKOUTEDITOR_H
#define _GC_WORKOUTEDITOR_H

#include <QWidget>
class QwtPlotCurve;
class RideItem;

#include "ui_WorkoutEditor.h"

class WorkoutEditor : public QWidget, public Ui::WorkoutEditor
{
    Q_OBJECT

 protected:
    // data
    int ftp;
    bool useMetricUnits;
    enum WorkoutTypeEnum { WT_ERG, WT_MRC, WT_CRS };
    WorkoutTypeEnum workoutType;
    QwtPlotCurve *workoutCurve;
    RideItem *ride;
    double startAltitude;

    // methods
    void setup();
    void update();

 public:
    WorkoutEditor(QWidget *parent, int _ftp, RideItem *_ride) :QWidget(parent)
    {
        useMetricUnits = false;
        workoutType = WT_ERG;
        ftp = _ftp;
        ride = _ride;
        setupUi(this);
        setup();
    }

 public slots:
    void unitsChanged(QAbstractButton *button);
    void workoutTypeChanged(QAbstractButton *button);
    void saveWorkout();
    void reset();
    void cellChanged(int,int);
    void addRow();
    void insertRow();
    void deleteRow();
    void import();
    void tablePopupClicked();
    void insertLap();
    void ftpChanged(int x);
};

class Workout
{
public:
    double ftp;
    std::vector<std::pair<double,double> > Data;
};

#include <QWizard>
#include <QRadioButton>
#include <QButtonGroup>

class RowValidator
{
public:
    bool Validate(QVector<QTableWidgetItem> row);
};

class WorkoutEditor2 : public QTableWidget
{
    RowValidator *rv;
public:
    WorkoutEditor2(QStringList &colms, RowValidator *rv_ = NULL)
    {
        setHorizontalHeaderLabels(colms);
        setColumnCount(colms.count());
        horizontalHeader()->setVisible(true);
        setShowGrid(true);

    }


};

class WorkoutGraph
{


};

class WorkoutTypePage : public QWizardPage
{
     Q_OBJECT

    QButtonGroup *buttonGroupBox;
    QRadioButton *absWattageRadioButton, *relWattageRadioButton, *gradientRadioButton, *importRadioButton;
public:
    WorkoutTypePage(QWidget *parent = NULL);
    int nextId() const;
   bool isComplete() const { return true; }
};
class AbsWattagePage : public QWizardPage
{
 Q_OBJECT
    WorkoutEditor2 *we;
public:
    AbsWattagePage(QWidget *parent = NULL) : QWizardPage(parent)
    {
        setTitle("Workout Wizard");
        setSubTitle("Absolute Wattage Workout Creator");
        QLayout *layout = new QVBoxLayout();
        QStringList colms;
        colms.append(tr("Minutes"));
        colms.append(tr("Wattage"));
        we = new WorkoutEditor2(colms);
        layout->addWidget(we);
        this->setLayout(layout);
    }
       bool isFinalPage() const { return true; }
};
class RelWattagePage : public QWizardPage
{
 Q_OBJECT
public:
    RelWattagePage() {
        setTitle("Workout Wizard");
        setSubTitle("% of FTP Workout Creator");
    }
    bool isFinalPage() const { return true; }
    bool nextId()  { return -1; }
};
class GradientPage : public QWizardPage
{
     Q_OBJECT
public:
    GradientPage()
    {
        setTitle("Workout Wizard");
        setSubTitle("Manually crate a workout based on gradient (slope) and distance");
    }
    bool isFinalPage() const { return true; }
};

class ImportPage : public QWizardPage
{
    Q_OBJECT
public:
    ImportPage()
    {
        setTitle("Workout Wizard");
        setSubTitle("Import current ride as a Gradient Ride (slope based)");
        setFinalPage(true);
    }
    bool isFinalPage() const { return true; }
};


#include <QWizard>

class WorkoutWizard : public QWizard
{
        Q_OBJECT
public:
    enum { WW_WorkoutTypePage, WW_AbsWattagePage, WW_RelWattagePage, WW_GradientPage, WW_ImportPage };

    WorkoutWizard(QWidget *parent = NULL);

    // called at the end of the wizard...
    void accept();
};



#endif // _GC_WORKOUTEDITOR_H
