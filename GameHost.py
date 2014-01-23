import asyncore
import random
import collections
import socket
from time import sleep

from GameEngine import GameEngine as Game

MAX_MESSAGE_LENGTH = 1024

class RemoteClient(asyncore.dispatcher):
  """Wraps a remote client socket."""

  def __init__(self, host, socket, address, game = None, game_id = None):
    asyncore.dispatcher.__init__(self, socket)
    self.host = host
    self.address = address
    self.game = game
    self.game_id = game_id

  def handle_read(self):
    message = self.read()
    #print 'Recieved from', self.address, ": ", message
    if not message:
      # Game has been killed
      return

    if self.game == None:
      self.game_id = message    # Set the new game key
      if self.game_id == "0":   # Create a new game with random key
        self.game_id = str(random.randint(1000,10000))
      # Echo the game ID to the client
      self.write(self.game_id)

      # Get the game from the host
      self.game = self.host.get_game(self.game_id)

      # Attempt to join the game
      if not self.game.add_player(self):
        self.write("ERROR: Player could not be added to game" + self.game_id)
        self.host.remove_client(self)
        return

      # If the game is ready, notify the players
      if self.game.is_ready():
        print "Starting game", self.game_id

        # Kludge for concated streams problem
        sleep(0.4)

        for player in self.game.players():
          player.write("READY")

        # Kludge for concated streams problem
        sleep(0.4)

        self.game.start()
    else:
      # Active game, let the game engine interprit the move
      self.game.make_move(self, message)


  def handle_close(self):
    if self.game == None:
      # If this client does not have a game, just close and return
      self.host.remove_client(self)
    else:
      # Remove this client's game from the game host
      self.host.remove_game(self.game_id)

  def read(self):
    try:
      message = self.recv(MAX_MESSAGE_LENGTH).strip()
      #print "Read from", self.address, ":", message
      return message
    except:
      return ""

  def write(self, message):
    #print "Sending to", self.address, ": ", message
    if len(message) > MAX_MESSAGE_LENGTH:
      raise ValueError('Message too long')
    try:
      self.send(message)
    except:
      return


class GamesHost(asyncore.dispatcher):
  """Wraps a host socket listens for players and creates games"""

  def __init__(self, address=('', 8080)):
    asyncore.dispatcher.__init__(self)
    self.create_socket(socket.AF_INET, socket.SOCK_STREAM)
    self.bind(address)
    self.listen(5)
    self.remote_clients = []
    self.game = {}

  def handle_accept(self):
    socket, addr = self.accept()
    print 'Accepted', addr
    self.remote_clients.append(RemoteClient(self, socket, addr))

  def remove_client(self, client):
    if client != None:
      print "Closing", client.address
      client.close()
      self.remote_clients.remove(client)

  def get_game(self, game_id):
    # If this game doesn't exist, create it
    if not game_id in self.game:
      print "Creating game", game_id
      self.game[game_id] = Game()

    print "Clients/Games:", len(self.remote_clients), len(self.game)
    # Return the game
    return self.game[game_id]

  def remove_game(self, game_id):
    # Remove the game
    if game_id != None:
      print "Closing game", game_id
      # Remove all the players of the game
      for player in self.game[game_id].players():
        self.remove_client(player)
      del self.game[game_id]

    print "Clients/Games:", len(self.remote_clients), len(self.game)


if __name__ == '__main__':
  print "Creating host"
  host = GamesHost()
  print "Listening..."
  asyncore.loop()
