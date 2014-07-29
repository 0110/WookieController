# List of all the board related files.
APPSRC = ${APP}/main.c \
		${APP}/usbcdc/usbcdc.c \
		${APP}/cmd/cmd_threads.c \
		${APP}/cmd/cmd_mem.c 

APPSRC += ${APP}/HD44780/lcdMain.c

# Required include directories
APPINC = ${APP}/HD44780 \
		 ${APP}
		 

# List all user C define here
APPDEFS =

