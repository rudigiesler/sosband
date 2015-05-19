from app import db
from datetime import datetime


class NumberPriorities(db.Model):
    __tablename__ = 'number_priorities'

    priority = db.Column(db.Integer, primary_key=True)
    numbers = db.relationship("EmergencyNumbers", backref='number_priorities')

    def __init__(self, priority):
        self.priority = priority

    def __repr__(self):
        return "<Priority %d>" % self.priority


class EmergencyNumbers(db.Model):
    __tablename__ = 'emergency_numbers'

    id = db.Column(db.Integer, primary_key=True)
    priority = db.Column(
        db.Integer, db.ForeignKey('number_priorities.priority'))
    number = db.Column(db.String)
    created_timestamp = db.Column(db.DateTime)
    archived = db.Column(db.Boolean)
    archived_timestamp = db.Column(db.DateTime)

    def __init__(self, priority, number):
        self.priority = priority
        self.number = number
        self.created_timestamp = datetime.now()
        self.archived = False

    def archive(self):
        self.archived_timestamp = datetime.now()
        self.archived = True

    def __repr__(self):
        return '<%s>' % (self.number)
