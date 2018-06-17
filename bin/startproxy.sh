#!/bin/sh

case "$SHORTHOSTNAME" in
        sexbox|fourten)
		ssh -o UserKnownHostsFile=/dev/null -o StrictHostKeyChecking=no -N -D 10010 dank &
		ssh -o UserKnownHostsFile=/dev/null -o StrictHostKeyChecking=no -N -D 11111 dank & 
		;;
	baldr)
		ssh -o UserKnownHostsFile=/dev/null -o StrictHostKeyChecking=no -N -p 2222 -D 10010 glenn@lancer &
		ssh -o UserKnownHostsFile=/dev/null -o StrictHostKeyChecking=no -N -D 11111 glenn@lancer & 
		ssh -o UserKnownHostsFile=/dev/null -o StrictHostKeyChecking=no -f -L 5901:127.0.0.1:5902 xlr -N
		;;
	*)

		;;

esac

# example IRC proxy
# ssh -N -L 6667:127.0.0.1:6667 

