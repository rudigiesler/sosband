function update_number_1() {
    var number = $('#number1').val();
    update_number(0, number);
}

function update_number_2() {
    var number = $('#number2').val();
    update_number(1, number);
}

function update_number(priority, number) {
    $.ajax({
        data: JSON.stringify({number: number}),
        contentType: "application/json",
        type: "POST",
        url: "/api/priorities/" + priority
    })
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
function initMap() {
    map = new google.maps.Map(document.getElementById("map"));
}

function update_map() {
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
            marker = new google.maps.Marker({
                position: {
                    lat: p.latitude,
                    lng: p.longitude
                },
                map: map,
                title: p.timestamp,
                zIndex: p.id
            });
            markers.push(marker);
            latlngbound.extend(marker.position);
        };
        map.setCenter(latlngbound.getCenter());
        map.fitBounds(latlngbound);
    });
}
