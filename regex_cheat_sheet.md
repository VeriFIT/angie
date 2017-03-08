# make all methods virtual:
```
^(\s+)([^/\s]+.+);(.*)
$1virtual $2 = 0;$3
```

# make all virtual methods virtual override :
```
^(\s+virtual )([^/\s]+.+) = 0(;.*)
$1$2 override$3
```

# make all virtual methods virtual override - multiline:
```
^(\s*virtual )([^/\s]+(?:[^{;]|\n)+) = 0(;.*)
$1$2 override$3
```

# remove all implemented virtual methods and associated comments
# associated comments means comment on previous lines or on last line
```
(?:(?:\s*\/\/.*\n)*|\s*?)(?:[^\S\n]*virtual )(?:[^/\s]+(?:(?:[^{;]|\n)(?!= 0))+)(?:;.*|{[^}]*}.*)
[nothing]
```

# same as before, without "NotImplementedException" implementations 
```
(?:(?:\s*\/\/.*\n)*|\s*?)(?:[^\S\n]*virtual )(?:[^/\s]+(?:(?:[^{;]|\n)(?!= 0))+)(?:;.*|{(?!\s*throw NotImplementedException)[^}]*}.*)
[nothing]
```
