#include <iostream>
#include <string>
#include <vector>
#include <climits>   
#include "PlayerGame.hpp"
#include <cmath>
int divisor;
#define LOGISTIC_FLATTEN 2.
#define LOGISTIC_SHIFT  12.
bool quiet = false;
const double opt_arr[4][4] = {{99.0,89.1,72.6,48.5},
			      {89.1,72.6,48.5,26.4},
			      {72.6,48.5,26.4, 9.9},
			      {48.5,26.4, 9.9, 0.0}};

Game::wall_type lwall;
bool initialized = false;
double score(const Game::wall_type& wall);
double opt_arr_addition(const Game::wall_type& wall);
int counter_score(const Game::wall_type& wall);
double* calculate_draw (const Game::wall_type& wall, int i);
double calculate_random_draw (const Game::wall_type& wall);

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

  // Check if board score can be increased more than random with the discard
  double rand = calculate_random_draw (wall);
  double max_if_discard = calculate_draw(wall,discard_brick)[2];
  std::cout << "\n max if take discarded" << max_if_discard << "\nMax random" << rand  << std::endl;
  std::cout << "\nMy Wall (score " << score(wall) << "):" << std::endl;
  if (max_if_discard >= rand)
    {
      std::cout << "Better than random" << std::endl;
      return "d";
    }
  std::cout << "Worse than random. Drawing" << std::endl;
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
  double* min_info = calculate_draw(wall, brick);
  int fst = (int) min_info[0];
  int snd = (int) min_info[1];
  Game::wall_type temp = wall;
  temp[fst][snd] = brick;
  if (score(wall) >= score(temp))
    {
      return "bad_brick";
    }
  else
    {
        std::cout << "Replacing " << wall[fst][snd] << " at " << char('A' + fst) << snd << " with " << brick << std::endl;
  return rowcol2coord(fst, snd);
    }
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
  double penalizer = 0;
  for(int i = 0;i<wall.size();i++)
    {
      for(int j=0;j<wall.size();j++)
	{
	  penalizer += max(20,abs(wall[i][j] - opt_arr[i][j]));
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

double* calculate_draw (const Game::wall_type& wall, int brick_val)
{
  Game::wall_type temp_init = wall;
  temp_init[0][0]=brick_val;
  double max_score = score(temp_init);
  
  int max_j = 0;
  int max_k = 0;
  for (int j = 0;j<wall.size();j++)
    {
      for (int k = 0; k<wall.size();k++)
	{
	  Game::wall_type temp = wall;
	  temp[j][k] = brick_val;
	  double scr = score(temp);
	  if (scr > max_score)
	    {
	      max_score = scr;
	      max_j = j;
	      max_k = k;
	    }
	}
    }
  double* max_arr = new double[3];
  max_arr[0] = max_j;
  max_arr[1] = max_k;
  max_arr[2] = max_score;
  return max_arr;
}

double calculate_random_draw (const Game::wall_type& wall)
{
  double total_score = 0.;
  for (int i = 0; i<99;i++)
    {
      total_score += calculate_draw (wall, i)[2];
    }
  return total_score/100.;
}
