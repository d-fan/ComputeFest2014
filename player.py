#!/usr/bin/env python
import sys

from PlayerGame import Game

def print_wall(s):
  """Pretty print a wall."""
  print "  || %s ||" % (' | '.join('%02s' % i for i in range(0, s.shape[1])))
  print "====" + ("=====" * s.shape[1]) + "="
  for i in xrange(s.shape[0]-1, 0, -1):
    print '%s || %s ||' % (chr(ord('A')+i), ' | '.join('%02s' % i for i in s[i]))
    print "--||" + ("----|" * s.shape[1]) + "|"
  print '%s || %s ||' % ('A', ' | '.join('%02s' % i for i in s[0]))
  print "====" + ("=====" * s.shape[1]) + "="

def choose_discard_or_pile(wall, owall, discard_brick):
  """Choose the known brick in the discard or a unknown brick in the pile.
  Input:
    wall:  A 2D numpy array denoting our game wall.
    owall: A 2D numpy array denoting the opponent's game wall.
    discard_brick: The integer value of the known brick in the discard.
  Output:
    'd': Accept the known brick in the discard.
    'p': Reject the known brick in the discard and draw from the pile.
  """
  print "\nOpponent:"
  print_wall(owall)
  print "\nMy Wall: "
  print_wall(wall)
  print "\np: **   d: %2d" % discard_brick
  return raw_input("Pile or Discard: ")

def rowcol2coord(row, col):
  """Convert numeric (row,col) to alphanumeric 'XY'"""
  return chr(ord('A') + row) + chr(ord('0') + col)

def choose_coord(wall, owall, brick):
  """Choose the coordinate to place the chosen brick into our wall.
  Input:
    wall:  A 2D numpy array denoting our wall.
    owall: A 2D numpy array denoting the opponent's wall.
    brick:  The integer value of the chosen brick.
  Output:
    'XY' where X is a char from 'A' to 'D' and Y is an char from '0' to '3'
  Consider using rowcol2coord to produce valid output.
  """
  print brick
  return raw_input("Coord: ")


if __name__ == '__main__':
  if len(sys.argv) != 2:
    print "Usage: python", sys.argv[0], "GAMEID"
    print "\tGAMEID = 0    creates a new game"
    print "\tGAMEID = WXYZ connect to a specific game"
    exit()

  # Connect to a Game with gameid from the command line
  game = Game(sys.argv[1])

  while True:
    # On our turn, we get the brick on the pile
    pile = game.get_discard()

    # Choose 'p' to get the pile brick or 'd' to get a random brick
    pd_move = choose_discard_or_pile(game.wall, game.owall, pile)

    # Get the brick we chose (either the pile brick or a random brick)
    brick = game.get_brick(pd_move)

    # Determine where to place this brick with row-col coords "A0", "B3", etc
    co_move = choose_coord(game.wall, game.owall, brick)

    # Make the move -- informs the opponent and updates the wall
    game.make_move(co_move)
