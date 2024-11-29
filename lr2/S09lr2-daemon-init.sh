#!/bin/bash

PID_FILE_NAME='/var/run/lr2_daemon.pid'

case "$1" in
start)
echo "Starting..."
if [[ -f ${PID_FILE_NAME} ]]; then
  PID=`cat ${PID_FILE_NAME}`

  if ps -p $PID > /dev/null; then
    echo "Process already running: ${PID}"
    exit 1
  fi
fi

lr2_daemon
;;
stop)
kill -SIGINT `cat ${PID_FILE_NAME}`
rm ${PID_FILE_NAME}
;;
status)

if [[ -f ${PID_FILE_NAME} ]]; then
  PID=`cat ${PID_FILE_NAME}`

  if ps -p $PID > /dev/null; then
    echo "Process running: ${PID}"
    exit 0
  else
    echo "Process ${PID} unexpectedly stopped"
    exit 0
  fi
else
  echo "Process stopped"
fi

;;
force-reload|restart)
$0 stop && $0 start
;;
*)
echo "Usage: /etc/init.d/networking {start|stop|status|restart|force-reload}"
exit 1
;;
esac