#                     __ _ _      
#    _ __  _ __ ___  / _(_) | ___ 
#   | '_ \| '__/ _ \| |_| | |/ _ \
#  _| |_) | | | (_) |  _| | |  __/
# (_) .__/|_|  \___/|_| |_|_|\___|
#   |_|
# sh/ksh/bash/zsh initialization

. ~/.shell_funcs # load our funcs first

#######---------------- COMMON ENV VARIABLES

# PD KSH and OpenBSD-KSH:
# $ echo $KSH_VERSION
# @(#)PD KSH v5.2.14 99/07/13.2

# REAL KSH:
# $ echo $KSH_VERSION
# Version A 2020.0.0

# MirBSD KSH:
# $ echo $KSH_VERSION
# @(#)MIRBSD KSH R59 2020/10/31

REAL_KSH=False
[ "${KSH_VERSION:-unset}" = unset ] || \
	contains "$KSH_VERSION" "-ksh" || \
	contains "$KSH_VERSION" "-mksh" || \
	export REAL_KSH=TRUE


PATH=$HOME/bin:$PATH
if contains "$0" "bash" || contains "$0" "-ksh" ; then 
	PS1="\n\u@\H \n\w \\$ " # bash and OpenBSD-ksh
elif contains "$0" "-mksh" ; then 
	PS1=$'$USER@`hostname` \n${PWD/$HOME/~} \$ ' # for mksh
elif contains "$0" "ksh" && contains "$REAL_KSH" "TRUE" ; then 
	PS1=$'`whoami`@`hostname` \n${PWD/$HOME/~} \$ ' # for real KSH
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
	HISTSIZE=10000
	HISTFILESIZE=99999
	shopt -s histappend
elif contains "$0" "ksh" ; then 
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
EMAIL="glenn@wurr.net"
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
RANGER_LOAD_DEFAULT_RC=FALSE
# fix java windows
_JAVA_AWT_WM_NONREPARENTING=1

#######---------------- ALIASES

case "$UNAME" in
	Linux|SunOS)
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
alias startx='startx -- -listen tcp -dpi 100'
command -v gpg >/dev/null 2>&1 ||  alias gpg='gpg2'
alias tmux='tmux -2'

if command -v gfind >/dev/null 2>&1 ; then
	alias find='gfind'
fi

if test -x "$(command -v nvim)" ; then
	alias vim='nvim'
fi

if test -x "$(command -v gsed)" ; then
	alias sed='gsed'
fi

alias encrypt='gpg --use-agent -c'
alias decrypt='gpg --use-agent --batch -d'

alias dotfiles='git --git-dir=$HOME/.cfg/ --work-tree=$HOME' # this is for the git repo that this file is a part of
alias dotfiles-update='dotfiles status && dotfiles pull && dotfiles submodule init && dotfiles submodule update && dotfiles submodule status && dotfiles status'
alias dotfiles-check='dotfiles status -unormal' # so we can see untracked files without recursively listing entire homedir
alias dotf='dotfiles'
alias dotfc='dotfiles-check'


#######---------------- GOLANG
if test -d "${HOME}/go/bin" ; then
	PATH=$HOME/go/bin:$PATH
fi

#######---------------- Some MacOS ricing
if contains "$IS_THIS_A_MAC" "TRUE" && test -e "${HOME}/.profile.macos" ; then
	. ${HOME}/.profile.macos
fi

#######---------------- EXPORTS AND SHELL OPTS

export PATH HOME TERM EDITOR HISTFILE HISTSIZE HISTFILESIZE LC_CTYPE LANG PS1 PS2 PS3 PS4 PAGER BROWSER CLICOLOR VISUAL UNAME XDG_CONFIG_HOME SHORTHOSTNAME EMAIL IS_THIS_A_MAC IS_THIS_ITERM2 RANGER_LOAD_DEFAULT_RC _JAVA_AWT_WM_NONREPARENTING

test "$0" != "zsh" && set -o emacs
umask 022

#######---------------- Bash-specific ricing
if contains "$0" "bash" && test -e "${HOME}/.profile.bashisms" ; then 
	. ${HOME}/.profile.bashisms
fi

#######---------------- PERLBREW
PERLBREWFILE=$HOME/perl5/perlbrew/etc/bashrc
[[ -e "$PERLBREWFILE" ]] && . $PERLBREWFILE

#######---------------- X11/wm stuff
. ~/.wm_vars

#######---------------- Load local profile if exists
if test -e "${HOME}/.profile.local" ; then
	. ${HOME}/.profile.local
fi

#######---------------- Load polyglot git shell rice if exists
#if test -e "${HOME}/.profile.polyglot" ; then
	#. ${HOME}/.profile.polyglot
#fi

# EOF
