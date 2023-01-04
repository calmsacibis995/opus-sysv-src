FNAME=`eval echo \\$$#`
/bin/sifilter -m $FNAME /tmp/sifilt$$
mv /tmp/sifilt$$ $FNAME
/lib/as $@
