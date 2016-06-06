from flask import Flask, request
from flask.ext.sqlalchemy import SQLAlchemy
import os
import json


app = Flask(__name__)
app.config.from_object(os.environ['APP_SETTINGS'])
db = SQLAlchemy(app)

from models import NumberPriorities, EmergencyNumbers, GPSPoints


@app.route('/')
def hello():
    return "Hello World!"

def serialize_number(number, priority):
    " Serialize the given number into a dict "
    ret = {}
    ret['priority'] = getattr(priority, 'priority', None)
    ret['number'] = getattr(number, 'number', None)
    timestamp = getattr(number, 'created_timestamp', None)
    ret['created_timestamp'] = timestamp and timestamp.isoformat()
    if getattr(number, 'archived', None):
        ret['archived'] = True
        ret['archived_timestamp'] = getattr(number, 'archived_timestamp').isoformat()
    else:
        ret['archived'] = False
    return ret


def serialize_point(point):
    ret = {}
    if not point:
        return ret
    ret['id'] = getattr(point, 'id')
    ret['latitude'] = getattr(point, 'latitude')
    ret['longitude'] = getattr(point, 'longitude')
    ret['speed'] = getattr(point, 'speed')
    ret['course'] = getattr(point, 'course')
    timestamp = getattr(point, 'timestamp')
    ret['timestamp'] = timestamp and timestamp.isoformat()
    if getattr(point, 'archived'):
        ret['archived'] = True
        ret['archived_timestamp'] = getattr(number, 'archived_timestamp').isoformat()
    else:
        ret['archived'] = False
    return ret


@app.route('/priorities', methods=['GET'])
def emergency_numbers():
    archived = request.args.get('archived', 'false')
    if archived.upper() == 'TRUE':
        archived = True
    else:
        archived = False
    numbers = (
        db.session.query(NumberPriorities, EmergencyNumbers)
        .outerjoin(EmergencyNumbers, db.and_(
            EmergencyNumbers.priority == NumberPriorities.priority,
            EmergencyNumbers.archived == archived))
        .order_by(NumberPriorities.priority)
        )
    numbers = [serialize_number(en, np) for np, en in numbers]
    return json.dumps(numbers)

@app.route('/priorities/<int:priority>', methods=['GET', 'POST'])
def priorities_resource(priority):
    if request.method == "GET":
        return get_number_for_priority(priority)
    else:
        return set_number_for_priority(priority)


def get_number_for_priority(priority):
    """ Returns the current emergency number for the priority """
    number = (EmergencyNumbers.query.filter(db.and_(
        EmergencyNumbers.priority == priority,
        EmergencyNumbers.archived == False))
        .first()
        )
    priority = NumberPriorities.query.get(priority)
    return json.dumps(serialize_number(number, priority))


def set_number_for_priority(priority):
    """ Archives the old emergency number, and creates a new emergency number
        with the priority `priority`. """
    priority = (NumberPriorities.query.filter(
        NumberPriorities.priority == priority)
        .first()
        )
    old_numbers = (
        EmergencyNumbers.query.filter(db.and_(
            EmergencyNumbers.archived == False,
            EmergencyNumbers.priority == priority.priority))
        )
    for number in old_numbers:
        number.archive()
        db.session.add(number)
    new_num = EmergencyNumbers(priority.priority, request.get_json()['number'])
    db.session.add(new_num)
    db.session.commit()
    return json.dumps(serialize_number(new_num, priority))

@app.route('/gps', methods = ['GET', 'POST'])
def gps_resource():
    if request.method == 'GET':
        return get_gps_points()
    else:
        return create_gps_point()


def get_gps_points():
    archived = request.args.get('archived', 'false')
    if archived.upper() == 'TRUE':
        archived = True
    else:
        archived = False
    points = (
        db.session.query(GPSPoints)
        .filter(GPSPoints.archived == archived)
        .order_by(GPSPoints.timestamp)
        )
    points = map(serialize_point, points)
    return json.dumps(points)

def create_gps_point():
    data = request.get_json()
    if data.get('nmea_string'):
        point = GPSPoints.from_string(data['nmea_string'])
    else:
        point = GPSPoints(
            data.get('timestamp'), data.get('latitude'), data.get('longitude'),
            data.get('speed'), data.get('course')
            )
    db.session.add(point)
    db.session.commit()
    return json.dumps(serialize_point(point))


@app.route('/arduino/numbers',  methods=['GET'])
def arduino_numbers():
    """ A special endpoint for the Arduino designed to be easy to process
        on the Arduino """
    numbers = (
        db.session.query(NumberPriorities, EmergencyNumbers.number)
        .outerjoin(EmergencyNumbers, db.and_(
            EmergencyNumbers.priority == NumberPriorities.priority,
            EmergencyNumbers.archived == False))
        .order_by(NumberPriorities.priority)
        )
    numbers = [n or '' for p, n in numbers]

    return '\n'.join(numbers)

@app.route('/arduino/gps', methods=['POST'])
def arduino_gps_data():
    """ A special endpoint for the Arduino designed to be very easy
        to post NMEA data directly to """
    point = GPSPoints.from_string(request.data)
    db.session.add(point)
    db.session.commit()
    return ""
