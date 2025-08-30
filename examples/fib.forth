\ Initialize first two Fibonacci numbers
0 1 20  \ stack: F0 F1 count

begin
    dup 0 >      \ check if count > 0
while
    over .       \ print the current number (top of stack)
    over over +  \ compute next Fibonacci number: F(n-1) + F(n-2)
    rot drop     \ drop the oldest number, keep last two
    1-           \ decrement the counter
repeat

drop drop       \ clean up remaining numbers from the stack
