#pragma once
#include "question.h"
#include <vector>
#include <string>

class Category {
public:
    std::string name;
    std::vector<Question> questions;

    Category(const std::string& category_name) : name(category_name) {}
    void addQuestion(const Question& question) {
        questions.push_back(question);
    }
};
