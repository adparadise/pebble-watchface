// Listen for when the watchface is opened
Pebble.addEventListener('ready', readyHandler);
// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage', appMessageHandler);

function readyHandler (e) {
    getWeather();
}

function appMessageHandler (e) {
    getWeather();
}

function xhrRequest (url, type, callback) {
    var xhr;
    xhr = new XMLHttpRequest();
    xhr.onload = function () {
        callback(this.responseText);
    };

    xhr.open(type, url);
    xhr.send();
};

function clearScreen () {
    var appMessage;
    appMessage = {
        'KEY_TEMPERATURE': '',
        'KEY_CONDITIONS': '',
        'KEY_CONDITIONS_ID': 0,
        'KEY_LAT': '',
        'KEY_LONG': ''
    };

    Pebble.sendAppMessage(appMessage, successHandler, errorHandler);

    function errorHandler (event) {
        appLog(':( clear');
    }

    function successHandler (event) {}
}

function locationSuccess (pos) {
    var url;
    url = 'http://api.openweathermap.org/data/2.5/weather?' +
        'lat=' + pos.coords.latitude + '&' +
        'lon=' + pos.coords.longitude;

    appPos(pos, '...weather');
    xhrRequest(url, 'GET', weatherResponseHandler);

    function weatherResponseHandler (responseText) {
        var json, appMessage;
        var temperature, conditions, conditionsId, locationName;
        var time;

        try {
            json = JSON.parse(responseText);

            temperature = json.main.temp - 273.15;
            temperature = (temperature * 9 / 5) + 32;
            temperature = Math.round(temperature);
            temperature = '' + temperature + 'F'

            conditions = json.weather[0].main;
            conditionsId = '' + json.weather[0].id;

            locationName = json.name;

            time = '';
            time = new Date();
            time = time.toISOString();
            time = time.slice(11, 19);

            appMessage = {
                'KEY_TEMPERATURE': temperature,
                'KEY_CONDITIONS': conditions,
                'KEY_CONDITIONS_ID': conditionsId,
                'KEY_LAT': time,
                'KEY_LONG': locationName
            };

            Pebble.sendAppMessage(appMessage, successHandler, errorHandler);
        } catch (e) {
            appLog(e.slice(0, 16));
        }

        function errorHandler (event) {
            appLog(':( weather');
        }

        function successHandler (event) {
            appLog('');
        }
    }
}

// Send our lat/long to the app for display.
function appPos (pos, message) {
    var appMessage;

    appMessage = {
        'KEY_LAT': stringify(pos.coords.latitude),
        'KEY_LONG': stringify(pos.coords.longitude)
    };
    if (message) {
        appMessage.KEY_LOG = message;
    };

    Pebble.sendAppMessage(appMessage, successHandler, errorHandler);

    function stringify (coord) {
        coord = coord.toFixed(6);
        coord = "" + coord;

        return coord;
    }

    function errorHandler (event) {
        console.log(event);
    }

    function successHandler (event) {
        console.log(event);
    }
}

// Send an arbitrary, short string to the app for display.
function appLog(message) {
    var appMessage;

    appMessage = {
        'KEY_LOG': message
    };

    Pebble.sendAppMessage(appMessage, successHandler, errorHandler);

    function errorHandler (event) {
        console.log(event);
    }

    function successHandler (event) {
        console.log(event);
    }
}

function locationError(err) {
    appLog(':( location');
}

function getWeather() {
    clearScreen();
    appLog('location...');

    navigator.geolocation.getCurrentPosition(
        locationSuccess,
        locationError,
        {
            timeout: 15000,
            maximumAge: 60000
        }
    );
}
