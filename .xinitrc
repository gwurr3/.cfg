#!/bin/sh
#         _       _ _            
#   __  _(_)_ __ (_) |_ _ __ ___ 
#   \ \/ / | '_ \| | __| '__/ __|
#  _ >  <| | | | | | |_| | | (__ 
# (_)_/\_\_|_| |_|_|\__|_|  \___|
#                                
# 

run_util() {
	$@ >/dev/null 2>$HOME/utils.log &
}

run_bg() {
	$@ >/dev/null 2>&1 &
}

userresources=$HOME/.Xresources
usermodmap=$HOME/.Xmodmap
sysresources=/etc/X11/xinit/.Xresources
sysmodmap=/etc/X11/xinit/.Xmodmap

# merge in defaults and keymaps

if [ -f $sysresources ]; then
    xrdb -merge $sysresources
fi

if [ -f $sysmodmap ]; then
    xmodmap $sysmodmap
fi

if [ -f "$userresources" ]; then
    xrdb -merge "$userresources"
fi

if [ -f "$usermodmap" ]; then
    xmodmap "$usermodmap"
fi

xset fp+ ~/.fonts
xset fp rehash

xsetroot -solid black

run_util redshift -O 4500k

run_util feh --bg-fill $HOME/.BG.jpg

# screen powersave defaults
xset +dpms
xset dpms 0 0 300

# allow local connections from current user to X
xhost +local:`whoami`



## BEGIN BOX-SPECIFC ZONE ################
case "$SHORTHOSTNAME" in
	fourten)
		#dispwin -d 1 ~/T410/monitorfiles/MONITOR/t410.icc 
		if [[ $(xrandr | grep 'HDMI1 connected') != "" ]]; then
		#	xrandr --output VGA1 --auto --right-of LVDS1
			 xrandr --output HDMI1 --auto --right-of LVDS1
		fi

		xbacklight -set 45

		# deactivate touchpad
		synclient TouchpadOff=1

		# better fonts on FreeBSD
		#xset fp+ /usr/local/share/fonts/urwfonts
		#xset fp rehash

		#synclient TapButton1=1
		#synclient VertEdgeScroll=1

		## activate scroll wheel button on OpenBSD
		#xinput set-prop "/dev/wsmouse" "WS Pointer Wheel Emulation" 1
		#xinput set-prop "/dev/wsmouse" "WS Pointer Wheel Emulation Axes" 6 7 4 5
		#xinput set-prop "/dev/wsmouse" "WS Pointer Wheel Emulation Button" 2
		#xinput set-prop "/dev/wsmouse" "WS Pointer Wheel Emulation Timeout" 50
		#xinput set-prop "/dev/wsmouse" "WS Pointer Wheel Emulation Inertia" 3
		# increase pointer speed
		#xinput set-prop "/dev/wsmouse" "Device Accel Constant Deceleration" 0.6

		;;
	sexbox)
		[[ $(xrandr | grep 'DVI-1 connected') != "" ]] && xrandr --output DVI-1 --mode 1600x1200 --rotate left --left-of DisplayPort-0
		xrandr --output DisplayPort-0 --mode 3840x2160 --rate 30 --primary
		;;
	baldr)
		[[ $(xrandr | grep 'HDMI1 connected') != "" ]] && xrandr --output HDMI1 --auto --left-of VGA1
		;;
esac
## END BOX-SPECIFC ZONE ################

## Startup the WM stuff

# so the programs will get started the first time bspwm starts
[ -f $AUTOSTARTCHECKFILE ] && rm -f $AUTOSTARTCHECKFILE
run_util sxhkd -c ~/.sxhkdrc
while true ; do
	bspwm -c ~/.bspwmrc || break
done
