#include <Arduino.h>
#include "speaker.h"
#include "gyro.h"
#include "led.h"
#include "state.h"
#include "question_getter.h"
#include "sd.h"
#include <iostream>
#include "player.h"
#include "question.h"
#include "communication.h"

// current state of the cube
State state = State::speaking_player_turn; // todo speaking
int topSide =  gyro_get_orientation();

const int total_rounds = 1;
int rounds_left_to_play = total_rounds;

std::vector<Player> players;

int current_player_index = 1;

Question current_question("", false, "");
QuestionGetter qg;

int current_true_false_sides [] = {-1, -1};

uint32_t colors_red[12];
uint32_t colors_green[12];
uint32_t colors_black[12];
uint32_t colors_white[12];

bool isPlayingBoardGame = false;

int* getNeighbors();
void board_setup();
void board_loop();

void change_current_player() {
    current_player_index++;
    if (current_player_index >= players.size()) {
        current_player_index = 0;
    }
    speaker_play_file(players[current_player_index].path_to_player_turn.c_str());
}

void game_end() {
    if(players[0].points > players[1].points) {
        speaker_play_file("/end/winner_player1.mp3");
    }
    else if(players[1].points > players[0].points) {
        speaker_play_file("/end/winner_player2.mp3");
    }
    else {
        speaker_play_file("/end/winner_neutral.mp3");
    }
    players[0].resetPoints();
    players[1].resetPoints();
    qg.loadCategories(4);
}

void remove_cube_colors() {
    for(int i = 0; i < 6; ++i){
        led_display_side(i, colors_black);
    }
}

void set_category_colors() {
    int *neighbors = getNeighbors();
    for(int i = 0; i < qg.categories.size(); ++i){
        uint32_t colors[12];
        for (int a = 0; a < 12; a++) {
            colors[a] = qg.categories[i].color;
        }
        led_display_side(neighbors[i], colors);
        qg.categories[i].setCurrentSide(neighbors[i]);
    }
    delete[] neighbors;
} 

void set_true_false_colors() {
    if (current_true_false_sides[0] == -1 || current_true_false_sides[1] == -1) return;
    led_display_side(current_true_false_sides[0], colors_red);
    led_display_side(current_true_false_sides[1], colors_green);
}

void play_category() {
    for (const auto& category : qg.categories) {
        if(category.current_side == topSide) {
            std::string path = "/category/category_" + category.name + ".mp3";
            speaker_play_file(path.c_str());
        }
    }
}

void select_and_play_question() {
    for (auto& category : qg.categories) {
        if(category.current_side == topSide) {
            current_question = qg.getQuestion(category.name);
        }
        category.reset();
        speaker_play_file(current_question.path_to_question.c_str());
    }
}

void check_answer(bool answer) {
    if(answer == current_question.answer) {
        players[current_player_index].addPoint();
        speaker_play_file("/feedback/correct/001.mp3");
    }
    else {
        speaker_play_file("/feedback/incorrect/001.mp3");
    }
}

void play_game_score() {
    switch(players[current_player_index].points){
        case 0: 
            speaker_play_file("/score/0.mp3"); break;
        case 1: 
            speaker_play_file("/score/1.mp3"); break;
        case 2: 
            speaker_play_file("/score/2.mp3"); break;
        case 3: 
            speaker_play_file("/score/3.mp3"); break;
        case 4: 
            speaker_play_file("/score/4.mp3"); break;
        case 5: 
            speaker_play_file("/score/5.mp3"); break;
    }
}

void setup() {
    Serial.begin(115200);

    Player player1("Player 1", "/turn/turn_player1.mp3");
    Player player2("Player 2", "/turn/turn_player2.mp3");
    players.push_back(player1);
    players.push_back(player2);

    
    sd_setup();
    speaker_setup();
    gyro_setup();
    led_setup();
    communication_setup();

    int o;
    while (o = gyro_get_orientation() == -1)
    {
        delay(100);
    }
    topSide = o;

    qg.loadCategories(4);

    for (int a = 0; a < 12; a++) {
        colors_red[a] = 0xff0000;
        colors_green[a] = 0x00ff00;
        colors_black[a] = 0x000000;
        colors_white[a] = 0xffffff;
    }
    
    change_current_player();
}

int* getNeighbors() {
    int exclude1 = topSide;
    int exclude2 = 5 - exclude1;

    int* result = new int[4]; // neighbors of the topSide
    int index = 0;
    for (int i = 0; i < 6; ++i) {
        if (i != exclude1 && i != exclude2) {
            result[index++] = i;
        }
    }
    return result;
}

// true (index 0) and false (index 1)
void getTrueFalseSides(int *array) {
    int trueSide = getNeighbors()[0];
    int falseSide = 5 - trueSide;
    array[0] = falseSide;
    array[1] = trueSide;
}
 
void loop()
{
    speaker_loop();
    if (communication_wait_for_client())
    {
        if (!isPlayingBoardGame)
        {
            isPlayingBoardGame = true;
            board_setup();
        }
        board_loop();
        return;
    }
    if (isPlayingBoardGame)
        isPlayingBoardGame = false;
    int answer, orientation;
    switch(state) {
        case State::speaking_player_turn:
            if(!speaker_is_playing()){
                rounds_left_to_play = rounds_left_to_play - 1;
                state = State::waiting_for_category_selection;
            }
            break;
        case State::speaking_category_selection:
            if(!speaker_is_playing()){
                state = State::waiting_for_category_selection;
            }
            break;
        case State::waiting_for_category_selection: 
            set_category_colors();
            orientation = gyro_get_orientation();
            if(topSide != orientation && orientation != -1) {
                topSide = orientation; // update topSide
                play_category();
                state = State::speaking_category;
            } break;
        case State::speaking_category:
            if(!speaker_is_playing()){
                select_and_play_question();
                state = State::speaking_question;
            }
            break;
        case State::speaking_question:
            if(!speaker_is_playing()){
                remove_cube_colors();
                state = State::waiting_for_answer_selection;
            }
            break;
        case State::waiting_for_answer_selection:
            if(current_true_false_sides[0] = -1) {
                getTrueFalseSides(current_true_false_sides);
            }
            set_true_false_colors();
            // and conditions implies the side rotation sould be correct (left and right) 
            // index 0 and 2 are true and false for the user to select
            answer = gyro_get_orientation();
            if(topSide != answer && (answer == current_true_false_sides[0] || answer == current_true_false_sides[1])) {
                // reset true false array
                current_true_false_sides[0] = -1;
                // maps user selection to boolean
                check_answer(answer == current_true_false_sides[1]);
                topSide = gyro_get_orientation(); // update topSide
                state = State::speaking_feedback;
            } break;
        case State::speaking_feedback:
            if(!speaker_is_playing()){
                speaker_play_file(current_question.path_to_explanation.c_str());
                state = State::speaking_explanation;
            }
            break;
        case State::speaking_explanation:
            if(!speaker_is_playing()){
                play_game_score();
                state = State::speaking_score;
            }
            break;
        case State::speaking_score:
            if(!speaker_is_playing()){
                remove_cube_colors();
                state = State::checking_game_end;
                if(rounds_left_to_play == 0)
                {
                    game_end();
                    state = State::speaking_game_end;
                }
                else{
                    state = State::speaking_player_turn;
                }
            }
            break;
        case State::speaking_game_end:
            if(!speaker_is_playing()){
                rounds_left_to_play = total_rounds;
                state = State::speaking_player_turn;
            }
            break;
        default: break;
    };
    
}


#include "board_game.h"