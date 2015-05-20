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

@app.route('/emergency_numbers')
def emergency_numbers():
    def serialize(number):
        ret = {}
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
    archived = request.args.get('archived', 'false')
    if archived.upper() == 'TRUE':
        archived = True
    else:
        archived = False
    numbers = EmergencyNumbers.query.filter_by(archived=archived)
    numbers = [serialize(n) for n in numbers]
    return json.dumps(numbers)

@app.route('/arduino/numbers')
def arduino_numbers():
    """ A special endpoint for the Arduino designed to be easy to process
        on the Arduino """
    priorities = (NumberPriorities.query
        .outerjoin(EmergencyNumbers)
        .order_by(NumberPriorities.priority.asc())
        )
    numbers = []
    for n in priorities:
        if len(n.numbers) > 0:
            numbers.append(n.numbers[0].number)
        else:
            numbers.append('')
    print '\n'.join(numbers)
    return '\n'.join(numbers)
