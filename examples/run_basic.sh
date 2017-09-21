for VAR in $(find | grep '0._.*\.ll')
do
    F=$(basename $VAR)
    timeout 5 ../Angie/bin/x64-Debug/Angie.exe -f $F
    echo $F : $?
done
