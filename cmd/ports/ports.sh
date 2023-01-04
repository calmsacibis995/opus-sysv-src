CFG=/tmp/opus.cfg
INIT=/etc/inittab

# Get current configuration file from dos
/opus/bin/ducp -a \\opus\\opus.cfg $CFG

# get list of tty devices from config file
CFGLST=`grep '^<tty[0-9]' $CFG | sed '{s/^<//
s/=.*$//
}'`

# get list of tty devices in current inittab file
INITLST=`grep '[ 	]tty[0-9]*[ 	]' $INIT | awk '{ print $3 }'`

# get list of tty devices in /dev directory
DEVLST=`/bin/ls /dev/tty[0-9]* | sed 's/\/dev\///'`

# filter out current tty enties in inittab file
sed '/[ 	]tty[0-9]*[ 	]/d' $INIT > /tmp/init$$

# Loop for all tty entries in config file
for TTY in $CFGLST
do
	NUM=`echo $TTY | sed 's/tty//'`
	# make single digit numbers double digit
	case $NUM in
		'' ) NUM="00"
		   ;;
		[0-9] ) NUM="0${NUM}"
		   ;;
	esac
	# add new entry to inittab file
	echo "${NUM}:23:respawn:/etc/getty -t60 $TTY 9600" >> /tmp/init$$
	# create new device nodes if needed
	if [ ! -c /dev/${TTY} ]; then
		mknod /dev/${TTY} c 9 $NUM
		chmod 666 /dev/${TTY}
		chown root /dev/${TTY}
		chgrp sys /dev/${TTY}
	fi
done

# save old inittab file
mv $INIT /etc/oinittab
mv /tmp/init$$ $INIT
exit 0
