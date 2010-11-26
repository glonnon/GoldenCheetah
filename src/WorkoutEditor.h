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

#include "ui_WorkoutEditor.h"

class WorkoutEditor : public QWidget, public Ui::WorkoutEditor
{
    Q_OBJECT

 protected:
    // data
    bool isEnglish;
    enum WorkoutTypeEnum { WT_ERG, WT_MRC, WT_CRS };
    WorkoutTypeEnum workoutType;

    // methods
    void setup();
    void update();


 public:
    WorkoutEditor(QWidget *parent = NULL) :QWidget(parent)
    {
        setupUi(this);
        setup();
        isEnglish = true;
        workoutType = WT_ERG;

    }
 public slots:
    void unitsChanged(QAbstractButton *button);
    void workoutTypeChanged(QAbstractButton *button);
    void saveWorkout();
    void reset();
    void validateCell(QTableWidgetItem *,QTableWidgetItem *);
    void addRow();
    void insertRow();
    void deleteRow();


};

#endif // _GC_COMPUTRAINER3DP_H