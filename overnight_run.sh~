rm output_file
for ((s = 1.0; s <= 1.2; s = s + 0.2)); do
  for ((h = 5; h <= 15; h = h + 2)); do
    for ((d = 6; d <= 10; d = d + 2)); do
      rm temp.out
      for ((i = 1; i <= 50; i++)); do
        ./basic_score_player $s $h $d anotherstring | tail -1 >> temp.out & ./strict_player_v2 anotherstring > /dev/null
      done
      echo $s $h $d >> output_file
      grep WIN temp.out | wc -l >> output_file
    done
  done
done
