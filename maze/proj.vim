set path+=src
set path+=src/inc
let buildfold='build'
" F5 - 保存、make
map <F5> :w<Esc><CR>:exe 'cd' buildfold<CR>:make<CR>:cd ..<CR>:cw<CR><CR>
imap <F5> <Esc><F5>





