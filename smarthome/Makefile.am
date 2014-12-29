
bin_PROGRAMS = smart_home
smart_home_SOURCES = smart_home.c galileo_gpio.c message.c serial.c thermometer.c device.c message_queue.c socket_listener.c gasdetector.c camera.c

AM_CFLAGS = @smart_home_CFLAGS@
AM_LDFLAGS = @smart_home_LIBS@ -pthread -lopencv_core -lopencv_imgproc -lopencv_legacy -lopencv_ml -lopencv_highgui -lm

CLEANFILES = *~ 
