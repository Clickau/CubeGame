#include <string>

class Player {
public:
    std::string name;
    int points = 0;

    Player(const std::string& player_name) : name(player_name)) {}

    void addPoint() {
        points++;
    }
};