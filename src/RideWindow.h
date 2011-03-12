#ifndef RIDEWINDOW_H
#define RIDEWINDOW_H

#include <QWidget>
#include <QWebView>

#include <QDebug>

#include "TrainTool.h"
#include "RealtimeWindow.h"
#include "RealtimeController.h"

class RideWindow : public QWidget
{
    Q_OBJECT

    QWebView *view;
    TrainTool *trainTool;
    RealtimeWindow *realtimeWindow;
    RealtimeController *deviceController;


public:
    explicit RideWindow(MainWindow *, TrainTool *, const QDir &, RealtimeWindow *);

signals:

public slots:

};

#endif // RIDEWINDOW_H
