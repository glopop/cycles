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

  void sendMove() {
    // Cycle through all directions: north, east, south, west
    Direction directions[] = {Direction::north, Direction::east, Direction::south, Direction::west};
    connection.sendMove(directions[moveCounter % 4]);
    moveCounter++;
  }

  void receiveGameState() {
    state = connection.receiveGameState();
    std::cout << "amount: " << state.players.size() << " players" << std::endl;
  }

public:
  BotClient(const std::string &botName) : name(botName) {
    connection.connect(name);
    if (!connection.isActive()) {
      std::cerr << "failed to conect" << std::endl;
      exit(1);
    }


  void run() {
    while (connection.isActive()) {
      receiveGameState();
      sendMove();
    }
  }
};

int main() {
  BotClient bot("gloria");
  bot.run();
  return 0;
}
