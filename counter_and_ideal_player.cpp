#include <iostream>
#include <string>
#include <vector>
#include <climits>   

#include "PlayerGame.hpp"

#include <cmath>

int divisor;

#define LOGISTIC_FLATTEN 2.
#define LOGISTIC_SHIFT  12.

const double opt_arr[4][4] = {{99.0,89.1,72.6,48.5},
				                      {89.1,72.6,48.5,26.4},
				                      {72.6,48.5,26.4, 9.9},
				                      {48.5,26.4, 9.9, 0.0}};

Game::wall_type lwall;
bool initialized = false;
double score(const Game::wall_type& wall);
double opt_arr_addition(const Game::wall_type& wall);
int counter_score(const Game::wall_type& wall);
bool quiet = false;

/** Pretty print a game wall */
void print_wall(const Game::wall_type& s, bool highlight = false)
{
  for (int i = s.size()-1; i >= 0; --i) {
    int prev = -1;
    for (int j = 0; j < s.size(); ++j) {
      if(highlight && s[i][j] != lwall[i][j]) {
        printf("\033[1;31m%2d\033[0m  ", s[i][j]);
        prev = lwall[i][j];
      } else {
        printf("%2d  ", s[i][j]);
      }
    }
    if (prev == -1)
      printf("\n");
    else
      printf("  %d\n", prev);
  }
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
  if (!quiet) {
    std::cout << "\nOpponent (counter " << counter_score(owall) 
              << ", optimal " << opt_arr_addition(owall) << ", score " << score(owall) << "):" << std::endl;
    print_wall(owall, initialized);
    std::cout << "\nMy Wall (counter " << counter_score(wall) 
              << ", optimal " << opt_arr_addition(wall) << ", score " << score(wall) << "):" << std::endl;
    print_wall(wall);
    //lwall = owall;
    //initialized = true;
  }

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
  // Best coordinates found so far
  int best[2] = {-1,-1};
  // Best score found so far
  double best_score = -1000000000000;
  std::cout << "  Candidate moves" << std::endl;
  // Iterate through every cell
  for(int i = 0; i < wall.size(); i++) {
    for(int j = 0; j < wall.size(); j++) {
      // Create temporary wall with a change at (i,j)
      Game::wall_type temp = wall;
      temp[i][j] = brick;
      // Compare score of board with change to score of current board
      double temp_score = score(temp);
      std::cout << "  " << rowcol2coord(i, j) << ": " << temp_score << std::endl;
      if (temp_score > best_score) {
        best[0] = i;
        best[1] = j;
        best_score = temp_score;
      }
    }
  }
  // Make no move if there is no improvement
  if (best[0] < 0 || best[1] < 0) return "-1";
  std::cout << "Replacing " << wall[best[0]][best[1]] << " at " << char('A' + best[0]) << best[1] << " with " << brick << std::endl;


  return rowcol2coord(best[0], best[1]);
}


int main(int argc, char** argv)
{
  if (argc < 3) {
    std::cout << "Usage: " << argv[0] << " divisor GAMEID [-q]" << std::endl;
    std::cout << "\tGAMEID = 0     creates a new game" << std::endl;
    std::cout << "\tGAMEID = WXYZ  connect to a specific game" << std::endl;
    exit(0);
  }

  divisor = atoi(argv[1]);
  quiet = argc > 3 && argv[3][0] == '-' && argv[3][1] == 'q';

  // Connect to a FoosGame with id from the command line
  Game game(argv[2]);

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

int counter_score(const Game::wall_type& wall) {
  int counter = 0;
  for(int i = 0; i < wall.size(); i++) {
    for(int j = 0; j < wall.size(); j++) {
      for(int m = i; m < wall.size(); m++) {
        for(int n = j; n < wall.size(); n++) {
          if(wall[m][n] > wall[i][j]) counter++;
        }
      }
    }
  }
  return counter;
}

double opt_arr_addition(const Game::wall_type& wall)
{
  double penalizer = 0.0;
  for(int i = 0; i < wall.size();i++)
  {
    for(int j = 0; j < wall.size();j++)
	  {
	    int penalty = abs((double)wall[i][j]-opt_arr[i][j]);
      penalty = penalty > 20 ? 20 : penalty;
      penalizer += penalty;
	  }
  }
  return penalizer;
}

double score(const Game::wall_type& wall)
{
  //return -opt_arr_addition(wall);
  int counter = counter_score(wall);
  double logistic_mult = 1. / (std::exp(-counter / LOGISTIC_FLATTEN + LOGISTIC_SHIFT) + 1);
  //return -(counter + (int)(opt_arr_addition(wall)*counter*counter/divisor));
  return -(counter * (1-logistic_mult) + (opt_arr_addition(wall) * (logistic_mult) / divisor));
}












