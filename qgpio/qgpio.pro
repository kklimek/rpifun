#########
# QGpio library provides Qt-way to control and read RaspberryPi gpio
# It's based on WiringPi library from https://projects.drogon.net/raspberry-pi/wiringpi/
#


TEMPLATE = lib
TARGET = qgpio

LIBS += -lwiringPi

HEADERS += QGpio.h
SOURCES += QGpio.cpp
