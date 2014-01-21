#!/usr/bin/env python
"""
A simple game client
"""
import socket
import numpy as np

STATE_SIZE  = 4
STATE_DIM   = 2
STATE_SHAPE = (STATE_SIZE,) * STATE_DIM
STATE_NUM   = STATE_SIZE**STATE_DIM

MAX_MESSAGE_LENGTH = 1024
DELIMITER = ' '

# Convert a list of ints to a player wall
def list2wall(s):
  return np.array(s).reshape(STATE_SHAPE)

# Convert a string to a list of ints
def string2list(s):
  return [int(i) for i in s.split(DELIMITER)]

class Game:
  def __init__(self, game_id):
    self.comm = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    self.comm.connect(('crisco.seas.harvard.edu',8080))
    #self.comm.connect(('localhost',8080))

    self.__write(game_id)
    print "Waiting for game", self.__read()
    data = self.__read()
    print data
    assert data == "READY"

    # Our game wall
    self.wall = list2wall(string2list(self.__read()))
    # Opponent game wall -- updated when we get the discard data
    self.owall = []

  # Close the connection on deletion
  def __del__(self):
    self.comm.close()

  # Wait for the server to respond with a message and read it
  def __read(self):
    message = self.comm.recv(MAX_MESSAGE_LENGTH).strip()
    #print "Recieving: ", message
    if not message:
      print "END"
      exit()
    return message

  # Send a message to the server
  def __write(self, message):
    #print "Sending: ", message
    self.comm.send(message)


  def get_discard(self):
    msg = self.__read()
    if msg == "LOSE":
      print "***" + msg + "***"
      exit()
    data = string2list(msg)           # The discarded brick + wall info
    self.owall = list2wall(data[1:])  # The opponent wall
    return data[0]

  def get_brick(self, move):
    self.__write(str(move))
    return int(self.__read())

  def make_move(self, move):
    self.__write(str(move))
    msg = self.__read()
    if msg == "WIN":
      print "***" + msg + "***"
      exit()
    self.wall = list2wall(string2list(msg))
