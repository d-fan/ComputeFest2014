# arguments: num_times_to_run, first_bot, second_bot, output_name

#rm emp.out
#for ((d = 10000; d <= 30000; d = d + 1000)); do
#echo $d
for ((i = 1; i <= 100; i++)); do
    ./counter_and_ideal_player 10 jkl >> colinsteam.txt
# | tail -1 >> temp.out & ./counter_and_ideal_player 10 teststring -q > /dev/null
    tail -1 colinsteam.txt
    echo $i
done
#grep WIN temp.out | wc -l
#rm temp.out
#done
