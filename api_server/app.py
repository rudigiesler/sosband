from flask import Flask, request
from flask.ext.sqlalchemy import SQLAlchemy
import os
import json


app = Flask(__name__)
app.config.from_object(os.environ['APP_SETTINGS'])
db = SQLAlchemy(app)

from models import NumberPriorities, EmergencyNumbers


@app.route('/')
def hello():
    return "Hello World!"

def serialize_number(number):
    " Serialize the given number into a dict "
    ret = {}
    if not number:
        return ret
    ret['id'] = number.id
    ret['priority'] = number.priority
    ret['number'] = number.number
    ret['created_timestamp'] = number.created_timestamp.isoformat()
    if number.archived:
        ret['archived'] = True
        ret['archived_timestamp'] = number.archived_timestamp.isoformat()
    else:
        ret['archived'] = False
    return ret


@app.route('/emergency_numbers', methods=['GET'])
def emergency_numbers():
    archived = request.args.get('archived', 'false')
    if archived.upper() == 'TRUE':
        archived = True
    else:
        archived = False
    numbers = EmergencyNumbers.query.filter_by(archived=archived)
    numbers = map(serialize_number, numbers)
    return json.dumps(numbers)

@app.route('/priorities/<int:priority>', methods=['GET', 'POST'])
def priorities_resource(priority):
    if request.method == "GET":
        return get_number_for_priority(priority)
    else:
        return set_number_for_priority(priority)


def get_number_for_priority(priority):
    """ Returns the current emergency number for the priority """
    number = (
        EmergencyNumbers.query.filter(db.and_(
            EmergencyNumbers.archived == False,
            EmergencyNumbers.priority == priority))
        .first()
        )
    return json.dumps(serialize_number(number))


def set_number_for_priority(priority):
    """ Archives the old emergency number, and creates a new emergency number
        with the priority `priority`. """
    pass

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
    
