#pragma once
#include "question.h"
#include <vector>
#include <string>

class Category {
public:
    std::string name;
    std::vector<Question> questions;
    unsigned int color;
    int current_side = -1;

    Category(const std::string& category_name, const std::string& color) : name(category_name), color(color) {}
    void addQuestion(const Question& question) {
        questions.push_back(question);
    }

    void setCurrentSide(const int number) {
        this->current_side = number;
    }

    void reset() {
        this->current_side = -1;
    }
};
