"use strict;"


// waits until everything is loaded until we start...
$(document).ready(function()
                  {
                      initialize();
                  });

// reduce the data
var subSample = 10;
var useMetric = true;
function displayKmOrMiles(d)
{
    if(useMetric == false)
        return 1.609344 * d;
    return d;
}
function displayMetersOrFeet(a)
{
    if(useMetric == false)
        return 0.3048 * distance;
    return distance;
}

var rider;

function initialize() 
{
    //    $.jqplot.config.enablePlugins = true;
    rider = new Rider(ride);
    rider.setTime(5);
    mapInit();
    streetInit();
    //    rideChartInit();

    animateLoop(0);
}

var ridePolyLine;
var map;
var positionMarker;
var mapBounds;
var curSample;


function mapInit()
{
    curSample = rider.curSample;
    var mapOptions = {
        mapTypeId: google.maps.MapTypeId.ROADMAP
    };
    map = new google.maps.Map(document.getElementById("map"),mapOptions);
    retArray = rider.route;
    routePolyline = retArray[0];
    mapBounds = retArray[1];
    routePolyline.setMap(map);
    map.fitBounds(mapBounds);
    positionMarker = new google.maps.Marker();
    positionMarker.setMap(map);
}

function mapUpdate(rider)
{
    var sample = rider.curSample;

    lng = sample.LON;
    lat = sample.LAT;
    if(lng == 0) return;
    if(lat == 0) return;
    var point = new google.maps.LatLng(lat,lng);
    var bound = map.getBounds();
    if(bound != null)
    {
        if(bound.contains(point) == false)
        {
            map.panTo(point);
        }
    }
    positionMarker.setPosition(point);

}

var streetOptions = {
    addressControl : false,
    panControl : false,
    linksControl : false,
    scrollwheel : false,
    zoomControl : false
};

var street;
function streetInit()
{
    street = new google.maps.StreetViewPanorama(document.getElementById("streetview"),streetOptions);
}

function streetUpdate(rider)
{
    var point = new google.maps.LatLng(rider.curSample.LAT,rider.curSample.LON);
    var h = rider.computeHeading();
    street.setPov({ heading: h, pitch: 10, zoom: 1 });
    street.setPosition(point);
    street.setVisible(true);
}


// this leaks...  UGH!!!!
/*
var rideChart;
var rideArray;
function rideChartInit()
{
    return;
    var minHeight = 10000;
    var maxHeight = 0;
    var endDistance;
    var maxDistance = 0;

    var altArray = new Array();
    for(var i = 0; i < rider.ride.RIDE.SAMPLES.length; i ++)
    {
        var sample = rider.ride.RIDE.SAMPLES[i];
        maxDistance = sample.KM;
        maxHeight = Math.max(maxHeight,sample.ALT);
        minHeight = Math.min(minHeight, sample.ALT);
        altArray.push([sample.KM,sample.ALT]);
    }

    rideArray = new Array();
    rideArray.push ( [ rider.ride.RIDE.SAMPLES[0], rider.ride.RIDE.SAMPLES[0] ] );
    plotOptions =
        {
            title :'Alittude',
            seriesDefaults :
            {
                fill : true,
                showMarker : false
            },
            series: [ {color: '#f5f5dc' }, { color: 'B22222' } ],

            axes:
            {
                xaxis: { min: 0, max: maxDistance, pad: 1.2 },
                yaxis: { min: minHeight, max: maxHeight, pad: 1.2 }
            }

    }

    rideChart = $.jqplot('chart',[altArray,rideArray],plotOptions);
    rideArray = new Array();
}
*/

/*
function metricsUpdate(sampleIndex)
{
    return;
    var curSample = rider.ride.RIDE.SAMPLES[sampleIndex];

    var startIndex = Math.max(0,sampleIndex - 10);
    rideArray.length = 0;
    for(var i = startIndex; i < sampleIndex; i++)
    {
        rideArray.push([rider.ride.RIDE.SAMPLES[i].KM,
                        rider.ride.RIDE.SAMPLES[i].ALT]);
    }
    rideChart.series[1].data = rideArray;
    $('chart').empty();
    rideChart.replot();
    return;

    var html = "<p>Watts: " + curSample.WATTS + "</p>";
    html = html +  "<p>Cadence: " + curSample.CAD + "</p>";
    html = html +  "<p>Heart Rate: " + curSample.HR + "</p>";
    html = html +  "<p>Speed: " + curSample.KPH + "</p>";
    html = html +  "<p>Alt: " + curSample.ALT + "</p>";
    document.getElementById('ridedata').innerHTML = html;
}
*/

// time between updates
var period = 100;
var sampleInterval = 4;
var streetRate = 100;
var metricsRate = 100 ;
var mapRate = period;
// event loop drives the animation
function animateLoop(eventCount)
{

    var time;
    if(typeof(GCRider) != "undefined")
    {
        // we are running inside GC
        time= GCRider.time;
    }
    else
    {
        // we are running inside a browser (faster development)
        time= sampleInterval * eventCount;
    }

    rider.setTime(time);
    var done = rider.isDone();

    if(eventCount * period % mapRate == 0)
    {
        mapUpdate(rider);
    }
    if(eventCount * period % streetRate == 0)
    {
        streetUpdate(rider);
    }
    if(eventCount * period % metricsRate == 0)
    {
        // metricsUpdate(sampleIndex);
    }
    eventCount = eventCount + 1;

    if(done != true)
    {
        setTimeout("animateLoop(" + eventCount+ ")",period);
    }
}




