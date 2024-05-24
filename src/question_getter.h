#pragma once
#include "category.h"
#include <vector>
#include <string>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <random>

namespace fs = std::filesystem;

class QuestionGetter {
public:
    std::vector<Category> categories;

    QuestionGetter(const std::string& base_path) {
        loadCategories(base_path);
    }

    Question getQuestion(const std::string& category_name) {
        auto it = std::find_if(categories.begin(), categories.end(), [&category_name](const Category& category) {
            return category.name == category_name;
        });

        if (it != categories.end() && !it->questions.empty()) {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(0, it->questions.size() - 1);
            int index = dis(gen);

            Question question = it->questions[index];
            it->questions.erase(it->questions.begin() + index);

            return question;
        } else {
            throw std::runtime_error("Category not found or no questions available");
        }
    }

private:
    bool readAnswer(const std::string& path) {
        std::ifstream file(path);
        std::string line;
        if (file.is_open() && std::getline(file, line)) {
            return (line == "true" || line == "1");
        }
        return false;
    }

    void loadCategories(const std::string& base_path) {
        for (const auto& category_entry : fs::directory_iterator(base_path)) {
            if (category_entry.is_directory()) {
                Category category(category_entry.path().filename().string());

                for (const auto& question_entry : fs::directory_iterator(category_entry.path())) {
                    if (question_entry.is_directory()) {
                        std::string question_number = question_entry.path().filename().string();
                        std::string question_path = (question_entry.path() / (question_number + "_que.wav")).string();
                        std::string answer_path = (question_entry.path() / (question_number + "_ans.txt")).string();
                        std::string explanation_path = (question_entry.path() / (question_number + "_exp.wav")).string();
                        
                        bool answer = readAnswer(answer_path);
                        Question question(question_path, answer, explanation_path);
                        category.addQuestion(question);
                    }
                }
                categories.push_back(category);
            }
        }
    }
};