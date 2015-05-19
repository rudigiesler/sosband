from flask.ext.script import Manager
from flask.ext.migrate import Migrate, MigrateCommand
import os

from app import app, db
from models import NumberPriorities
app.config.from_object(os.environ['APP_SETTINGS'])

migrate = Migrate(app, db)
manager = Manager(app)

manager.add_command('db', MigrateCommand)

@manager.command
def create_priorities(levels=2):
    "Create phone number priorities"
    for i in range(levels):
        p = NumberPriorities(i)
        db.session.add(p)
    db.session.commit()

if __name__ == '__main__':
    manager.run()
