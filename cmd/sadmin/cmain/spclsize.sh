
# This script reports the number of available bytes on a dos-formatted
#  diskette. The -b option reports in number of 512 byte blocks.

/opus/bin/dos 'chkdsk a: \ny\ny\n > \opus\FPSIZE'
/opus/bin/ducp -a \\opus\\FPSIZE /tmp/FPSIZE
/opus/bin/dos erase \\opus\\FPSIZE

BYTES=`grep 'bytes total disk space$' /tmp/FPSIZE | \
sed 's/^ *//' | \
cut -d' ' -f1`

if [ "$1" = "-b" ]; then
	BLOCKS=`expr ${BYTES} / 512`
	echo ${BLOCKS}
else
	echo ${BYTES}
fi
