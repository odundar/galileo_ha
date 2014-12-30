#!/bin/sh
#
# Start the Home Automation Application
#

echo "Starting Home Automation Application" > /var/log/homelog
/usr/bin/smart_home &> /var/log/homelog

