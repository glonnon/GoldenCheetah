#include <QWebPage>
#include <QWebFrame>


#include "RideWindow.h"

#include "RealtimeData.h"

#include "JsonRideFile.h"

#include <QWebInspector>

extern RideItem *hacktogetrideitem;
void RideWindow::showEvent(QShowEvent *)
{
    qDebug() << "show event is called";
    ride = myRideItem;

    if(ride == NULL)
    {
        loadNoRide();
        return;
    }
    if(rideLoaded) return;
    loadRide(ride);
   this->rideItem();

}

void RideWindow::hideEvent(QHideEvent *)
{
    qDebug() << "hide event is called";

}

void RideWindow::loadNoRide()
{


}


// this will delete the current file at new
bool copyFile(const QString &oldPath, const QString &newPath)
{
    if(QFile::exists(newPath))
    {
        QFile::remove(newPath);
    }
    bool rc = QFile::copy(oldPath,newPath);
    return rc;
}

// this is called on every page load in the web browser
void RideWindow::addJSObjects()
{
    qDebug() << "page was reloaded...";
    view->page()->mainFrame()->addToJavaScriptWindowObject("GCRider",rider);
}

void RideWindow::loadRide(RideItem *r)
{
    QWebSettings::globalSettings()->setAttribute(QWebSettings::PluginsEnabled, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::JavascriptEnabled, true);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);

    QWebPage *page = view->page();
    QWebFrame *frame = page->mainFrame();

    // Signal is emitted before frame loads any web content:
    QObject::connect(frame, SIGNAL(javaScriptWindowObjectCleared()),
                            this, SLOT(addJSObjects()));


    view->setPage(page);

    // used for testing...
    TestRider* tr = new TestRider();  // this is for testing the javascript code (only..)
    tr->setRide(r);
    rider = tr;
    addJSObjects();

    // load the ridewindow page
    // need to copy it out of the resource to the temp directory
    // this is needed, so we can see the ride javascript file
    // in the future, we should proxy the ride file over...
    bool flag;
    QString filePath = QDir::tempPath() + QDir::separator() + "RideWindow.html";
    flag = copyFile(":/ride/web/RideWindow.html",filePath);

    QString tmpPath =  QDir::tempPath() + QDir::separator() + "ride.js";
    flag = copyFile(":/ride/web/ride.js",tmpPath);

    tmpPath =  QDir::tempPath() + QDir::separator() + "jquery-1.5.1.min.js";
    flag = copyFile(":/ride/web/jquery-1.5.1.min.js",tmpPath);

    tmpPath =  QDir::tempPath() + QDir::separator() + "jquery.jqplot.min.js";
    flag = copyFile(":/ride/web/jquery.jqplot.min.js",tmpPath);

    tmpPath =  QDir::tempPath() + QDir::separator() + "Rider.js";
    flag = copyFile(":/ride/web/Rider.js",tmpPath);


    JsonFileReader jfr;
    QString jsPath =  QDir::tempPath() + QDir::separator() + "ridedata.js";
    QFile file(jsPath);
    jfr.writeRideFile((const RideFile *)ride->ride(),file,true);

    frame->load(QUrl(filePath));

    view->page()->mainFrame()->addToJavaScriptWindowObject("RealtimeRider",rider);

    // put the QWebInspector on the page...
    QWebInspector * qwi = new QWebInspector();
    qwi->setPage(page);
    qwi->setVisible(true);
}

RideWindow::RideWindow(MainWindow * main) :
    GcWindow(main),
    rideLoaded(false)
{
    setInstanceName("Ride Window");
    //    trainTool = _trainTool;
    //    realtimeWindow = _realtimeWindow;
    view = new QWebView();
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(view);
    setLayout(layout);
}
