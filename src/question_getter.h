#pragma once
#include "category.h"
#include <vector>
#include <string>
#include <algorithm>
#include <random>

#include "sd.h"

unsigned int colors[] = {0x0000FF, 0x00FF00, 0xFFFF00, 0xFF0000, 0xFFFFFF};

class QuestionGetter {
public:
    std::vector<Category> categories;

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

    void loadCategories(int numberOfCategories) {
        const char *base_path = "/questions";
        categories.clear();
        File base_dir = sd_open_file(base_path);
        int counter = 0;
        while (File category_dir = base_dir.openNextFile()) {
            if (category_dir.isDirectory()) {
                Category category(category_dir.name(), colors[counter]);
                ++counter;
                while (File question_file = category_dir.openNextFile()) {
                    if (question_file.isDirectory()) {
                        const char *question_number = question_file.name();
                        std::string question_path = std::string(question_file.path()) + "/" + question_number + "_que.mp3";
                        std::string answer_path = std::string(question_file.path()) + "/" + question_number + "_ans.txt";
                        std::string explanation_path = std::string(question_file.path()) + "/" + question_number + "_exp.mp3";

                        bool answer = readAnswer(answer_path);
                        Question question(question_path, answer, explanation_path);
                        category.addQuestion(question);
                    }
                }

                categories.push_back(category);
                if (counter >= numberOfCategories) return;
            }
        }
    }

private:
    bool readAnswer(const std::string& path) {
        File file = sd_open_file(path.c_str());
        if (!file) return false;
        String s = file.readStringUntil('\n');
        return s == "true" || s == "1";
    }
};