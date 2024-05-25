#pragma once
 enum class State {
    waiting_for_game_selection,
    waiting_for_category_selection,
    waiting_for_answer_selection,
    speaking_feedback,
    speaking_explanation,
    speaking_score,
    speaking_player_turn,
    speaking_category,
    speaking_category_selection,
    speaking_game_end,
    speaking_question,
    checking_game_end,
    default_dummy
};

enum class BoardState {
    waiting_for_category_selection,
    speaking_category,
    speaking_question,
    waiting_for_answer,
    speaking_feedback,
    speaking_explanation,
    speaking_points_left,
    speaking_game_end,
    default_dummy
};