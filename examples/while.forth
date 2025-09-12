\ Push initial counter
10

\ Loop start
begin
    dup 0 >          \ check if counter > 0
while
    dup .  cr          \ print current counter
    1 -              \ decrement counter
repeat

drop                 \ clean up stack after loop
