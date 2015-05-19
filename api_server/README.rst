The backend server that collects the GPS data and provides a REST API to access it

To run a development server:
  - Install requirements
      - pip install -e .
  - Setup database
      - psql -U postgres -c "CREATE ROLE sosband WITH PASSWORD 'sosband' LOGIN"
      - psql -U postgres -c "CREATE DATABASE sosband"
      - ./manage.sh db upgrade
      - ./manage.sh create_priorities
  - Run the process
      - ./manage.sh runserver
