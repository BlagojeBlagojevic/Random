```c

read word from input
look up word in dictionary
if found {
  if state = interpret OR word header has immediate flag {
    execute word
  } else {
    compile word
  }
} else {
  if valid number under current base {
    if state = interpret {
      push number to stack
    } else {
      compile LIT
      compile number
    }
  } else {
    error "unknown word 'XXXX'"
  }
}
```
-links:

https://www.bradrodriguez.com/papers/moving1.htm


https://github.com/lkesteloot/trs80/tree/master/apps/forth
