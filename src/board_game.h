#pragma once
#include "game_functions.h"
#include "state.h"
#include <algorithm>

const int total_points = 69;
const int points_lost_for_category_change = 2;
const int points_lost_for_false_answer = 5;
QuestionGetter qg1;

class Board
{
public:
    Board() : category(2), position(-1) {}

    // true if we reached end
    bool move_forward()
    {
        position++;
        if (position >= 5)
        {
            return true;
        }
        return false;
    }

    void move_left()
    {
        category--;
        if (category < 0) category = 0;
    }

    void move_right()
    {
        category++;
        if (category > 4) category = 4;
    }

    int getCategory() {return category;}
    int getPosition() {return position;}
private:
    int category;
    int position;
} board;

BoardState boardState = BoardState::speaking_category;
int boardTopSide = -1;
int left_side = -1, front_side = -1, right_side = -1;
int random_categories[5] = {0, 1, 2, 3, 4};
int current_category = -1;
Question question("", false, "");
int points = total_points;
bool correct;

// top, front
int array_right_side[6][6] = {
{-1, 2, 4, 1, 3, -1},
{3, -1, 0, 5, -1, 2},
{1, 5, -1, -1, 0, 4},
{4, 0, -1, -1, 5, 1},
{2, -1, 5, 0, -1, 3},
{-1, 3, 1, 4, 2 -1}
};


void calculateSides()
{
    int exclude1 = boardTopSide;
    int exclude2 = 5 - exclude1;

    int result[4];
    int index = 0;
    for (int i = 0; i < 6; ++i) {
        if (i != exclude1 && i != exclude2) {
            result[index++] = i;
        }
    }

    front_side = result[0];
    right_side = array_right_side[boardTopSide][front_side];

    left_side = 5 - right_side;
}

void randomize()
{
    std::random_shuffle(random_categories, random_categories + 5);
}

void board_setup()
{
    qg1.loadCategories(5);
    int o;
    while (o = gyro_get_orientation() == -1)
    {
        delay(100);
    }
    boardTopSide = o;

    calculateSides();
    randomize();
}

void speak_category()
{
    auto category = qg1.categories[current_category];
    std::string path = "/category/category_" + category.name + ".mp3";
    speaker_play_file(path.c_str());
}

void speak_question()
{
    question = qg1.getQuestion(qg1.categories[current_category].name);
    speaker_play_file(question.path_to_question.c_str());
}

bool board_check_answer(bool answer) {
    if(answer == question.answer) {
        speaker_play_file("/feedback/correct/001.mp3");
    }
    else {
        speaker_play_file("/feedback/incorrect/001.mp3");
        points--;
    }
    return answer == question.answer;
}

void speak_game_lost()
{
    // todo
}

void speak_game_won()
{
    // todo
}

void speak_points()
{
    // todo
}

void speak_explanation()
{
    speaker_play_file(question.path_to_explanation.c_str());
}

// returns false if we still have points
bool check_game_over()
{
    return points <= 0;
}

void board_loop()
{
    int orientation;
    bool answer;
    switch (boardState)
    {
    case BoardState::waiting_for_category_selection:
        orientation = gyro_get_orientation();
        if (orientation != boardTopSide && orientation != -1)
        {
            // we go left
            if (orientation == right_side)
            {
                board.move_left();
                // new to old
                left_side = boardTopSide;
                right_side = 5 - boardTopSide;
                boardTopSide = orientation;
                current_category = random_categories[board.getCategory()];
                points--;
                if (check_game_over())
                {
                    speak_game_lost();
                    boardState = BoardState::speaking_game_end;
                    return;
                }
                speak_category();
                boardState = BoardState::speaking_category;
            }
            else if (orientation == left_side) // we go right
            {
                board.move_right();
                // new to old
                right_side = boardTopSide;
                left_side = 5 - boardTopSide;
                boardTopSide = orientation;
                current_category = random_categories[board.getCategory()];
                points--;
                if (check_game_over())
                {
                    speak_game_lost();
                    boardState = BoardState::speaking_game_end;
                    return;
                }
                speak_category();
                boardState = BoardState::speaking_category;
            }
            else if (orientation == 5 - front_side) // we go forward
            {
                bool finished = board.move_forward();
                front_side = boardTopSide;
                boardTopSide = orientation;
                remove_cube_colors();
                if (finished)
                {
                    speak_game_won();
                    boardState = BoardState::speaking_game_end;
                    return;
                }
                speak_question();
                boardState = BoardState::speaking_question;
            }
            else
            {
                // turned back
            }
        }
        break;
    case BoardState::speaking_category:
        if(!speaker_is_playing()){
            boardState = BoardState::waiting_for_category_selection;
        }
        break;
    case BoardState::speaking_question:
        if (!speaker_is_playing()) {
            led_display_side(left_side, colors_red);
            led_display_side(right_side, colors_green);
            boardState = BoardState::waiting_for_answer;
        }
        break;
    case BoardState::waiting_for_answer:
        orientation = gyro_get_orientation();
        if (orientation != -1 && orientation != boardTopSide && (orientation == left_side || orientation == right_side))
        {
            // turned right
            if (orientation == left_side)
            {
                right_side = boardTopSide;
                left_side = 5 - boardTopSide;
                answer = false;
            }
            else if (orientation == right_side)
            {
                left_side = boardTopSide;
                right_side = 5 - boardTopSide;
                answer = true;
            }
            boardTopSide = orientation;
            correct = board_check_answer(answer);
            boardState = BoardState::speaking_feedback;
        }
        break;
    case BoardState::speaking_feedback:
        if (!speaker_is_playing())
        {
            if (correct)
            {
                speak_explanation();
                boardState = BoardState::speaking_explanation;
            }
            else
            {
                speak_points();
                boardState = BoardState::speaking_points_left;
            }
        }
        break;
    case BoardState::speaking_explanation:
        if (!speaker_is_playing())
        {
            if (correct)
            {
                // answered correctly, can change category
                randomize();
                led_display_side(front_side, colors_white);
                speak_category();
                boardState = BoardState::speaking_category;
            }
            else
            {
                // answered incorrectly, has to answer another question of the same category
                speak_question();
                boardState = BoardState::speaking_question;
            }
        }
        break;
    case BoardState::speaking_points_left:
        if (!speaker_is_playing())
        {
            speak_explanation();
            boardState = BoardState::speaking_explanation;
        }
        break;
    case BoardState::speaking_game_end:
        break;
    default:
        break;
    }   
}