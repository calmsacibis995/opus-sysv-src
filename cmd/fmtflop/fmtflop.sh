
# DOS format a diskette

DRIVE=${1:-"a:"}
if [ $DRIVE = "-v" ]; then
	shift
	DRIVE=${1:-"a:"}
fi

FARG=${DRIVE}

case $DRIVE in
	*flpa | *a: | *A: | a | A ) FARG="a:"
	;;
	*flpb | *b: | *B: | b | B ) FARG="b:"
	;;
esac

/opus/bin/dos format "$FARG"
echo
