import math
import random
import numpy
import collections
from time import sleep

MIN_INT = 0
MAX_INT = 100

STATE_SIZE  = 4
STATE_DIM   = 2      # 1 or 2
STATE_SHAPE = (STATE_SIZE,) * STATE_DIM
STATE_NUM   = STATE_SIZE**STATE_DIM

MAX_MESSAGE_LENGTH = 1024
DELIMITER = ' '

# Join the elements of a wall into a string
def wall2string(a):
  return DELIMITER.join(map(str,a.flatten()))

# Check if a list/array is sorted
def is_sorted(a):
  return (a[:-1] >= a[1:]).all()

# Check is the wall is a winning wall
def is_win(wall):
  if STATE_DIM == 1:
    return is_sorted(wall)
  elif STATE_DIM == 2:
    return is_sorted(wall) and is_sorted(wall.transpose())

# Transform string to a tuple representing the wall subscript
def get_idx(move):
  if STATE_DIM == 1 and len(move) >= 1:
    return (ord(c[0]) - ord('0'),)
  elif STATE_DIM == 2 and len(move) >= 2:
    c = list(move.lower())
    return (ord(c[0]) - ord('a'), ord(c[1]) - ord('0'))
  else:
    return (-1,)


class GameEngine:
  def __init__(self):
    self.player0 = None
    self.player1 = None
    self.brick_pile = range(MIN_INT, MAX_INT)
    while True:
      random.shuffle(self.brick_pile)
      self.p0wall = numpy.array(self.brick_pile[0:STATE_NUM]).reshape(STATE_SHAPE)
      self.p1wall = numpy.array(self.brick_pile[STATE_NUM:2*STATE_NUM]).reshape(STATE_SHAPE)
      if not is_win(self.p0wall) and not is_win(self.p1wall):
        break
    # The pile comes from the front (left), the discards go on the back (right)
    self.brick_pile = collections.deque(self.brick_pile[2*STATE_NUM:])
    self.turn = random.randint(0,1)
    self.current_brick = -1

  def add_player(self, player):
    if self.player0 == None:
      self.player0 = player
      return True
    elif self.player1 == None:
      self.player1 = player
      return True
    else:
      return False

  def is_ready(self):
    return self.player0 != None and self.player1 != None

  def players(self):
    return [self.player0, self.player1]

  def start(self):
    # Send each of the players the game wall
    self.player0.write(wall2string(self.p0wall))
    self.player1.write(wall2string(self.p1wall))

    # Kludge for concated streams problem
    sleep(0.4)

    # Send the starting player the discard value
    if self.turn == 0:
      self.player0.write(str(self.brick_pile[-1]) + DELIMITER + wall2string(self.p1wall))
    else:
      self.player1.write(str(self.brick_pile[-1]) + DELIMITER + wall2string(self.p0wall))


  # Helper function for making a certain players' move
  def player_move(self, player, wall, move, opponent):
    if self.current_brick == -1:
      # Set the chosen brick and sent it to player
      if move.lower() == "p":       # Choose random pile brick
        self.current_brick = self.brick_pile.popleft()
      else:                         # Choose known discard brick
        self.current_brick = self.brick_pile.pop()

      # Send the brick to the player
      player.write(str(self.current_brick))
    else:
      # Update the player's wall with the move, if valid
      idx = get_idx(move)
      if all(0 <= i < STATE_SIZE for i in idx):
        # Discard and update wall
        self.brick_pile.append(wall[idx])
        wall[idx] = self.current_brick
        if is_win(wall):
          player.write("WIN")
          opponent.write("LOSE")
          return player.handle_close()
      else:
        # Current card goes in discard
        self.brick_pile.append(self.current_brick)

      # Give the player his wall
      player.write(wall2string(wall))
      # Tell the opponent what happened this move
      opponent.write(str(self.brick_pile[-1]) + DELIMITER + wall2string(wall))

      # Update the turn
      self.turn = (self.turn+1) % 2
      self.current_brick = -1


  def make_move(self, player, move):
    if self.turn == 0 and player == self.player0:
      self.player_move(self.player0, self.p0wall, move, self.player1)
    if self.turn == 1 and player == self.player1:
      self.player_move(self.player1, self.p1wall, move, self.player0)
