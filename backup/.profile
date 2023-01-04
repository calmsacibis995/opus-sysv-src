stty erase '^h' kill '^x' echoe
. /etc/TIMEZONE
PATH=.:/usr/src/backup:/opus/bin:/bin:/usr/bin:/etc; export PATH
TERM=opus-pc; export TERM
EXINIT='set ai nows nu redraw sm'; export EXINIT
dir() ls -CF $@;
BOLD="[1m" ; export BOLD
CLEAR="`tput clear\c" ; export CLEAR
REVERSE="[7m" ; export REVERSE
UNBOLD="[0m" ; export UNBOLD
