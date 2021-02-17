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

case "$SHORTHOSTNAME" in
	LAX-C02Z223WLVCJ)
		/opt/local/bin/quartz-wm &
		exec /opt/local/bin/xterm -ls &
		exit 0
		;;
esac


# screen powersave defaults
xset +dpms
xset dpms 0 0 3600

# allow local connections from current user to X
xhost +local:`whoami`


# set that DPI Boiii
xrandr --dpi 90

## BEGIN BOX-SPECIFC ZONE ################
case "$SHORTHOSTNAME" in
	lappytop)
		xbacklight -set 45
		if [[ $(xrandr | grep 'HDMI1 connected') != "" ]]; then
			 xrandr --output HDMI1 --auto --right-of LVDS1
		fi

		;;
	t410)
		#dispwin -d 1 ~/T410/monitorfiles/MONITOR/t410.icc 
		if cat /proc/acpi/button/lid/LID/state | grep -q closed ; then 
			echo yes
			xrandr --output LVDS1 --off
		fi
		
		if xrandr --listactivemonitors | grep -q LVDS1 ; then
			# need to see if we set it to inactive ^
			if [[ $(xrandr | grep 'LVDS1 connected') != "" ]] && \
				[[ $(xrandr | grep 'VGA1 connected') != "" ]] && \
				[[ $(xrandr | grep 'HDMI1 connected') != "" ]]; then
				xrandr --output LVDS1 --auto
				xrandr --output VGA1 --auto --left-of LVDS1
				xrandr --output HDMI1 --auto --left-of VGA1

			elif [[ $(xrandr | grep 'LVDS1 connected') != "" ]] && \
				[[ $(xrandr | grep 'VGA1 connected') != "" ]]; then
				xrandr --output LVDS1 --auto
				xrandr --output VGA1 --auto --left-of LVDS1

			elif [[ $(xrandr | grep 'LVDS1 connected') != "" ]] && \
				[[ $(xrandr | grep 'HDMI1 connected') != "" ]]; then
				xrandr --output LVDS1 --auto
				xrandr --output HDMI1 --auto --left-of LVDS1
			fi
		fi
		if [[ $(xrandr | grep 'HDMI1 connected') != "" ]] && \
			[[ $(xrandr | grep 'VGA1 connected') != "" ]]; then
			xrandr --output VGA1 --auto
			xrandr --output HDMI1 --auto --left-of VGA1
		fi


		xbacklight -set 45

		# deactivate touchpad
		synclient TouchpadOff=1

		# better fonts on FreeBSD
		#xset fp+ /usr/local/share/fonts/urwfonts
		#xset fp rehash

		synclient TapButton1=1
		synclient VertEdgeScroll=1

		# ## activate scroll wheel button on OpenBSD
		# xinput set-prop "/dev/wsmouse" "WS Pointer Wheel Emulation" 1
		# xinput set-prop "/dev/wsmouse" "WS Pointer Wheel Emulation Axes" 6 7 4 5
		# xinput set-prop "/dev/wsmouse" "WS Pointer Wheel Emulation Button" 2
		# xinput set-prop "/dev/wsmouse" "WS Pointer Wheel Emulation Timeout" 50
		# xinput set-prop "/dev/wsmouse" "WS Pointer Wheel Emulation Inertia" 3
		# # increase pointer speed
		# xinput set-prop "/dev/wsmouse" "Device Accel Constant Deceleration" 0.6
		# disable system beep
		xset b off
		# use caps lock as ctrl key
		setxkbmap -option ctrl:nocaps

		;;
	sexbox)
		[[ $(xrandr | grep 'DVI-1 connected') != "" ]] && xrandr --output DVI-1 --mode 1600x1200 --rotate left --left-of DisplayPort-0
		xrandr --output DisplayPort-0 --mode 3840x2160 --rate 30 --primary
		;;
	thor)
		#[[ $(xrandr | grep 'DVI-D-1 connected') != "" ]] && xrandr --output DVI-D-1 --mode 1280x1024 --rotate left --left-of DVI-D-0
		#[[ $(xrandr | grep 'DisplayPort-0 connected') != "" ]] && xrandr --output DisplayPort-0 --mode 1280x1024 --rotate left --left-of DVI-D-1
		#xrandr --output DVI-D-0 --mode 1920x1080 --primary
		[[ $(xrandr | grep 'DVI-1 connected') != "" ]] && xrandr --output DVI-1 --mode 1600x1200 --rotate left --left-of DisplayPort-0
		xrandr --output DisplayPort-0 --mode 3840x2160 --rate 30 --primary
		;;
	baldr)
		[[ $(xrandr | grep 'HDMI1 connected') != "" ]] && xrandr --output HDMI1 --auto --left-of DP1
		;;
esac


case "$SHORTHOSTNAME" in
	t410)
		#exec ~/.xinitrc_cde
		#exec fvwm
		;;
esac



## END BOX-SPECIFC ZONE ################

# color temp and bg AFTER screen setup is done
run_util redshift -O 4500k
run_util feh --bg-fill $HOME/.BG.jpg

# crypto agents
run_util gpg-agent --daemon
eval $(ssh-agent)


## Startup the WM stuff
# so the programs will get started the first time bspwm starts
[ -f $AUTOSTARTCHECKFILE ] && rm -f $AUTOSTARTCHECKFILE
run_util sxhkd -c ~/.sxhkdrc
while true ; do
	bspwm -c ~/.bspwmrc 2>$HOME/bspwm.log || break
done
