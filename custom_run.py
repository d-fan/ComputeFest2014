import os
import sys
import subprocess
import re
import threading

search = re.compile("WIN", re.MULTILINE)



player1 = sys.argv[1]
player2 = sys.argv[2]
game_name = sys.argv[3]
num_runs = int(sys.argv[4])

players = []

results = [0,0]

def run_game(command, name, player_num):
	#print("Running: " + command[0])
	result = subprocess.check_output(command.split(' '))
	win_lose = result[-8:-4]
	if(player_num == 0):
		print("Ending game   " + name + ": " + win_lose)
	if(win_lose == "*WIN"):
		results[player_num] += 1

# Start the games
for game_num in xrange(1, num_runs + 1):
	print("Starting game " + game_name + str(game_num) + ": " + player1 + " vs. " + player2)
	t1 = threading.Thread(target = run_game, args = ("./" + player1 + " " + game_name + str(game_num), game_name + str(game_num), 0))
	t1.start()
	t2 = threading.Thread(target = run_game, args = ("./" + player2 + " " + game_name + str(game_num), game_name + str(game_num), 1))
	t2.start()
	players.append((t1,t2,game_name + str(game_num)))



# Wait for the games to end
for (t1, t2, name) in players:
	t1.join()
	t2.join()

print("Player " + player1 + " won " + str(results[0]) + " times")
print("Player " + player2 + " won " + str(results[1]) + " times")
