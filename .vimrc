"        _                    
" __   _(_)_ __ ___  _ __ ___ 
" \ \ / / | '_ ` _ \| '__/ __|
"  \ V /| | | | | | | | | (__ 
" (_)_/ |_|_| |_| |_|_|  \___|
"     

let mapleader = "\<Space>"
let maplocalleader = "\<Space>"

set nocompatible
filetype plugin indent on
syntax on

execute pathogen#infect()

"set nu
set rnu
"set backspace=2
set backspace=indent,eol,start  " more powerful backspacing
set virtualedit=onemore,block  " make selecting way better
set nosol " don't always go to start-of-line with move commands

set history=50          " keep 50 lines of command line history
set ruler               " show the cursor position all the time
set showcmd             " display incomplete commands
set showmode		" show current mode
set incsearch           " do incremental searching
set laststatus=2
set encoding=utf-8
syntax enable
"set t_Co=8
"set background=dark
set background=light	
"colorscheme base16-eighties
"let base16colorspace=256
"
" swap BG light/dark
map <Leader>bg :let &background = ( &background == "dark"? "light" : "dark" )<CR>


" Suffixes that get lower priority when doing tab completion for filenames.
" " These are files we are not likely to want to edit or read.
set suffixes=.bak,~,.swp,.o,.info,.aux,.log,.dvi,.bbl,.blg,.brf,.cb,.ind,.idx,.ilg,.inx,.out,.toc,.png,.jpg
"
if has('gui_running')
  " Make shift-insert work like in Xterm
  map <S-Insert> <MiddleMouse>
  map! <S-Insert> <MiddleMouse>
endif

" In many terminal emulators the mouse works just fine, thus enable it.
if has('mouse')
  set mouse=a
endif
" actually, I don't really like mouse support
set mouse=""

" Switch syntax highlighting on, when the terminal has colors
" Also switch on highlighting the last used search pattern.
if &t_Co > 2 || has("gui_running")
  syntax on
  set hlsearch
endif

filetype plugin indent on

"== make Q execute what is on the line
noremap Q !!sh<CR>



"== make us some diff commands so we can see diff of current loaded file and
"== pending changes without having to leave vim or save a temp file
command Udiff :w !diff -u % -

noremap <F8> :Udiff<CR>

"====[ Make tabs, trailing whitespace, and non-breaking spaces visible ]======

exec "set listchars=tab:\uBB\uBB,trail:\uB7,nbsp:~"
"set list
" toggle it with F4
noremap <F4> :set list!<CR>

noremap <F5> :set rnu!<CR> " relative line numbering
noremap <F6> :setlocal spell! spelllang=en_us<CR> " spell checking

" F3 toggles paste mode
set pastetoggle=<F3>

" map some shit for plugins

inoremap <expr>  <C-K>   BDG_GetDigraph()


"vmap <expr>  ++  VMATH_YankAndAnalyse()
nmap         ++  vipy==
nmap <expr>  ==  VMATH_Analyse()

vmap  <expr>  <S-LEFT>   DVB_Drag('left')
vmap  <expr>  <S-RIGHT>  DVB_Drag('right')
vmap  <expr>  <S-DOWN>   DVB_Drag('down')
vmap  <expr>  <S-UP>     DVB_Drag('up')
vmap  <expr>  D        DVB_Duplicate()

" let g:DVB_TrimWS = 1



"====[ Make the 81st column stand out ]====================

"    " EITHER the entire 81st column, full-screen...
"    highlight ColorColumn ctermbg=magenta
"    set colorcolumn=81

    " OR ELSE just the 81st column of wide lines...
    highlight ColorColumn ctermbg=magenta
    call matchadd('ColorColumn', '\%81v', 100)

    "    " OR ELSE on April Fools day...
    "    highlight ColorColumn ctermbg=red ctermfg=blue
    "    exec 'set colorcolumn=' . join(range(2,80,3), ',')


"=====[ Highlight matches when jumping to next ]=============

    " This rewires n and N to do the highlighing...
    nnoremap <silent> n   n:call HLNext(0.2)<cr>
    nnoremap <silent> N   N:call HLNext(0.2)<cr>


"    " EITHER blink the line containing the match...
"    function! HLNext (blinktime)
"        set invcursorline
"        redraw
"        exec 'sleep ' . float2nr(a:blinktime * 1000) . 'm'
"        set invcursorline
"        redraw
"    endfunction
"
"    " OR ELSE ring the match in red...
    function! HLNext (blinktime)
        highlight RedOnRed ctermfg=red ctermbg=red
        let [bufnum, lnum, col, off] = getpos('.')
        let matchlen = strlen(matchstr(strpart(getline('.'),col-1),@/))
        echo matchlen
        let ring_pat = (lnum > 1 ? '\%'.(lnum-1).'l\%>'.max([col-4,1]) .'v\%<'.(col+matchlen+3).'v.\|' : '')
                \ . '\%'.lnum.'l\%>'.max([col-4,1]) .'v\%<'.col.'v.'
                \ . '\|'
                \ . '\%'.lnum.'l\%>'.max([col+matchlen-1,1]) .'v\%<'.(col+matchlen+3).'v.'
                \ . '\|'
                \ . '\%'.(lnum+1).'l\%>'.max([col-4,1]) .'v\%<'.(col+matchlen+3).'v.'
        let ring = matchadd('RedOnRed', ring_pat, 101)
        redraw
        exec 'sleep ' . float2nr(a:blinktime * 1000) . 'm'
        call matchdelete(ring)
        redraw
    endfunction
"
    " OR ELSE briefly hide everything except the match...
"    function! HLNext (blinktime)
"        highlight BlackOnBlack ctermfg=black ctermbg=black
"        let [bufnum, lnum, col, off] = getpos('.')
"        let matchlen = strlen(matchstr(strpart(getline('.'),col-1),@/))
"        let hide_pat = '\%<'.lnum.'l.'
"                \ . '\|'
"                \ . '\%'.lnum.'l\%<'.col.'v.'
"                \ . '\|'
"                \ . '\%'.lnum.'l\%>'.(col+matchlen-1).'v.'
"                \ . '\|'
"                \ . '\%>'.lnum.'l.'
"        let ring = matchadd('BlackOnBlack', hide_pat, 101)
"        redraw
"        exec 'sleep ' . float2nr(a:blinktime * 1000) . 'm'
"        call matchdelete(ring)
"        redraw
"    endfunction


"    " OR ELSE just highlight the match in red...
"    function! HLNext (blinktime)
"        let [bufnum, lnum, col, off] = getpos('.')
"        let matchlen = strlen(matchstr(strpart(getline('.'),col-1),@/))
"        let target_pat = '\c\%#\%('.@/.'\)'
"        let ring = matchadd('WhiteOnRed', target_pat, 101)
"        redraw
"        exec 'sleep ' . float2nr(a:blinktime * 1000) . 'm'
"        call matchdelete(ring)
"        redraw
"    endfunction
"



" some plugin stuff
"

" ignore old vim warning for vim-go
let g:go_version_warning = 0

" easymotion
let g:EasyMotion_do_mapping = 0 " Disable default mappings

nmap s <Plug>(easymotion-overwin-f2)
" Bidirectional & within line 't' motion
omap t <Plug>(easymotion-bd-tl)

" Turn on case insensitive feature
let g:EasyMotion_smartcase = 1
" Smartsign (type `3` and match `3`&`#`)
let g:EasyMotion_use_smartsign_us = 1

" HJKL motions: Line motions
map <Leader>l <Plug>(easymotion-lineforward)
map <Leader>j <Plug>(easymotion-j)
map <Leader>k <Plug>(easymotion-k)
map <Leader>h <Plug>(easymotion-linebackward)

let g:EasyMotion_startofline = 0 " keep cursor column when JK motion

let g:org_indent = 1 

let g:vimwiki_folding='list'

" SuperRetab function:
:command! -nargs=1 -range SuperRetab <line1>,<line2>s/\v%(^ *)@<= {<args>}/\t/g
" usage for 2 space indent would be :'<,'>SuperRetab 2
