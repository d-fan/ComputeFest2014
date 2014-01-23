# arguments: num_times_to_run, first_bot, second_bot, output_name

for ((s = 1.0; s <= 1.2; s = s + 0.2)); do
  for ((h = 5; h <= 15; h = h + 2)); do
    for ((d = 6; d <= 10; d = d + 2)); do
      rm $4
      for ((i = 1; i <= 50; i++)); do
        ./$2 anotherstring | tail -1 >> $4 & ./$3 anotherstring > /dev/null
      done
      echo $s $h $d >> output_file
      grep WIN $4 | wc -l
    done
  done
done
