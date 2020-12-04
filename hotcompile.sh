file_name=$1
out_name=$2

echo "\n\n\nSTARTING\n\n" >> logs

while sleep 1
do

    echo "============================"
    echo "COMPILING"
    cd build
    cmake -Wno-dev ../GLImac-Template >> ../logs
    make >> ../logs
    cd ..
    echo "COMPILING DONE"
    echo "============================"
    "./$out_name" $3 $4 $5&
    inotifywait -r -q -e modify *
    killall "$(basename $out_name)" >> logs
done

