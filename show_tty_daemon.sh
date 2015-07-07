#!/bin/bash
currentpath=`dirname $0`
function isalive(){
	local pid=`ps -ef|grep "show_tty_daemon.sh show"|grep -v "grep"|awk '{print $2}'`
	local pidArray=($pid)
	local pidNum=`ps -ef|grep "show_tty_daemon.sh show"|grep -v "grep"|awk '{print $2}'|wc -l`
	#echo "pid="$pid
	if [ ${#pid} -ne 0 ] ; then
		if [ ${pidNum} -gt 1 ] ; then
			killsame
			bash ${currentpath}/show_tty_daemon.sh show
			exit 1
		elif [ ${pidNum} -eq 0 ]; then
			bash ${currentpath}/show_tty_daemon.sh show	
			exit 1
		else ;then
			exit 0
		fi
	fi
}

function killsame(){
	local pid=`ps -ef|grep "show_tty_daemon.sh show"|grep -v "grep"|awk '{print $2}'`
	local pidArray=($pid)
	local pidNum=`ps -ef|grep "show_tty_daemon.sh show"|grep -v "grep"|awk '{print $2}'|wc -l`
	#echo "pid="$pid
	if [ ${#pid} -ne 0 ] ; then
		for (( i = 0; i < pidNum; i++)); do
			exec kill -9 ${pidArray[i]} &
			#echo ${pidArray[i]}
		done;
	fi
}

function show(){
	echo -ne '\033[?25l';
	echo $$>show_tty_name.txt;
	tty>>show_tty_name.txt;
	while true;do
		read -s;
	done
}


if [ $# -eq 1 ]; then
	if [ "$1" == "killsame" ]; then
		killsame
	elif [ "$1" == "isalive" ]; then
		isalive	
	elif [ "$1" == "show" ]; then
		show	
	fi
fi




