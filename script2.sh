for i in `seq 1 10`
do
	time ./client 127.0.0.1 $((8010+$i)) < inputs/inputs
	sleep 2
done