./smallsh <<__FOO
echo running 4 sleep and 4 ls commands
sleep 5 &
ls
sleep 5 &
ls
sleep 5 &
ls
sleep 6
ls
__FOO
