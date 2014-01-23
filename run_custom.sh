rm temp.out
for ((i = 1; i <= 50; i++)); do
  ./basic_score_player 2.1 13 9 anotherstring | tail -1 >> temp.out & ./strict_player_v2 anotherstring > /dev/null
done
grep WIN temp.out | wc -l
