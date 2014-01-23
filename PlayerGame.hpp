#pragma once

#include <cstdlib>
#include <cstdio>

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

#include <sstream>
#include <iterator>

#include <cmath>
#include <cassert>
#include <cstring>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

const int STATE_SIZE  = 4;
const int STATE_DIM   = 2;
const int STATE_NUM   = std::pow(STATE_SIZE, STATE_DIM);

const int MAX_MESSAGE_LENGTH = 1024;
const char DELIMITER = ' ';

template <typename IT>
std::vector<std::vector<int> > data2wall(IT begin, IT end) {
  std::vector<std::vector<int> > data;
  for (; begin < end; begin += STATE_SIZE)
    data.push_back(std::vector<int>(begin, begin + STATE_SIZE));
  return data;
}

std::vector<std::vector<int> > vector2wall(const std::vector<int>& v) {
  return data2wall(v.begin(), v.end());
}

std::vector<int> string2vector(const std::string& s) {
  std::vector<int> data;
  std::stringstream str(s);
  std::copy(std::istream_iterator<int>(str), std::istream_iterator<int>(),
            std::back_inserter(data));
  return data;
}


class Game {
 private:
  int comm;
  char buffer[MAX_MESSAGE_LENGTH];

  std::string recv() {
    int read_char = read(comm, buffer, MAX_MESSAGE_LENGTH);
    if (read_char == -1)
      std::cout << "READ ERROR" << std::endl;
    std::string s(buffer, read_char);
    if (s == std::string("")) {
      std::cout << "END" << std::endl;
      exit(0);
    }
    return s;
  }

  void send(const std::string& s) {
    if (write(comm, s.c_str(), s.size()) == -1)
      std::cout << "WRITE ERROR" << std::endl;
  }

 public:
  typedef std::vector<std::vector<int> > wall_type;
  wall_type wall;
  wall_type owall;

  Game(const std::string& game_id) {
    comm = socket(AF_INET, SOCK_STREAM, 0);
    memset(buffer, 0, MAX_MESSAGE_LENGTH);

    hostent* server = gethostbyname("crisco.seas.harvard.edu");
    //hostent* server = gethostbyname("localhost");
    if (server == NULL)
      std::cout << "No such host!" << std::endl;;

    sockaddr_in addr;
    memset((char*)&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    memcpy((char*)&addr.sin_addr.s_addr, (char*)server->h_addr, server->h_length);
    if (connect(comm, (sockaddr*)&addr, sizeof(addr)) < 0)
      std::cout << "Connection Error" << std::endl;

    send(game_id);
    std::cout << "Waiting for game " << recv() << std::endl;

    std::string message = recv();
    std::cout << message << std::endl;
    assert(message == std::string("READY"));

    // Receive our game wall
    wall = vector2wall(string2vector(recv()));
  }

  ~Game() {
    close(comm);
  }

  int get_discard() {
    std::string msg = recv();
    if (msg == std::string("LOSE")) {
      std::cout << "***" << msg << "***" << std::endl;
      exit(0);
    }
    std::vector<int> data = string2vector(msg);
    owall = data2wall(data.begin()+1, data.end());
    return data[0];
  }

  int get_brick(const std::string& move) {
    send(move);
    return atoi(recv().c_str());
  }

  void make_move(const std::string& move) {
    send(move);
    std::string msg = recv();
    if (msg == std::string("WIN")) {
      std::cout << "***" << msg << "***" << std::endl;
      exit(0);
    }
    wall = vector2wall(string2vector(msg));
  }
};
