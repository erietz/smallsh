./smallsh <<__FOO
sleep 5 &
ls
sleep 5
sleep 5 &
ls
sleep 5
sleep 5 &
ls
sleep 6
ls
__FOO
