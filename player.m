%% SKELETON GAME FOR MATLAB

% USAGE foosgame(GAMEID)
% GAMEID = 0     creates a new game
% GAMEID = WXYZ  connect to a specific game

function [] = player(GAMEID)
    % Connect to a FoosGame with game_id from the input
    game = PlayerGame(GAMEID);

    while true
        % On our turn, we get the brick on the pile
        pile = game.get_discard();

        % Choose 'p' to get the pile brick or 'd' to get a random brick
        pd_move = choose_discard_or_pile(game.wall, game.owall, pile);

        % Get the brick we chose (either the pile brick or a random brick)
        brick = game.get_brick(pd_move);

        % Determine where to place this brick with row-col coords 'A0', 'B3', etc
        co_move = choose_coord(game.wall, game.owall, brick);

        % Make the move -- informs the opponent and updates the wall
        game.make_move(co_move);
    end
end

% Pretty print a game wall
function [] = print_wall(s)
    fprintf('  ||');
    for i = 1:size(s,2) fprintf(' %2d |', i-1); end
    fprintf(['|\n' repmat('=', 1, 4+5*size(s,1)+1) '\n']);
    for i = size(s,1):-1:2
      fprintf('%c ||', 'A' + i - 1);
      for j = 1:size(s,2) fprintf(' %2d |', s(i,j)); end
      fprintf(['|\n--||' repmat('----|', 1, size(s,2)) '|\n']);
    end
    fprintf('%c ||', 'A');
    for j = 1:size(s,2) fprintf(' %2d |', s(1,j)); end
    fprintf(['|\n' repmat('=', 1, 4+5*size(s,1)+1) '\n']);
end

% Choose the known brick in the discard or a unknown brick in the pile.
%   Input:
%     wall:  A matrix denoting our game wall.
%     owall: A matrix denoting the opponent's game wall.
%     discard_brick: The integer value of the known brick in the discard.
%   Output:
%     'd': Accept the known brick in the discard.
%     'p': Reject the known brick in the discard and draw from the pile.
function pd_move = choose_discard_or_pile(wall, owall, discard_brick)
  fprintf('\nOpponent:\n');
  print_wall(owall);
  fprintf('\nMy Wall:\n');
  print_wall(wall);
  fprintf('\np: **   d: %2d\n', discard_brick);
  pd_move = input('Pile or Discard: ', 's');
end

%  Convert numeric (row,col) to alphanumeric 'XY' -- 1 indexed!
function str = rowcol2coord(row, col)
  str = char(['A' + row - 1, '0' + col - 1]);
end

% Choose the coordinate to place the chosen brick into our wall.
%   Input:
%     wall:  A 2D array denoting our wall.
%     owall: A 2D array denoting the opponent's wall.
%     brick:  The integer value of the chosen brick.
%   Output:
%     'XY' where X is a char from 'A' to 'D' and Y is an char from '0' to '3'
%   Consider using rowcol2coord to produce valid output.
function coord = choose_coord(wall, owall, brick)
  fprintf('%2d\n', brick);
  coord = input('Coord: ', 's');
end
