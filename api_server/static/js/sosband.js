$('#formnumber1').submit(function () {
    var number = $('#number1').val();
    var button = $('#formnumber1 button');
    update_number(0, number, button);
    return false;
});

$('#formnumber2').submit(function () {
    var number = $('#number2').val();
    var button = $('#formnumber2 button');
    update_number(1, number, button);
    return false;
});

function update_number(priority, number, button) {
    button.prop('disabled', true);
    $.ajax({
        data: JSON.stringify({number: number}),
        contentType: "application/json",
        type: "POST",
        url: "/api/priorities/" + priority
    }).done(function (data) {
        button.prop('disabled', false);
    });
}

$(document).ready(function () {
    $.ajax({
        type: "GET",
        url: "/api/priorities/0"
    }).done(function (data) {
        number = JSON.parse(data).number;
        $('#number1').val(number);
    });

    $.ajax({
        type: "GET",
        url: "/api/priorities/1"
    }).done(function (data) {
        number = JSON.parse(data).number;
        $('#number2').val(number);
    });

    $('#starttime').val(new Date(new Date() - 60*60*1000).toUTCString());
    $('#endtime').val(new Date().toUTCString());
    initMap();
    update_map();
});


var map;
var markers = [];
var red_circle ={
    path: google.maps.SymbolPath.CIRCLE,
    fillColor: 'red',
    fillOpacity: .6,
    scale: 4.5,
    strokeColor: 'white',
    strokeWeight: 1
};
var green_circle ={
    path: google.maps.SymbolPath.CIRCLE,
    fillColor: 'green',
    fillOpacity: 1,
    scale: 4.5,
    strokeColor: 'white',
    strokeWeight: 1
};


function initMap() {
    map = new google.maps.Map(document.getElementById("map"));
}

function update_map() {
    $('button[name="update_map"]').prop('disabled', true);
    starttime = $('#starttime').val();
    endtime = $('#endtime').val();
    latlngbound = new google.maps.LatLngBounds();
    for(i in markers) {
        m = markers[i];
        m.setMap(null);
    }
    markers = [];
    $.ajax({
        type: "GET",
        url: "/api/gps",
        data: {
            starttime: starttime,
            endtime: endtime
        }
    }).done(function (data) {
        points = JSON.parse(data);
        for(i in points) {
            var p = points[i];
            var marker = new google.maps.Marker({
                position: {
                    lat: p.latitude,
                    lng: p.longitude
                },
                map: map,
                title: p.timestamp,
                icon: red_circle,
                zIndex: p.id
            });
            console.log(p);
            marker.point = p;
            google.maps.event.addListener(marker, 'click', function () {
                new google.maps.InfoWindow({
                    content: [
                        'latitude:', this.point.latitude, '<br />',
                        'longitude:', this.point.longitude, '<br />',
                        'speed:', this.point.speed, '<br />',
                        'timestamp:', this.point.timestamp, '<br />'].join(' ')
                }).open(map, this);
            });
            markers.push(marker);
            latlngbound.extend(marker.position);
        };
        if(markers.length > 0) {
            markers[markers.length-1].icon = green_circle;
        }
        map.setCenter(latlngbound.getCenter());
        map.fitBounds(latlngbound);
        $('button[name="update_map"]').prop('disabled', false);
    });
}
