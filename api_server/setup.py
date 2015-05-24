from setuptools import setup, find_packages

setup(
    name="sosband_api",
    version="0.0.1a",
    url="https://github.com/rudigiesler/sosband",
    license="BSD",
    description="Provides a REST API for submitting and retrieving GPS data",
    long_description=open('README.rst', 'r').read(),
    author="Rudi Giesler",
    author_email="rgiesler@gmail.com",
    packages=find_packages(),
    scripts=[],
    install_requires=[
        'Flask',
        'gunicorn',
        'supervisor',
        'psycopg2cffi',
        'Flask-SQLAlchemy',
        'Flask-Migrate',
        'pynmea2',
    ],
    classifiers=[
        'Development Status :: 4 - Beta',
        'Intended Audience :: Developers',
        'License :: OSI Approved :: BSD License',
        'Operating System :: POSIX',
        'Programming Language :: Python',
        'Programming Language :: Python :: Implementation :: PyPy',
        'Topic :: Internet :: WWW/HTTP :: Dynamic Content',
    ],
)
