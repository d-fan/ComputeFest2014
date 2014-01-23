for ((s = 11; s <= 12; s = s + 1)); do
  for ((h = 5; h <= 15; h = h + 2)); do
    for ((d = 6; d <= 10; d = d + 2)); do
      rm temp.out
      for ((i = 1; i <= 50; i++)); do
	  t=$(echo ".2 * $s" | bc)
        ./basic_score_player $t $h $d anotherstring | tail -1 >> temp.out & ./strict_player_v2 anotherstring > /dev/null
      done
      echo $t $h $d >> output_file
      grep WIN temp.out | wc -l >> output_file
    done
  done
done
