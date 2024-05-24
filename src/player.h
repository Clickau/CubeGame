#include <string>

class Player {
public:
    std::string name;
    int points = 0;
    std::string path_to_player_turn;

    Player(const std::string& player_name, const std::string& path) : name(player_name), path_to_player_turn(path_to_player_turn) {}

    void addPoint() {
        points++;
    }
};