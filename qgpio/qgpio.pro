#########
# QGpio library provides Qt-way to control and read RaspberryPi gpio
# It's based on WiringPi library from https://projects.drogon.net/raspberry-pi/wiringpi/
#


TEMPLATE = lib
TARGET = qgpio

QT -= gui webkit sql

include(qgpio.pri)
