
# Make the linkable object files from all master files in /etc/master.d

for i in $*
do
	case $i in
		"-m" ) shift; MASDIR=$1; shift
			;;
		"-d" ) shift; OUTDIR=$1; shift
			;;
		"-s" ) shift; SYSTEM=$1; shift
			;;
		* ) DRIVER=$i
			;;
	esac
done

MASDIR=${MASDIR:=/etc/master.d}
OUTDIR=${OUTDIR:=/boot}
SYSTEM=${SYSTEM:=/etc/system}

if [ ! -d $MASDIR ]; then
	echo "$MASDIR : bad directory"
	exit 255
fi
if [ ! -d $OUTDIR ]; then
	echo "$OUTDIR : bad directory"
	exit 255
fi
if [ ! -f $SYSTEM ]; then
	echo "$SYSTEM : bad file"
	exit 255
fi

MASTER=`echo $DRIVER | sed "s/^.*\/\(.*\)$/\1/" | tr '[A-Z]' '[a-z]' |
	sed 's/^\(.*\)\.o/\1/'`
DEF=`echo $MASTER | tr '[a-z]' '[A-Z]'`
grep "#define[	 ]*$DEF" $SYSTEM > /dev/null 2>&1
INSTALLED=$?
cp ${MASDIR}/${MASTER} /tmp/driver.c
if [ $INSTALLED -eq 0 ]; then
	cd /tmp; cc -c -Dopus driver.c
	OFILE=${OUTDIR}/${MASTER}.o
	if [ $? -eq 0 ]; then
		rm ${DRIVER} > /dev/null 2>&1
		echo "	- ${MASDIR}/${MASTER}"
		echo "	- ${OFILE}"
		ld -rd -o ${DRIVER} /tmp/driver.o ${OFILE}
	else
		echo "mkboot: Missing driver file - ${OUTDIR}/${MASTER}.o"
		rm /tmp/driver.c /tmp/driver.o > /dev/null 2>&1
		exit 255
	fi
		
else
	echo "	- ${MASDIR}/${MASTER}"
	cd /tmp; cc -c driver.c
	mv /tmp/driver.o ${DRIVER}
fi
rm /tmp/driver.c /tmp/driver.o > /dev/null 2>&1

echo "Linkable driver \"${DRIVER}\" completed."
exit 0
