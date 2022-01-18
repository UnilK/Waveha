map mk :make<CR>

map ml :silent !./waveha<CR><C-l>

map <C-h> G/include<CR> :noh<CR> w w gf

set path=.,include,lib/*/include

nmap qqq :qa<CR>

execute "set <C-S-M-q>=\eq"
execute "set <C-S-M-e>=\ee"
imap <C-S-M-q> :ptag <C-r><C-w><CR>
imap <C-S-M-e> :pc<CR>
nmap <C-S-M-q> :ptag <C-r><C-w><CR>
nmap <C-S-M-e> :pc<CR>

let undo_path = expand('./dev/.undo')
if !isdirectory(undo_path)
    call system('mkdir -p ' undo_path)
endif
let &undodir = undo_path
set undofile
