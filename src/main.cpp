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
State state = State::speaking_category_selection; // todo speaking
int topSide =  gyro_get_orientation();

std::vector<Player> players;
players.emplace_back("Player 1");
players.emplace_back("Player 2");

current_player_index = 0;

Question current_question = nullptr;

void change_current_player() {
    current_player_index++;
    if (current_player_index >= players.size()) {
        current_player_index = 0;
    }
}

void game_end() {
    if(players[0] > players[1]) {
        speaker_play_file("end/player_1.mp3")
    }
    else if(players[1] > players[0]) {
        speaker_play_file("end/player_2.mp3")
    }
    else {
        speaker_play_file("end/tie.mp3")
    }
}

void set_category_colors() {
    int neighbors [] = getNeighbors();
    for(int i = 0; i < qg.categories.size(); ++i){
        uint32_t colors[12];
        for (int i = 0; i < 12; i++) {
            colors[i] = qg.categories[i].color
        }
        led_display_side(neighbors[i], colors)
        qg.categories[i].setCurrentSide(neighbors[i])
    }
} 

void set_true_false_colors() {
    
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
    speaker_play_file(current_question.path_to_explanation)
}

void setup() {
    Serial.begin(115200);
    
    sd_setup();
    speaker_setup();

    sd_print_file("/test.txt");
    QuestionGetter qg("/categories");
    
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

int current_true_false_sides [] = {-1, -1};
 
void loop()
{
    // quiz logic
    
    // set the category side numbers
    //...
    switch(state) { // todo breaks add
        case State::waiting_for_category_selection: 
            set_category_colors();
            if(topSide != gyro_get_orientation()) {
                topSide = gyro_get_orientation(); // update topSide
                // todo call readQuestion with the current position with getNeighbors()
                select_and_play_question();
                state = State::speaking_question;
            } break;
        case State::speaking_question:
            if(!speaker_is_playing()){
                state = State::waiting_for_answer_selection;
            }
            break;
        case State::waiting_for_answer_selection:
            if(current_true_false_sides[0] = -1) {
                current_true_false_sides[0] = getTrueFalseSides()[0];
                current_true_false_sides[1] = getTrueFalseSides()[1];
            }
    
            // and conditions implies the side rotation sould be correct (left and right) 
            // index 0 and 2 are true and false for the user to select
            if(topSide != gyro_get_orientation() && (topSide == current_true_false_sides[0] || topSide == getTrueFalseSides()[1])) {
                // reset true false array
                current_true_false_sides[0] = -1;
        
                topSide = gyro_get_orientation(); // update topSide
                state = State::speaking_answer_explanation_score_turns_or_end;
            } break;
        // cube speak mode
        case State::speaking_answer_explanation_score_turns_or_end:
            state = State::speaking_category_selection;
            //...
            break;
    
        case State::speaking_category_selection:
            state = State::waiting_for_category_selection;
            //...
            break;
        default: break;
        // dummy case to add more cases/states later
    };
    
}