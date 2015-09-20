/*
 * TCAT app
 * By: Ryan Dong, Michael Huang, and Alan Wu
 */

var NUM_STOPS = 5;
var NUM_ROUTES = 10;
var STOP_URL = 'http://cornelldata.org/api/v0/TCAT-data/stop-locations';
var STOP_SCHED = 'http://cornelldata.org/api/v0/TCAT-data/stop-schedules';
var stop_data;
var stop_schedules_data;

function latLongDist(lat1, long1, lat2, long2){
  return (lat1-lat2)*(lat1-lat2) + (long1-long2)*(long1-long2);
}

function convertTime(s) {
    var colonInd = s.indexOf(":");
    var hours = +s.slice(0,colonInd);
    var minutes = +s.slice(colonInd+1,colonInd+3);
    var PM = 0;
    if (s.slice(colonInd+3) == "PM") {
        PM = 12;
    }
    return (hours+PM)*60 + minutes;
}

function loadStops(){
  var req = new XMLHttpRequest();
  req.open('GET', STOP_URL, true);
  req.onload = function(){
    if(req.readyState === 4){
      stop_data = JSON.parse(req.responseText);
      loadStopSchedules();
    }
    else{
      console.log('Error');
    }
  };
  req.send(null);
}

function loadStopSchedules(){
  var req = new XMLHttpRequest();
  req.open('GET',STOP_SCHED, true);
  req.onload = function(){
    if(req.readyState === 4){
      stop_schedules_data = JSON.parse(req.responseText);
    }
    else{
      console.log('Error');
    }
  };
  req.send(null);
}

function findStops(lat, long){
  var dists = stop_data.map(function(s){
    var dist =  latLongDist(lat, long, s.Latitude, s.Longitude);
    return [s.Name,dist];
  });
  var closest_stops = 
    dists.sort(function(a,b){return a[1] - b[1];})
    .slice(0,Math.min(stop_data.length,NUM_STOPS))
    .map(function(a){return a[0];});
  return closest_stops;
}

function findRoutes(time, stop){
  var routes = stop_schedules_data.filter(function(a){return a.Stop == stop;})
    .sort(function(a,b){return (convertTime(a.Time) - time) - (convertTime(b.Time)-time);});
  routes = routes.slice(0,Math.min(routes.length, NUM_ROUTES));  
  
  var routeString = '';
  for (var i=0; i<routes.length; i++) {
    routeString += routes[i].Stop + ',' + routes[i].Time + ',';
  }
  return routeString;
}


/* ================================================================================
                                  MESSAGING CODE
   ================================================================================*/

// GPS options
var options = {
  enableHighAccuracy: true,
  timeout: 5000,
  maximumAge: 0
};

/* Upon sucessful  */
function success(pos) {
  var crd = pos.coords;
  var dict = {};
  
  var stops = findStops(crd.latitude, crd.longitude);
  for (var i=0; i<stops.length; i++) {
    dict.push({
      key: i,
      value: stops[i]
    });
  }
  for (var j=0; j<stops.length; j++) {
    dict.push({
      key: j+5,
      value: findRoutes(time, stops[j])
    });
  }
  
  // Send the data 
  Pebble.sendAppMessage(dict,
    function(e) {
      console.log('Send successful.');
    },
    function(e) {
      console.log('Send failed!');
    }
  );
}

// Callback function from miscommunication
function error(err) {
  console.warn('ERROR(' + err.code + '): ' + err.message);
}


// Intial calback function from first communication
Pebble.addEventListener('ready', function(e) {
  console.log('PebbleKit JS Ready!');
  
  loadStops();
  navigator.geolocation.getCurrentPosition(success, error, options);
  
});


Pebble.addEventListener('gpsrequest', function(e) {
  console.log('Asking for GPS coords...');
  
  navigator.geolocation.getCurrentPosition(success, error, options);
});

