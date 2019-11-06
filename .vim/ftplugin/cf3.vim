
fun! Getchar()
  let c = getchar()
  if c != 0
    let c = nr2char(c)
  endif
  return c
endfun

fun! Eatchar(pat)
   let c = Getchar()
   return (c =~ a:pat) ? '' : c
endfun

"
" Check to see if DisableCF3Ftplugin is defined
" If you only want the syntax plugin add "let g:DisableCF3Ftplugin=1" in
" ~/.vimrc
if exists("g:DisableCF3Ftplugin")
    finish
endif


" Only do this when not done yet for this buffer
if exists("b:loaded_CFE3Ftplugin")
  finish
endif
let b:loaded_CFE3Ftplugin = 1

let s:install_dir = expand('<sfile>:p:h:h')

" =============== Keyword Abbreviations  ===============
" enable keyword abbreviations with by adding 
" "let g:EnableCFE3KeywordAbbreviations=1" to your vimrc
" Convenience function ToggleCFE3KeywordAbbreviations
" mapped to ,i by default to toggle abbreviations on or off
"
function! EnableCFE3KeywordAbbreviations()
    iab <buffer> = =>
    iab <buffer> ba bundle agent
    iab <buffer> bc bundle common
    iab <buffer> bu bundle
    iab <buffer> cano canonify( "<C-R>=Eatchar('\s')<CR>
    iab <buffer> cla classes:
    iab <buffer> comma commands:
    iab <buffer> comme comment => "<C-R>=Eatchar('\s')<CR>
    iab <buffer> exp expression => <C-R>=Eatchar('\s')<CR>
    iab <buffer> fil files:
    iab <buffer> han handle => "<C-R>=Eatchar('\s')<CR>
    iab <buffer> ifv ifvarclass => <C-R>=Eatchar('\s')<CR>
    iab <buffer> met methods:
    iab <buffer> pro processes:
    iab <buffer> rep reports:
    iab <buffer> sli slist => {
    iab <buffer> str string => "<C-R>=Eatchar('\s')<CR>
    iab <buffer> sysw ${sys.workdir}
    iab <buffer> ub usebundle =>
    iab <buffer> var vars:
endfunction

function! DisableCFE3KeywordAbbreviations()
     iunab <buffer> =
     iunab <buffer> ba
     iunab <buffer> bc
     iunab <buffer> bu
     iunab <buffer> cano
     iunab <buffer> cla
     iunab <buffer> comma
     iunab <buffer> comme
     iunab <buffer> exp
     iunab <buffer> fil
     iunab <buffer> han 
     iunab <buffer> ifv
     iunab <buffer> met
     iunab <buffer> pro
     iunab <buffer> rep
     iunab <buffer> sli
     iunab <buffer> str
     iunab <buffer> sysw
     iunab <buffer> ub
     iunab <buffer> var
endfunction

" Default abbreviations off
" to disable let g:EnableCFE3KeywordAbbreviations=1 in ~/.vimrc
if exists('g:EnableCFE3KeywordAbbreviations')
    call EnableCFE3KeywordAbbreviations()
endif

function! ToggleCFE3KeywordAbbreviations()
    if !exists('b:EnableCFE3KeywordAbbreviations')
        let b:EnableCFE3KeywordAbbreviations=1
        call EnableCFE3KeywordAbbreviations()
    else
        unlet b:EnableCFE3KeywordAbbreviations
        call DisableCFE3KeywordAbbreviations()
    endif
endfunction

function! EnableCFE3PermissionFix()
" On Save set the permissions of the edited file so others can't access
    :autocmd BufWritePost *.cf silent !chmod g-w,o-rwx %
endfunction

" Default permission fix off
" To enable permission fixing in your main .vimrc
" let g:EnableCFE3PermissionFix=1
if exists('g:EnableCFE3PermissionFix')
    call EnableCFE3PermissionFix()
endif

" maps
" Toggle KeywordAbbreviations
nnoremap <buffer> ,i :call ToggleCFE3KeywordAbbreviations()<CR>
" Wrap WORD in double quotes
nnoremap <buffer> ,q dE<ESC>i"<ESC>pa"<ESC>
" Insert blank promise
nnoremap <buffer> ,p o""<CR><TAB>handle => "",<CR>comment => ""<ESC>
" quote list items
vnoremap <buffer> ,q :s/^\s*\(.*\)\s*$/"\1",/g<CR>

" Function to align groups of => assignment lines.
" Credit to 'Scripting the Vim editor, Part 2: User-defined functions'
" by Damian Conway
" http://www.ibm.com/developerworks/linux/library/l-vim-script-2/index.html
if !exists("*CF3AlignAssignments")
function CF3AlignAssignments (AOP)
    "Patterns needed to locate assignment operators...
    if a:AOP == 'vars'
       let ASSIGN_OP   = '\(string\|int\|real\|data\|slist\|ilist\|rlist\|expression\|and\|or\|not\|volume\)*\s\+=>'
    else
       let ASSIGN_OP   = '=>'
    endif
    let ASSIGN_LINE = '^\(.\{-}\)\s*\(' . ASSIGN_OP . '\)'

    "Locate block of code to be considered (same indentation, no blanks)
    let indent_pat = '^' . matchstr(getline('.'), '^\s*') . '\S'
    let firstline  = search('^\%('. indent_pat . '\)\@!','bnW') + 1
    let lastline   = search('^\%('. indent_pat . '\)\@!', 'nW') - 1
    if lastline < 0
        let lastline = line('$')
    endif

    "Find the column at which the operators should be aligned...
    let max_align_col = 0
    let max_op_width  = 0
    for linetext in getline(firstline, lastline)
        "Does this line have an assignment in it?
        let left_width = match(linetext, '\s*' . ASSIGN_OP)

        "If so, track the maximal assignment column and operator width...
        if left_width >= 0
            let max_align_col = max([max_align_col, left_width])

            let op_width      = strlen(matchstr(linetext, ASSIGN_OP))
            let max_op_width  = max([max_op_width, op_width+1])
         endif
    endfor

    "Code needed to reformat lines so as to align operators...
    let FORMATTER = '\=printf("%-*s%*s", max_align_col, submatch(1),
    \                                    max_op_width,  submatch(2))'

    " Reformat lines with operators aligned in the appropriate column...
    for linenum in range(firstline, lastline)
        let oldline = getline(linenum)
        let newline = substitute(oldline, ASSIGN_LINE, FORMATTER, "")
        call setline(linenum, newline)
    endfor
endfunction
endif

nnoremap <buffer> <silent>  ,=  :call CF3AlignAssignments("null")<CR>
nnoremap <buffer> <silent>  <ESC>=  :call CF3AlignAssignments("vars")<CR>

