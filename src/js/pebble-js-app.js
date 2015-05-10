// Listen for when the watchface is opened
Pebble.addEventListener('ready', readyHandler);
// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage', appMessageHandler);

function readyHandler (e) {
    appLog('pong');

    getWeather();
}

function appMessageHandler (e) {
    console.log('AppMessage received!');

    // Get the initial weather
    //getWeather();
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

function locationSuccess(pos) {
    var url;
    url = 'http://api.openweathermap.org/data/2.5/weather?' +
        'lat=' + pos.coords.latitude + '&' +
        'lon=' + pos.coords.longitude;

    appLog(':) loc');
    xhrRequest(url, 'GET', weatherResponseHandler);

    function weatherResponseHandler (responseText) {
        var json, temperature, conditions;
        var appMessage;

        try {
            json = JSON.parse(responseText);

            temperature = json.main.temp - 273.15;
            temperature = (temperature * 9 / 5) + 32;
            temperature = Math.round(temperature);

            conditions = json.weather[0].main;

            appMessage = {
                'KEY_TEMPERATURE': temperature,
                'KEY_CONDITIONS': conditions
            };

            Pebble.sendAppMessage(appMessage, successHandler, errorHandler);
        } catch (e) {
            appLog(e.slice(0, 16));
        }

        function errorHandler (event) {
            appLog(':( weather');
        }

        function successHandler (event) {
            appLog(':)');
        }
    }
}

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
    navigator.geolocation.getCurrentPosition(
        locationSuccess,
        locationError,
        {
            timeout: 15000,
            maximumAge: 60000
        }
    );
}
