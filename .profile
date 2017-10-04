#                     __ _ _      
#    _ __  _ __ ___  / _(_) | ___ 
#   | '_ \| '__/ _ \| |_| | |/ _ \
#  _| |_) | | | (_) |  _| | |  __/
# (_) .__/|_|  \___/|_| |_|_|\___|
#   |_|
# sh/ksh/bash/zsh initialization

. ~/.shell_funcs # my custom funcs and misc customizations

#######---------------- COMMON ENV VARIABLES

PATH=$HOME/bin:$PATH
if contains "$0" "bash" || contains "$0" "-ksh" ; then 
	PS1="\n\u@\H \n\w \\$ " # bash and OpenBSD-ksh
elif contains "$0" "-mksh" ; then 
	PS1=$'$USER@`hostname` \n${PWD/$HOME/~} \$ ' # for mksh
elif contains "$0" "zsh"  ; then 
	PS1='$ ' # this is set elsewhere, later, for zsh
fi
PS2='> '
PS3='> '
PS4='+ '
LC_CTYPE="en_US.UTF-8"
LANG=$LC_CTYPE
if contains "$0" "bash" ; then 
	HISTFILE=$HOME/.bash_history
elif contains "$0" "-ksh" || contains "$0" "-mksh" ; then 
	HISTFILE=$HOME/.ksh_history
elif contains "$0" "zsh" ; then 
	HISTFILE=$HOME/.zsh_history
fi
EDITOR='vim'
PAGER=less
BROWSER=firefox
CLICOLOR=1
VISUAL=$EDITOR
UNAME=`uname | tr -d '\n\r\v\t'`
SHORTHOSTNAME=`hostname -s`
XDG_CONFIG_HOME="$HOME/.config"
IS_THIS_A_MAC=FALSE
IS_THIS_ITERM2=FALSE
case "$UNAME" in
	Darwin)
		IS_THIS_A_MAC=TRUE
		;;
	*)
		IS_THIS_A_MAC=FALSE
		;;
esac

#######---------------- ALIASES

case "$UNAME" in
	Linux)
		alias ll="ls -la --color=auto"
		;;
	FreeBSD|Darwin)
		alias ll="ls -laFG"
		;;
	OpenBSD)
		alias ll="colorls -laGF"
		;;
	*MSYS*|*CYGWIN*)
		alias ll="ls -la --color=auto"
		;;
	*)
		alias ll="ls -la"
		;;
esac

alias insecssh='ssh -o "StrictHostKeyChecking=no" -o "UserKnownHostsFile=/dev/null" -o "PreferredAuthentications=keyboard-interactive"'
alias rsh='ssh -F ~/.ssh/proxyconfig -l root -p2222'
alias rsftp='sftp -o port=2222 -F ~/.ssh/proxyconfig'
alias startx='startx -- -listen tcp -dpi 100'
which gpg >/dev/null 2>&1 ||  alias gpg='gpg2'
alias tmux='tmux -2'
if contains "$IS_THIS_A_MAC" "TRUE" ; then 
	alias ic='cd ~/Library/Mobile\ Documents/com~apple~CloudDocs'
	alias icloud='cd ~/Library/Mobile\ Documents/com~apple~CloudDocs'
fi

#######---------------- iterm2 integrations if shell is bash
if contains "$0" "bash" ; then 
	#test -e "${HOME}/.iterm2_shell_integration.bash" && source "${HOME}/.iterm2_shell_integration.bash"
	if test -e "${HOME}/.iterm2_shell_integration.bash" ; then
		source "${HOME}/.iterm2_shell_integration.bash"
		if test -e "${HOME}/.iterm2/it2check" && ${HOME}/.iterm2/it2check ; then
			IS_THIS_ITERM2=TRUE
			alias tmux='tmux -CC'
			#test -e "${HOME}/.iterm2/it2attention" && ${HOME}/.iterm2/it2attention fireworks
			test -e "${HOME}/.iterm2/it2attention" && ${HOME}/.iterm2/it2attention start
		else
			alias tmux='tmux'
		fi
	fi
fi

#######---------------- EXPORTS AND SHELL OPTS
export PATH HOME TERM EDITOR HISTFILE LC_CTYPE LANG PS1 PS2 PS3 PS4 PAGER BROWSER CLICOLOR VISUAL UNAME XDG_CONFIG_HOME SHORTHOSTNAME IS_THIS_A_MAC IS_THIS_ITERM2
test "$0" != "zsh" && set -o emacs
umask 027

#######---------------- SSH KEYS
if contains "$IS_THIS_A_MAC" "FALSE" ; then 
	id1=$HOME/.ssh/identity
	id2=$HOME/.ssh/id_dsa
	id3=$HOME/.ssh/id_rsa
	id4=$HOME/.ssh/id_ecdsa
	id5=$HOME/.ssh/id_ed25519

	if [ -z "$SSH_AGENT_PID" ]; then
		if [ -x /usr/bin/ssh-agent ] && [ -f $id1 -o -f $id2 -o -f $id3 -o -f $id4 -o -f $id5 ]; then
			eval `ssh-agent -s`
			ssh-add < /dev/null
		fi
	fi
fi

#######---------------- PERLBREW
PERLBREWFILE=$HOME/perl5/perlbrew/etc/bashrc
[[ -e "$PERLBREWFILE" ]] && . $PERLBREWFILE

#######---------------- X11/wm stuff

. ~/.wm_vars

# EOF
