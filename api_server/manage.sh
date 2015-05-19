export APP_SETTINGS='config.DevelopmentConfig'
export SECRET_KEY='devsecretkey'
export DATABASE_URL='postgresql://sosband:sosband@localhost/sosband'
python manage.py "$@"
