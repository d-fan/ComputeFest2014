# arguments: num_times_to_run, first_bot, second_bot, output_name

for ((i = 1; i <= $1; i++)); do
    ./$2 anotherstring | tail -1 >> $4 & ./$3 anotherstring > /dev/null
    echo $i
done
grep WIN $4 | wc -l
