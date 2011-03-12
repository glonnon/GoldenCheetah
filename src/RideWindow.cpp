#include "RideWindow.h"


class RealtimeData : QObject
{

public:
    Q_PROPERTY(int HR )
};

RideWindow::RideWindow(MainWindow * parent, TrainTool *_trainTool, const QDir &dir, RealtimeWindow *_realtimeWindow) :
    QWidget(parent)
{
    setInstanceName("Ride Window");
    trainTool = _trainTool;
    realtimeWindow = _realtimeWindow;
    view = new QWebView();
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(view);
    setLayout(layout);

}
