#include "api.h"
#include "utils.h"
#include <string>
#include <iostream>

using namespace cycles;

class BotClient {
  Connection connection;
  std::string name;
  GameState state;
  int moveCounter = 0;

  // checks if a move is safe by checing boundaries and obstacles
  bool isMoveSafe(Direction direction) {
    Position currentPosition = state.me.position; // get current position of the bot
    Position nextPosition = getNextPosition(currentPosition, direction);

    // if move stays within grid bounds
    if (nextPosition.x < 0 || nextPosition.y < 0 || 
        nextPosition.x >= state.grid.width || nextPosition.y >= state.grid.height) {
      return false; // out of bounds
    }

    // if target cell is empty (not a wall)
    if (state.grid.cells[nextPosition.x][nextPosition.y] != Cell::EMPTY) {
      return false;
    }

    return true; // safe
  }

  // calculates the next position based on direction
  Position getNextPosition(const Position &currentPosition, Direction direction) {
    Position nextPosition = currentPosition;
    switch (direction) {
      case Direction::north: nextPosition.y -= 1; break; // move up
      case Direction::east:  nextPosition.x += 1; break; // move right
      case Direction::south: nextPosition.y += 1; break; // move down
      case Direction::west:  nextPosition.x -= 1; break; // move left
    }
    return nextPosition;
  }

  void sendMove() {
    // try all directions to find a safe move
    Direction directions[] = {Direction::north, Direction::east, Direction::south, Direction::west};
    for (int i = 0; i < 4; i++) {
      Direction nextDirection = directions[(moveCounter + i) % 4]; // rotate directions
      if (isMoveSafe(nextDirection)) {
        connection.sendMove(nextDirection); // send first safe move
        moveCounter = (moveCounter + i + 1) % 4; // update direction sequence
        return;
      }
    }

    // if no safe moves are found, move in the current direction
    connection.sendMove(directions[moveCounter % 4]);
    moveCounter++;
  }

  void receiveGameState() {
    state = connection.receiveGameState(); // update game state
    std::cout << "amount: " << state.players.size() << " players" << std::endl;
  }

public:
  // initializes bot with name and ensures successful connection
  BotClient(const std::string &botName) : name(botName) {
    connection.connect(name); // establish connection to the server
    if (!connection.isActive()) {
      std::cerr << "failed" << std::endl;
      exit(1); // 
    }
  }

  // main game loop that handles state and moves
  void run() {
    while (connection.isActive()) { // keep running while connection is active
      receiveGameState(); // update state from server
      sendMove(); // send the next move
    }
  }
};

int main() {
  BotClient bot("gloria"); // create bot named gloria
  bot.run(); // start the game
  return 0;
}
