#include <iostream>
#include <string>
#include <vector>

#include <climits>   

#include "PlayerGame.hpp"

Game::wall_type lwall;
bool initialized = false;
int score(const Game::wall_type& wall);

/** Pretty print a game wall */
void print_wall(const Game::wall_type& s, bool highlight = false)
{
  printf("  ||");
  for (unsigned i = 0; i < s[0].size(); ++i) printf(" %2d |", i);
  printf("|\n%s\n", std::string(4+5*s[0].size()+1,'=').c_str());
  for (unsigned i = s.size()-1; i > 0; --i) {
    printf("%c ||", 'A' + i);
    int prev = -1;
    for (unsigned j = 0; j < s[i].size(); ++j) {
      if(highlight && s[i][j] != lwall[i][j]) {
        printf(" \033[1;31m%2d\033[0m |", s[i][j]);
        prev = lwall[i][j];
      } else {
        printf(" %2d |", s[i][j]);
      }
    }
    if (prev == -1)
      printf("|\n");
    else
      printf("| %d\n", prev);
    printf("--||");
    for (unsigned j = 0; j < s[i].size(); ++j) printf("----|");
    printf("|\n");
  }
  printf("%c ||", 'A');
  for (unsigned j = 0; j < s[0].size(); ++j) printf(" %2d |", s[0][j]);
  printf("|\n%s\n", std::string(4+5*s[0].size()+1,'=').c_str());
}

/** Choose the known brick in the discard or a unknown brick in the pile.
 *   Input:
 *     wall:  A 2D array denoting our game wall.
 *     owall: A 2D array denoting the opponent's game wall.
 *     discard_brick: The integer value of the known brick in the discard.
 *   Output:
 *     'd': Accept the known brick in the discard.
 *     'p': Reject the known brick in the discard and draw from the pile.
 */
std::string choose_discard_or_pile(const Game::wall_type& wall,
                                const Game::wall_type& owall,
                                int discard_brick)
{
  std::cout << "\nOpponent (score " << score(owall) << "):" << std::endl;
  print_wall(owall, initialized);
  std::cout << "\nMy Wall (score " << score(wall) << "):" << std::endl;
  print_wall(wall);
  lwall = owall;
  initialized = true;

  // Check if board score can be increased with discard_brick
  for (int i = 0; i < wall.size(); i++) {
    for (int j = 0; j < wall.size(); j++) {
      Game::wall_type temp = wall;
      temp[i][j] = discard_brick;
      if(score(temp) > score(wall)) {
        std::cout << "Found improvement" << std::endl;
        return "d";
      }
    }
  }
  std::cout << "Found no improvement. Drawing" << std::endl;
  return "p";
}

/** Convert numeric (row, col) to alphanumeric "XY" */
std::string rowcol2coord(int row, int col)
{
  return std::string(1, 'A' + row) + std::string(1, '0' + col);
}

/** Choose the coordinate to place the chosen brick into our wall.
 * Input:
 *   wall:  A 2D array denoting our wall.
 *   owall: A 2D array denoting the opponent's wall.
 *   brick: The integer value of the chosen brick.
 * Output:
 *   'XY' where X is a char from 'A' to 'D' and Y is an char from '0' to '3'
 *   Consider using rowcol2coord to produce valid output.
 */
std::string choose_coord(const Game::wall_type& wall,
                         const Game::wall_type& owall,
                         int brick)
{
  int best[3] = {-1,-1,INT_MIN};
  for(int i = 0; i < wall.size(); i++) {
    for(int j = 0; j < wall.size(); j++) {
      Game::wall_type temp = wall;
      temp[i][j] = brick;
      int temp_score = score(temp);
      if (temp_score > best[3]) {
        best[0] = i;
        best[1] = j;
        best[2] = temp_score;
      }
    }
  }
  if (best[0] < 0 || best[1] < 0) return "-1";
  std::cout << "Replacing " << wall[best[0]][best[1]] << " at " << best[0] << best[1] << " with " << brick << std::endl;
  return rowcol2coord(best[0], best[1]);
}


int main(int argc, char** argv)
{
  if (argc != 2) {
    std::cout << "Usage: " << argv[0] << " GAMEID" << std::endl;
    std::cout << "\tGAMEID = 0     creates a new game" << std::endl;
    std::cout << "\tGAMEID = WXYZ  connect to a specific game" << std::endl;
    exit(0);
  }

  // Connect to a FoosGame with id from the command line
  Game game(argv[1]);

  while (true) {
    // On our turn, we get the brick on the pile
    int pile = game.get_discard();

    // Choose 'p' to get the pile brick or 'd' to get a random brick
    std::string pd_move = choose_discard_or_pile(game.wall, game.owall, pile);

    // Get the brick we chose (either the pile brick or a random brick)
    int brick = game.get_brick(pd_move);

    // Determine where to place this brick with row-col coords "A0", "B3", etc
    std::string co_move = choose_coord(game.wall, game.owall, brick);

    // Make the move -- informs the opponent and updates the wall
    game.make_move(co_move);
  }

  return 0;
}

/*

Rough idea:
45 30 15 00
60
75
99

w[i+1][j] - w[i][j]

*/

int score2(const Game::wall_type& wall) {
  // Count how many elements are in order
  int ordered = 0;
  for(int i = 0; i < wall.size() - 1; i++) {
    for(int j = 0; j < wall.size() - 1; j++) {
      if (wall[i][j] > wall[i+1][j]) ordered++;
      if (wall[i][j] > wall[i][j+1]) ordered++;
    }
  }
  return ordered;
}

int score3(const Game::wall_type& wall) {
  int score = 0;
  for(int i = 0; i < wall.size(); i++) {
    for(int j = 0; j < wall.size(); j++) {
      if (j < wall.size() - 1) score += wall[i][j] > wall[i][j+1] ? wall[i][j] - wall[i][j+1] : 99;
      if (i < wall.size() - 1) score += wall[i][j] > wall[i+1][j] ? wall[i][j] - wall[i+1][j] : 99;
    }
  }
  return -score;
}






















