
# various handy-dandy utilities

DIR=/opus/bin		# where we live
FLPA=/dev/rflpa		# floppy disk device a:
FLPB=/dev/rflpb		# floppy disk device b:
TAP0=/dev/rmt/0m	# tape device mt/0
TAP1=/dev/rmt/1m	# tape device mt/1

FLOP=$FLPA		# default floppy disk device
TAPE=$TAP0		# default tape device

case "$1" in
	a:)		FLOP=$FLPA; shift;;
	b:)		FLOP=$FLPB; shift;;
	mt/0)		TAPE=$TAP0; shift;;
	mt/1)		TAPE=$TAP1; shift;;
esac

case $0 in

	#
	# cls: clear screen
	# dir: display directory in a concise way
	#
	cls|*/cls)	tput clear;;
	dir|*/dir)	if [ "$1" = "" -o "$1" = . ]; then
				echo "`pwd`:"
				elif [ $# -eq 1 -a -d "$1" ]; then
				echo "${1}:"
			fi
			/bin/ls -CF $*;;
	#
	# floppy/tape commands
	# fin/tin:   get files from floppy/tape
	# fout/tout: copy files to floppy/tape
	# fdir/tdir: get file names from floppy/tape
	#
	fin|*/fin)	cpio -icvdumB -F$FLOP $*;;
	fout|*/fout)	find $* -print | sort | cpio -ocvB -F$FLOP;;
	fdir|*/fdir)	cpio -icvtB -F$FLOP $*;;
	tin|*/tin)	cpio -icvdumC512000 -F$TAPE $*;;
	tout|*/tout)	find $* -print | sort | cpio -ocvC512000 -F$TAPE;;
	tdir|*/tdir)	cpio -icvtC512000 -F$TAPE $*;;

	#
	#  misc:  link this file ($DIR/misc) to its various aspects
	#
	$DIR/misc)	ln $DIR/misc $DIR/cls
			ln $DIR/misc $DIR/dir
			ln $DIR/misc $DIR/fin
			ln $DIR/misc $DIR/fout
			ln $DIR/misc $DIR/fdir
			ln $DIR/misc $DIR/tin
			ln $DIR/misc $DIR/tout
			ln $DIR/misc $DIR/tdir;;
	*)	echo $0: unknown command;;
esac
