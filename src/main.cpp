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

// current state of the cube
State state = State::speaking_player_turn; // todo speaking
int topSide =  gyro_get_orientation();

const int total_rounds = 6;
rounds_left_to_play = total_rounds;

std::vector<Player> players;
players.emplace_back("Player 1", "turn.player_1.mp3");
players.emplace_back("Player 2", "turn.player_2.mp3");

current_player_index = 1;

Question current_question = nullptr;

int current_true_false_sides [] = {-1, -1};

uint32_t colors_red[12];
uint32_t colors_green[12];
uint32_t colors_black[12];

void change_current_player() {
    current_player_index++;
    if (current_player_index >= players.size()) {
        current_player_index = 0;
    }
    speaker_play_file(players[current_player_index].path_to_player_turn)
}

void game_end() {
    if(players[0].points > players[1].points) {
        speaker_play_file("end/player_1.mp3")
    }
    else if(players[1].points > players[0].points) {
        speaker_play_file("end/player_2.mp3")
    }
    else {
        speaker_play_file("end/tie.mp3")
    }
}

void remove_cube_colors() {
    for(int i = 0; i < 6; ++i){
        led_display_side(i, colors_black);
    }
}

void set_category_colors() {
    int neighbors [] = getNeighbors();
    for(int i = 0; i < qg.categories.size(); ++i){
        uint32_t colors[12];
        for (int a = 0; a < 12; a++) {
            colors[a] = qg.categories[i].color
        }
        led_display_side(neighbors[i], colors)
        qg.categories[i].setCurrentSide(neighbors[i])
    }
} 

void set_true_false_colors() {
    led_display_side(current_true_false_sides[0], colors_red);
    led_display_side(current_true_false_sides[1], colors_green);
}

void select_and_play_question() {
    for (const auto& category : qg.categories) {
        if(category.current_side == topSide) {
            current_question = qg.getQuestion(category.name);
        }
        category.reset();
        speaker_play_file(current_question.path_to_question);
    }
}

void check_answer(bool answer) {
    if(answer == current_question.answer) {
        players[current_player_index].addPoint();
        speaker_play_file("feedback/correct/001.mp3")
    }
    else {
        speaker_play_file("feedback/incorrect/001.mp3")
    }
}

void setup() {
    Serial.begin(115200);
    
    sd_setup();
    speaker_setup();

    sd_print_file("/test.txt");
    QuestionGetter qg("/categories");

    for (int a = 0; a < 12; a++) {
        colors_red[a] = 0xff0000;
        colors_green[a] = 0x00ff00;
        colors_black[a] = 0x000000;
    }
    
    for (const auto &c : qg.categories)
    {
        Serial.println(c.name.c_str());
        for (const auto &q : c.questions)
        {
            Serial.println(q.path_to_question.c_str());
            Serial.println(q.path_to_explanation.c_str());
            Serial.println(q.answer);
        }
        Serial.println();
    }

    change_current_player()
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
int* getTrueFalseSides() {
    int trueSide = getNeighbors()[0];
    int falseSide = 5 - trueSide;
    int trueFalseSides[] = {falseSide, trueSide};
    return trueFalseSides;
}
 
void loop()
{
    // quiz logic
    
    // set the category side numbers
    //...
    switch(state) { // todo breaks add
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
            if(topSide != gyro_get_orientation()) {
                topSide = gyro_get_orientation(); // update topSide
                select_and_play_question();
                state = State::speaking_question;
            } break;
        case State::speaking_question:
            if(!speaker_is_playing()){
                remove_cube_colors();
                state = State::waiting_for_answer_selection;
            }
            break;
        case State::waiting_for_answer_selection:
            if(current_true_false_sides[0] = -1) {
                current_true_false_sides[0] = getTrueFalseSides()[0];
                current_true_false_sides[1] = getTrueFalseSides()[1];
            }
            set_true_false_colors()
            // and conditions implies the side rotation sould be correct (left and right) 
            // index 0 and 2 are true and false for the user to select
            int answer = gyro_get_orientation();
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
                speaker_play_file(current_question.path_to_explanation)
                state = State::speaking_score;
            }
            break;
        case State::speaking_score:
            if(!speaker_is_playing()){
                speaker_play_file(current_question.path_to_explanation)
                state = State::speaking_score;
            }
            remove_cube_colors();
            //play audio for playerturn

            //TODO: if/else check game end
            if(rounds_left_to_play == 0)
            {
                game_end();
                state = State::speaking_game_end;
            }
            else{
                state = State::speaking_player_turn;
            }
            break;
        case State::speaking_game_end:
            if(!speaker_is_playing()){
                rounds_left_to_play = total_rounds;
                state = State::speaking_player_turn;
            }
            break;
        default: break;
        // dummy case to add more cases/states later
    };
    
}