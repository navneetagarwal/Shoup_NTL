for i in `seq 1 10`
do
	./server $((8010+$i)) < inputs/file_$i.txt
done