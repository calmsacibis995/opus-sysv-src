LIST=`grep "Build of.*failed" /usr/src/LOG | fgrep -vf /usr/src/checkmake/dont_care_fail | awk '{ print $4 }'`
if [ "$LIST" ]; then
	echo "\n===== The following commands/libraries failed ====="
	echo "Some that we don't care about have been filtered out\n"
	for i in $LIST; do
		echo $i
	done
	echo
fi
for CMD in $LIST; do
	grepmk "===== $CMD" /usr/src/LOG
done
for CMD in $LIST; do
	grepmk "===== $CMD" /usr/src/LOG
done
