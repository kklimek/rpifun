#########
# QRpiGpio library provides Qt-way to interact RaspberryPi gpio P1 header
# It's based on WiringPi library from https://projects.drogon.net/raspberry-pi/wiringpi/
#

TEMPLATE = lib
TARGET = qrpigpio

QT = core

include(qrpigpio.pri)
