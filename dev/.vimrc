"compile
map mk :silent !dev/compile<CR><C-l>

"update dependencies
map nk :silent !dev/dependencies<CR><C-l>

"switch compilation flags
map fl :silent !dev/flag<CR><C-l>

"run
map ml :silent !dev/run<CR><C-l>

"run with automatically closing debug terminal
map nl :silent !dev/run -w<CR><C-l>

"debug with gdb
map mj :silent !dev/debug<CR><C-l>

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

set undofile
autocmd BufEnter * set undofile
autocmd BufAdd * set undofile
autocmd BufNew * set undofile
autocmd BufRead * set undofile
autocmd BufWinEnter * set undofile

