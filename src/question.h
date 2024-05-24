#pragma once
#include <string>

class Question {
public:
    std::string path_to_question;
    bool answer;
    std::string path_to_explanation;

    Question(const std::string& question_path, const bool ans, const std::string& explanation_path)
        : path_to_question(question_path), answer(ans), path_to_explanation(explanation_path) {}

    void play_question() const {
        //play_audio(path_to_question);
    }

    void play_explanation() const {
        //play_audio(path_to_explanation);
    }
};