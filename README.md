# README #

These are my dotfiles and misc related things

### SETUP ###

```
git clone --bare https://github.com/gwurr3/.cfg.git $HOME/.cfg
alias dotfiles='git --git-dir=$HOME/.cfg/ --work-tree=$HOME'
alias dotfiles-update='dotfiles status && dotfiles pull && dotfiles submodule init && dotfiles submodule update && dotfiles submodule status && dotfiles status'
dotfiles checkout
dotfiles config status.showUntrackedFiles no
dotfiles-update
```
