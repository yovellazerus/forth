\ Print the first 20 Fibonacci numbers

20            \ counter
1 1           \ F0 F1

begin
    dup 0 >           \ check if counter > 0
while
    over . cr         \ print F0
    over over +       \ compute next Fibonacci (F0+F1)
    rot drop          \ drop old F0, leave F1 next
    swap 1 - swap     \ decrement counter without disturbing F1/next order
repeat

drop drop drop        \ clean up remaining F0 F1 counter
