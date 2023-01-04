
# mkunix

BOOTDIR=/boot
MASDIR=/etc/master.d
LIBDIR=/boot/libs
SYSTEM=/etc/system
NAME=unix

if [ ! -f ${BOOTDIR}/conf.c ]; then
	echo "Missing ${BOOTDIR}/conf.c."
	exit 255
else
	cd ${BOOTDIR}; cc -c -Dopus conf.c; mv conf.o CONF

	if [ $? -ne 0 ]; then
		echo "Compile of conf.c failed."
		exit 255
	fi
fi

# Build date file
	echo 'char verstamp[] = "' `date` '";' > ${BOOTDIR}/date.c
	cc -c ${BOOTDIR}/date.c
	rm -f ${BOOTDIR}/date.c
	mv ${BOOTDIR}/date.o ${BOOTDIR}/DATE

if [ ! -f ${MASDIR}/kernel ]; then
	echo "Missing ${MASDIR}/kernel."
	exit 255
else
	cp ${MASDIR}/kernel /tmp/kernel.c
	cd /tmp; cc -c kernel.c
	mv /tmp/kernel.o ${BOOTDIR}/KERNEL
	if [ $? -ne 0 ]; then
		echo "Compile of ${MASDIR}/kernel failed."
		exit 255
	fi
	rm /tmp/kernel.c
fi

# Only use files define'd or undef'd in the system file
DEF=`cat ${SYSTEM} | grep '^#define' |
sed 's/^#define[	 ]*\([A-Z0-9]*\)[	 ].*$/\1/
/CONF/d
/LOCORE/d
/LD.FILE/d
/KERNEL/d'`

UNDEF=`cat ${SYSTEM} | grep '^#undef' |
sed 's/^#undef[	 ]*\([A-Z0-9]*\)$/\1/
/CONF/d
/LOCORE/d
/LD.FILE/d
/KERNEL/d'`

FILES=`echo $DEF $UNDEF`

for i in $FILES
do
	if [ ! -f ${BOOTDIR}/$i ]; then
		echo "mkunix: Missing driver file - ${BOOTDIR}/$i"
		exit 255
	fi
done

if clipper; then
cd $BOOTDIR; /bin/ld -o ${NAME} LD.FILE -x LOCORE KERNEL CONF DATE $FILES KLIB > /tmp/x$$ 2>&1
cat /tmp/x$$ | grep -v 'redefined$'
rm /tmp/x$$
else
cd $BOOTDIR; /bin/ld -o ${NAME} LD.FILE -e start -x LOCORE KERNEL CONF DATE $FILES KLIB
fi
RTN=$?
if [ $RTN -ne 0 ]; then
	echo "mkunix: link failed."
	exit 1
else
	if [ -z "$1" ]; then
		echo "New kernel is: ${BOOTDIR}/${NAME}"
		exit 0
	elif [ "$1" = "install" ]; then
		echo "Installing /boot/unix as /unix (old /unix saved as /ounix)."
		if [ -f /unix ]; then
			mv /unix /ounix
		fi
		cp /boot/unix /unix
		chmod 444 /unix
		chown root /unix
		chgrp sys /unix
		echo "Installing new \\opus\\unix in DOS (old saved as \\opus\\ounix)."
		dos REMOTE cd \\opus
		dos REMOTE if exist unix copy unix ounix
		udcp /unix \\opus\\unix
		exit 0
	else
		echo "$1: unknown option - New kernel is: ${BOOTDIR}/${NAME}"
	fi
fi
