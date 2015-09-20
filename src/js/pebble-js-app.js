/*
 * TCAT app
 * By: Ryan Dong, Michael Huang, and Alan Wu
 */

var NUM_STOPS = 5;
var NUM_ROUTES = 5;
var STOP_URL = 'http://cornelldata.org/api/v0/TCAT-data/stop-locations';
var ROUTE_URL = 'https://cornelldata.org/api/v0/TCAT-data/route-schedules';
var stop_data;
var route_data;
var time;
var day;

function latLongDist(lat1, long1, lat2, long2){
  return (lat1-lat2)*(lat1-lat2) + (long1-long2)*(long1-long2);
}

function five_n_sort(a) {
  var ans = [];
  var running_min;
  for (var i = 0; i < 5; i++) {
    running_min = Number.MAX_SAFE_INTEGER;
    var iid = 0;
    for (var j = 0; j < a.length; j++) {
      if (running_min > a[j][1]) {
          running_min = a[j][1];
          iid = j;
      }
    }
    ans.push(a[iid]);
    a.splice(iid,1);
  }
  return ans;
}

function convertTime(s) {
    var colonInd = s.indexOf(":");
    var hours = +s.slice(0,colonInd);
    var minutes = +s.slice(colonInd+1,colonInd+3);
    if(s.length > colonInd + 3){
      var ampm = s.slice(colonInd+3);
      if(ampm == 'PM' && hours == 12){
        return hours*60 + minutes;      
      }
      if(ampm == 'AM' && hours == 12){
        return minutes;
      }
      if (ampm == "PM") {
        return (hours + 12)*60 + minutes;
      } 
    }
    else{
      return hours*60 + minutes;
    }
}

function loadStops(){
  var req = new XMLHttpRequest();
  req.open('GET', STOP_URL, false);
  req.onload = function(){
    if(req.readyState === 4){
      stop_data = JSON.parse(req.responseText);
      loadRoutes();
    }
    else{
      console.log('Error');
    }
  };
  req.send(null);
}

function loadRoutes(){
  var req = new XMLHttpRequest();
  req.open('GET',ROUTE_URL, false);
  req.onload = function(){
    if(req.readyState === 4){
      route_data = JSON.parse(req.responseText);
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
  var closest_stops = five_n_sort(dists);
  
  for (var i = 0; i < closest_stops.length; i++ ) {
    console.log(closest_stops[i][0] + ' ' + closest_stops[i][1]);
  }
  
  closest_stops = closest_stops.map(function(a){return a[0];});
  return closest_stops;
}

function findRoutes(stop){
  var routes = route_data.filter(function(a){return a.Stop == stop && a.Day == day;});
  console.log(routes.length);
  routes.sort(function(a,b){return (convertTime(a.Time) - time) - (convertTime(b.Time) - time);});
  routes = routes.slice(0,Math.min(routes.length, NUM_ROUTES));  
  
  
  var routeString = '';
  for (var i=0; i<routes.length; i++) {
    routeString += routes[i].Route + ',' + routes[i].Time + ',' + (routes[i].Direction == 'Inbound' ? 'I' : 'O') + ',';
  }
  return routeString.substring(0,routeString.length-1);
}

loadStops();  
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
  
  console.log(crd.latitude + ' ' + crd.longitude);
  
  var stops = findStops(crd.latitude, crd.longitude);
  
  var dict = {};
  dict[0] = stops[0];
  dict[1] = stops[1];
  dict[2] = stops[2];
  dict[3] = stops[3];
  dict[4] = stops[4];
  dict[5] = findRoutes(stops[0]);
  dict[6] = findRoutes(stops[1]);
  dict[7] = findRoutes(stops[2]);
  dict[8] = findRoutes(stops[3]);
  dict[9] = findRoutes(stops[4]);
  
  for (var i = 5; i < 10; i++) {
    console.log(dict[i]);
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
  Pebble.sendAppMessage({12:'Happy Fuckb0i Day! 9/19/2015'},
                       function(e){console.log('Send successful.');},
                       function(e){console.log('Send failed!');});
  console.log('PebbleKit JS Ready!');
});


Pebble.addEventListener('appmessage', function(e) {
  console.log('Asking for GPS coords...');
  day = e.payload[10];
  time = e.payload[11];
  
  navigator.geolocation.getCurrentPosition(success, error, options);
});

