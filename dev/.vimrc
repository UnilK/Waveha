"compile
map mk :silent !echo %<Bar>sed -r "s/(.+)\/(.+)\..+/-d \1 \2/"<Bar>xargs ./dev/compile<CR><C-l>

"switch compilation flags
map fl :silent !echo %<Bar>sed -r "s/(.+)\/(.+)\..+/-d \1 -o \2/"<Bar>xargs ./dev/compile<CR><C-l>

"run
map ml :silent !echo %<Bar>sed -r "s/.+\/(.+)\.(.+)/\1 -f \2/"<Bar>xargs ./dev/run<CR><C-l>

"debug with gdb
map mj :silent !echo %<Bar>sed -r "s/.+\/(.+)\..+/\1/"<Bar>xargs ./dev/debug<CR><C-l>

"switch to headerfile. <C-o> switches back to source.
map <C-h> G/include<CR> :noh<CR> w w gf

"included jump paths
set path=.,include,lib/*/include

"close tabs by holding down q
nmap qqq :qa<CR>

"ctags
execute "set <C-S-M-q>=\eq"
execute "set <C-S-M-e>=\ee"
imap <C-S-M-q> :ptag <C-r><C-w><CR>
imap <C-S-M-e> :pc<CR>
nmap <C-S-M-q> :ptag <C-r><C-w><CR>
nmap <C-S-M-e> :pc<CR>

"presistent undo
let undo_path = expand('./dev/.undo')
if !isdirectory(undo_path)
    call system('mkdir -p ' undo_path)
endif
let &undodir = undo_path

autocmd BufEnter * set undofile

