
# edittbl - edit DOS configuration file

trap "rm /tmp/*$$ > /dev/null 2>&1;exit" 1 2 3

if [ ! -z "$1" ]; then
	DOSCFG="$1"
else
	DOSCFG="\\opus\\opus.cfg"
fi
CONFIG="/tmp/opus.cfg$$"
OLDCFG="/tmp/save.cfg$$"

# Get current configuration file from dos
rm $CONFIG > /dev/null 2>&1
/opus/bin/ducp -a $DOSCFG /tmp/xx.$$ > /dev/null 2>&1
if [ ! -f /tmp/xx.$$ ]; then
	echo "Cannot find configuration file: $DOSCFG"
	rm /tmp/xx.$$
	exit 1
fi
# strip out config comment lines
sed '/^\[/p
/^</p
d' /tmp/xx.$$ > $CONFIG
# Save comments & add back at end
sed '/^\./p
d' /tmp/xx.$$ > /tmp/comments.$$
rm /tmp/xx.$$

MONDEV="/boot/opmon/mondev.c"
if [ ! -f ${CONFIG} ]; then
	>${CONFIG}
	echo "[tz=PST8PDT]
[i=x]
[s=?]
[a=128]
<clock=clock>
<dos=dos>
<console=console>
<dsk/0=c:>" >> ${CONFIG}
fi

# Save original configuration
cp $CONFIG $OLDCFG

add_params()
{
echo
echo "	Some drivers will accept parameters that change default settings."
echo "	Any optional parameters for this device are listed in the Opus User"
echo "	Manual in the \"Devices\" section under \"${DRIVER}\"."
echo "	Would you like to add any parameters to this device or range"
echo "	of devices ? (y,n) : \c"
read XXX
CNT=0
if [ "$XXX" = "y" -o "$XXX" = "Y" ]; then
	PSTRING=""
	while [ $XXX != "q" -a $XXX != "Q" ]; do
		NUM=`expr $CNT + 1`
		echo "	Enter ${DRIVER} parameter $NUM (q if no more) : \c"
		read XXX
		if [ "$XXX" = "q" -o "$XXX" = "Q" ]
		then
			break
		fi
		PARAM="$XXX"
		echo "		Parameter $NUM = \"$PARAM\". Is this correct ? (y/n) : \c"
		read XXX
		if [ "$XXX" != "y" -a "$XXX" != "Y" ]; then
			continue
		fi
		PSTRING="${PSTRING},${PARAM}"
		CNT=`expr $CNT + 1`
		echo
	done
	echo
	PSTRING=`echo $PSTRING | sed '{s/(//g
		s/)//g
		s/<//g
		s/>//g
		s/^,//
		}'`
	PSTRING="(${PSTRING})"
else
	PSTRING=""
fi
}
		
add_driver()
{
sed '/^\/\*\*/{s/^\/\*\*\ *//
s/:<.*>.*$//
p
}
d' ${MONDEV} | \
grep -nv XXX | \
sed '{s/\([0-9]*:\)/\1 /
}' > /tmp/DRV$$

while true; do

tput clear
echo "		Available Drivers (Any driver with a '*' is required.)"
echo "----------------------------------------------------------------------------"
pr -2 -t /tmp/DRV$$
echo "----------------------------------------------------------------------------"
echo
VALID=0
while [ $VALID -eq 0 ]; do
echo "		Select a driver (or 'q' to quit) : \c"
read ANS
if [ "$ANS" = "q" -o "$ANS" = "Q" -o "$ANS" = "" ]; then
	rm /tmp/INSTLD$$ /tmp/DRV$$ /tmp/DEV$$ > /dev/null 2>&1
	return ;# exit driver menu loop
fi
SELECT=`grep "^${ANS}:" /tmp/DRV$$`
ANS_OK=$?
if [ $ANS_OK -ne 0 ]; then
	echo "			${ANS} not available - try again."
else
	VALID=1
fi
done

DRIVER=`echo $SELECT | sed 's/^[0-9]*: //'`
tput clear
echo
echo "These are the available devices for the \"$DRIVER\" driver:"
echo "----------------------------------------------------------------------------"
sed -e "/^\/\*\*[ 	]*$DRIVER[\*:]/,/^$/{s/^\/\*\*[	 ].*\*\///
s/^[	 ]*{[ 	]*\"\([^\"]*\)\".*/\1/
/^$/d
p
}
d" ${MONDEV} |
awk '{ if ( $1 ~ /^\/\*/ ) print $0; else print i++,")",$0 }' |
tee /tmp/DEV$$ | pr -3 -o8 -t
echo "----------------------------------------------------------------------------"
echo "These devices have already been added to the configuration table:"
DEVS=`grep '^[0-9]' /tmp/DEV$$ | awk '{ print $3 }'`
>/tmp/INSTLD$$
IFS='
'
for i in $DEVS; do
	LINE=`grep "$i[>(]" $CONFIG`
	if [ $? -eq 0 ]; then
		echo $LINE | sed 's/^\(<.*\)(.*\(>\)$/\1\2/
			s/^<.*=\(.*\)>$/\1/' >> /tmp/INSTLD$$
	fi
done
if [ -s /tmp/INSTLD$$ ]; then
	cat /tmp/INSTLD$$ | uniq | pr -3 -o8 -t
else
	echo "		NONE"
fi
echo "----------------------------------------------------------------------------"
# Do not display this menu again 1st time through selection loop
REPAINT=0

UNIXNAME=`grep "^\/\*\*\ *${DRIVER}[\*:]" ${MONDEV} | sed '{s/^.*:<\(.*\)>.*$/\1/
}'`
case "$UNIXNAME" in
	*\? )	INCFLG=1
		UNIXNAME=`echo ${UNIXNAME} | sed 's/?$//'`
	;;
	* )	INCFLG=0
	;;
esac


if [ $INCFLG -eq 1 ]; then
    while [ "$ANS" != "q" -a "$ANS" != "Q" -a "$ANS" != "" ]; do
	# Get the next device number not already used in the config file
	DEVCNT=`egrep "<${UNIXNAME}|</dev/${UNIXNAME}" ${CONFIG} | \
	sed "{s:^<${UNIXNAME}::
	s:</dev/${UNIXNAME}::
	s:^/::
	s:=.*$::
	}" | sort -n | \
	sed '{$p
	}
	d'`
	if [ -z "$DEVCNT" ]; then
		DEVCNT=0
	else
		expr $DEVCNT + 1 >/dev/null 2>&1
		RTN=$?
		if [ $RTN -ne 0 ]; then
			# for floppies - DEVCNT is numeric "0" or "1",
			#  DEVNUM is alpha "a" or "b"
			if [ "$UNIXNAME" = "flp" -a "$DRIVER" = "Raw Diskette" ]
			then
				grep flpA= ${CONFIG} > /dev/null 2>&1
				if [ $? -eq 0 ]; then
					grep flpB= ${CONFIG} > /dev/null 2>&1
					if [ $? -eq 0 ]; then
						DEVCNT=2
					else
						DEVCNT=1
					fi
				else
					DEVCNT=0
				fi
			else
				grep flpa= ${CONFIG} > /dev/null 2>&1
				if [ $? -eq 0 ]; then
					grep flpb= ${CONFIG} > /dev/null 2>&1
					if [ $? -eq 0 ]; then
						DEVCNT=2
					else
						DEVCNT=1
					fi
				else
					DEVCNT=0
				fi
			fi
			# This is a kludge for the the console driver only
			if [ "$DEVCNT" = "sole" ]; then
				DEVCNT=1
			fi
		else
			DEVCNT=`expr $DEVCNT + 1`
		fi
	fi

	# ----------- repaint menu quickly -------------------------
	if [ $REPAINT -eq 1 ]; then
	sleep 4
	tput clear
	echo "These are the available devices for the \"$DRIVER\" driver:"
	echo "----------------------------------------------------------------------------"
	cat /tmp/DEV$$ | pr -3 -o8 -t
	echo "----------------------------------------------------------------------------"
	echo "These devices have already been added to the configuration table:"
	if [ -s /tmp/INSTLD$$ ]; then
		cat /tmp/INSTLD$$ | uniq | pr -3 -o8 -t
	else
		echo "		NONE"
	fi
	echo "----------------------------------------------------------------------------"
	fi
	REPAINT=1
		# Adjust names for different types of devices
		if [ $UNIXNAME = "flp" ]; then
			if [ $DEVCNT -ge 2 ]; then
				echo "	No more Diskette devices available."
				echo "	RETURN to continue \c"
				read ANS
				continue
			fi
			if [ "$DRIVER" = "Raw Diskette" ]; then
				DEVNUM=`echo $DEVCNT | tr "01" "AB"`
			else
				DEVNUM=`echo $DEVCNT | tr "01" "ab"`
			fi
		else
			DEVNUM=${DEVCNT}
		fi
		echo "The Unix device name for ${DRIVER} is \"/dev/${UNIXNAME}\"."
		echo "	Select a number(s) for the Unix device \"/dev/${UNIXNAME}${DEVNUM}\""
		echo "	(number, number-number, q) : \c"
		read ANS
		if [ "$ANS" = "q" -o "$ANS" = "Q" -o "$ANS" = "" ]; then
			continue
		fi
		
		echo ${ANS} | grep "[0-9]-[0-9]" > /dev/null 2>&1
		if [ $? -eq 0 ]; then
			FIRST=`echo ${ANS} | cut -d'-' -f1`
			LAST=`echo ${ANS} | cut -d'-' -f2`
			SEQ=""
			CNT=$FIRST
			while [ $CNT -le $LAST ]; do
				SEQ="${SEQ} ${CNT}"
				CNT=`expr $CNT + 1`
			done
		else
			SEQ=${ANS}
		fi

		# Add any optional parameter strings
		add_params
		echo "		Checking configuration ..."

		IFS=' '
		for ANS in $SEQ
		do
		SELECT=`grep "^${ANS} " /tmp/DEV$$`
		ANS_OK=$?
		if [ $ANS_OK -ne 0 ]; then
			echo
			echo "			${ANS} not available - try again."
		else
			DOSDEV=`echo $SELECT | sed 's/^[0-9]* ) //'`
			ENTRY="${UNIXNAME}${DEVNUM}=${DOSDEV}"
			# check to make sure this line doesnt already exist
			# in opus.cfg 1st
			grep "=${DOSDEV}[(>]" ${CONFIG} > /dev/null 2>&1
			if [ $? -ne 0 ]; then
			# check that this is not a different name for a
			#  driver that has already been configured
				DT_NAME=`grep "^[ 	]*{[	 ]*\"${DOSDEV}\"" ${MONDEV} |
				sed 's/.*&\(dt_.*\)[ 	].*$/\1/'`
				FLG=0
IFS='
'
				for DRIVR in `cat ${CONFIG}`; do
					DOSNAME=`echo $DRIVR |
					 sed 's/^\(.*\)(.*$/\1/
						s/^.*=\(.*\)$/\1/
						s/>$//'`
					if [ $DOSNAME = "" ]; then
						continue
					fi
					FUNCT=`grep "\"${DOSNAME}\"" ${MONDEV} |
					sed 's/.*&\(dt_.*\)[ 	].*$/\1/'`
					if [ "$FUNCT" = "$DT_NAME" ]; then
						FLG=1
						SAVE=${DOSNAME}
					fi
				done
				if [ $FLG -eq 1 ]; then
					echo
					echo "		Cannot add \"${DOSDEV}\" to the configuration table:"
					echo "		The \"${DOSDEV}\" driver is the same as the \"${SAVE}\" driver"
					echo "		that has already been installed."
					echo "		Hit RETURN to continue\c"
					read XXX
					continue
				else
					# Add this driver to the config file
					echo "<${ENTRY}${PSTRING}>" >> ${CONFIG}
					echo $ENTRY | sed 's/^.*=\(.*\)$/\1/' >> /tmp/INSTLD$$
					echo "			\"${DOSDEV}\" device added."
				fi
			else
				echo "		Cannot add \"${DOSDEV}\" - device already in configuration table."
				echo "		Hit RETURN to continue \c"
				read XXX
				continue
			fi
		DEVCNT=`expr $DEVCNT + 1`
		# Adjust names for different types of devices
		if [ $UNIXNAME = "flp" ]; then
			if [ "$DRIVER" = "Raw Diskette" ]; then
				DEVNUM=`echo $DEVCNT | tr "01" "AB"`
			else
				DEVNUM=`echo $DEVCNT | tr "01" "ab"`
			fi
		else
			DEVNUM=${DEVCNT}
		fi
		fi
	done
    done
# for cases where the device does not need an index number
else
	CNT=`grep "^[0-9]" /tmp/DEV$$ | wc -l`
	if [ $CNT -eq 1 ]; then
		SELECT=`grep "^0 " /tmp/DEV$$`
		DOSDEV=`echo $SELECT | sed 's/^[0-9]* ) //'`
		ENTRY="${UNIXNAME}=${DOSDEV}"
		# check to make sure this line doesnt already exist
		# in opus.cfg 1st
		grep "=${DOSDEV}[(>]" ${CONFIG} > /dev/null 2>&1
		if [ $? -ne 0 ]; then
			echo "Do you want to add this device to the configuration table ? (y,n) : \c"
			read ANS
			case "$ANS" in
				y* | Y* ) add_params
					echo "<${ENTRY}${PSTRING}>" >> ${CONFIG}
					echo $ENTRY | sed 's/^.*=\(.*\)$/\1/' >> /tmp/INSTLD$$
					echo "		\"${DOSDEV}\" device added."
					echo "		Hit RETURN to continue \c"
					read XXX
				;;
				n* | N* ) continue
				;;
				* ) continue
				;;
			esac
		else
				echo "		Cannot add \"${DOSDEV}\" - device already in configuration table."
				echo "		Hit RETURN to continue \c"
				read XXX
		fi
	# more than one choice for a non-indexed device
	else
	   ANS_OK=1
	   while [ $ANS_OK -ne 0 ]; do

		# Special check for mouse serial driver before
		#  X-driver is allowed
		if [ $UNIXNAME = "X" ]; then
			grep '<tty' $CONFIG > /dev/null 2>&1
			if [ $? -ne 0 ]; then
				echo "	The X driver requires a serial mouse input."
				echo "	Please add this serial driver to the configuration file"
				echo "	before adding the X driver. Use the \"Serial TTY Ports\""
				echo "	option in the \"Add Driver\" menu."
				echo
				echo "	Hit RETURN to continue \c"
				read XXX
				break
			fi
		fi

		echo "The Unix device name for ${DRIVER} is \"/dev/${UNIXNAME}\"."
		echo "	Select a number for the Unix device \"/dev/${UNIXNAME}${DEVNUM}\" (q to quit) : \c"
		read ANS
		if [ "$ANS" = "q" -o "$ANS" = "Q" -o "$ANS" = "" ]; then
			break
		fi
		SELECT=`grep "^${ANS} " /tmp/DEV$$`
		ANS_OK=$?
		if [ $ANS_OK -ne 0 ]; then
			echo
			echo "			${ANS} not available - try again."
		else
			# Add optional parameters
			add_params
			echo "		Checking configuration ..."

			DOSDEV=`echo $SELECT | sed 's/^[0-9]* ) //'`
			ENTRY="${UNIXNAME}=${DOSDEV}"
			# check to make sure this line doesnt already exist
			# in opus.cfg 1st
			grep "=${DOSDEV}[(>]" ${CONFIG} > /dev/null 2>&1
			if [ $? -ne 0 ]; then
			# check that this is not a different name for a
			#  driver that has already been configured
				DT_NAME=`grep "^[ 	]*{[	 ]*\"${DOSDEV}\"" ${MONDEV} |
				sed 's/.*&\(dt_.*\)[ 	].*$/\1/'`
				FLG=0
				for DRIVR in `cat ${CONFIG}`; do
IFS='
'
					DOSNAME=`echo $DRIVR |
					 sed 's/^\(.*\)(.*$/\1/
						s/^.*=\(.*\)$/\1/
						s/>$//'`
					if [ $DOSNAME = "" ]; then
						continue
					fi
					FUNCT=`grep "\"${DOSNAME}\"" ${MONDEV} |
					sed 's/.*&\(dt_.*\)[ 	].*$/\1/'`
					if [ "$FUNCT" = "$DT_NAME" ]; then
						FLG=1
						SAVE=${DOSNAME}
					fi
				done
				if [ $FLG -eq 1 ]; then
					echo
					echo "		Cannot add \"${DOSDEV}\" to the configuration table:"
					echo "		The \"${DOSDEV}\" driver is the same as the \"${SAVE}\" driver"
					echo "		that has already been installed."
					echo "		Hit RETURN to continue\c"
					read XXX
					ANS_OK=1
					continue
				else
					# Add this driver to the config file
					echo "<${ENTRY}${PSTRING}>" >> ${CONFIG}
					echo $ENTRY | sed 's/^.*=\(.*\)$/\1/' >> /tmp/INSTLD$$
					echo "			\"${DOSDEV}\" device added."
					echo "		Hit RETURN to continue \c"
					read XXX
				fi
			else
				echo "		Cannot add \"${DOSDEV}\" - device already in configuration table."
				echo "		Hit RETURN to continue \c"
				read XXX
			fi
		fi
	   done
	fi
fi
rm /tmp/DEV$$ > /dev/null 2>&1
done
}

del_driver()
{
while [ true ]; do
tput clear
echo "			Current Configuration File "
echo "----------------------------------------------------------------------------"
IFS='+'
>/tmp/x$$

# Old way
#for LINE in `cat $CONFIG`
#do
#	ENTRY=`echo $LINE | sed 's/^\(<.*\)(.*\(>\)$/\1\2/
#		s/^<.*=\(.*\)>$/\1/'`
#	if [ ! -z "$ENTRY" ]; then
#		echo $ENTRY >> /tmp/x$$
#	fi
#done

# New way
sed 's/^\(<.*\)(.*\(>\)$/\1\2/
s/^<.*=\(.*\)>$/\1/' $CONFIG > /tmp/X$$

IFS='
'	
if [ -s /tmp/X$$ ]; then
	cat /tmp/X$$ | sed '{/^\[/d
/<end>/d
}' | uniq |
grep -nv XXX |
sed '{s/\([0-9]*:\)/\1 /
}' |
tee /tmp/ENT$$ | pr -3 -o8 -t
else
	echo "		NONE"
fi
echo "----------------------------------------------------------------------------"
rm /tmp/X$$
echo
echo "	Enter the number(s) of a driver you want to delete"
echo "	(number, number-number, q) : \c"
read ANS
if [ "$ANS" = "q" -o "$ANS" = "Q" -o "$ANS" = "" ]; then
	rm /tmp/ENT$$
	return ;# exit driver menu loop
fi
echo ${ANS} | grep "[0-9]-[0-9]" > /dev/null 2>&1
if [ $? -eq 0 ]; then
	FIRST=`echo ${ANS} | cut -d'-' -f1`
	LAST=`echo ${ANS} | cut -d'-' -f2`
	SEQ=""
	CNT=$FIRST
	while [ $CNT -le $LAST ]; do
		SEQ="${SEQ} ${CNT}"
		CNT=`expr $CNT + 1`
	done
else
	SEQ=${ANS}
fi

IFS=' '
for ANS in $SEQ
do
SELECT=`grep "^${ANS}:" /tmp/ENT$$`
ANS_OK=$?
if [ $ANS_OK -ne 0 ]; then
	echo
	echo "			${ANS} not available - try again."
	sleep 4
fi

DRIVER=`echo $SELECT | sed 's/^[0-9]*: //'`

# Check that DRIVER is not a required one. Required devices are the 1st device line
# of any device with a star as the last character of the name.
XXX=`sed "/^\/\*\*[ 	]*/{=
h
}
/\"${DRIVER}\",/{g
s/^\/\*\*.*\(.\):/\1:/
s/:.*$//
q
}
d" $MONDEV | tail -2 | pr -2 -t`
LINENO=`echo "$XXX" | awk '{ print $1 }'`
LASTCHAR=`echo "$XXX" | awk '{ print $2 }'`
if [ "$LASTCHAR" = "*" ]; then
	NEXTLINE=`sed "${LINENO}{:label
	s/dt_/dt_/
	t last
	N
	b label
	}
	d
	:last" ${MONDEV} | sed '/^[	 ]*{/{s/^[	 ]*{[ 	]*\"\([^\"]*\)\".*/\1/
				p
				}
				d'`
	if [ "$DRIVER" = "$NEXTLINE" ]; then
		echo "		\"${DRIVER}\" is a required driver and cannot be removed."
		continue
	fi
fi

sed "{/=${DRIVER}[(>]/d
}" $CONFIG > /tmp/conf$$
mv /tmp/conf$$ $CONFIG
echo "			\"${DRIVER}\" driver removed."
done ; # for ANS in SEQ
rm /tmp/ENT$$
sleep 4
done ; # while true
}

wr_config()
{
# put []'s 1st and <end> last
grep -v "<end>" ${CONFIG} > /tmp/conf$$
sed '{s/\[/\!\[/
}' /tmp/conf$$ | sort | sed '{s/^\!//
}' > ${CONFIG}
# add original comment lines
if [ -s /tmp/comments.$$ ]; then
	cat /tmp/comments.$$ >> ${CONFIG}
fi
echo "<end>" >> ${CONFIG}
rm /tmp/conf$$
/opus/bin/udcp -a $CONFIG $DOSCFG
}

list_conf()
{
tput clear
echo "			Current Configuration File "
echo
echo "	Unix device	DOS device	Parameters"
echo "----------------------------------------------------------------------------"
cat $CONFIG | sed '/^\[/d
/<end>/d
/^</{s/^</	\/dev\//
s/\/lp=/\/lp	=/
s/\/X=/\/X	=/
s/=/	/
s/(/		/
s/)/	/
s/>//
}'
echo "----------------------------------------------------------------------------"
}

mkopmon()
{
echo "Would you like to build a new DOS Opmon with the new configuration ? (y/n) : \c"
read ANS
if [ "$ANS" = "y" -o "$ANS" = "Y" ]; then
	/etc/mkopmon
fi
}

# MAIN MENU LOOP

# start of driver menu loop
while [ true ]; do

# Display current configuration with parameters
	list_conf
echo
echo "		(a)dd a driver"
echo "		(d)elete a driver"
echo "		(s)ave the new configuration and exit this program"
echo "		(q)uit without updating original configuration"
echo
echo "	Select a letter to edit the configuration table : \c"
read ANS
case "$ANS" in
	a* | A* ) add_driver
	;;
	d* | D* ) del_driver
	;;
	s* | S* ) wr_config; rm /tmp/*$$; mkopmon; exit 0
	;;
	q* | Q* ) mv $OLDCFG $CONFIG; rm /tmp/*$$; exit 0
	;;
	* ) echo "Not a choice - try again."; sleep 3
	;;
esac
# end of main menu loop
done
