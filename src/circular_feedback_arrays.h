#include <iostream>
#include <string>

class FeedbackArrays {
    std::string correctFeedback[5];
    std::string incorrectFeedback[5];

    private:
        int pointerCorrectArray = 0;
        int pointerIncorrectArray = 0;

    public:
    FeedbackArrays() {}

    // CAUTION: There can be only a maximum of 9 files for each of the correct and incorrect arrays (more is not necessary anyways)
    // each feedback file is of the form: feedback/[in]correct/00[index_digit]
    void setFeedbackArrayDirectories() {
        std::string correctString = "correct/00";
        std::string incorrectString = "incorrect/00";
        std::string feedbackString = "feedback/";
        // iterate first for setting the incorrect feedback array and second for the correct feedback array
        for(int i = 0; i < 5; i++) {
            correctFeedback[i] = feedbackString + correctString + std::to_string(i);
            incorrectFeedback[i] = feedbackString + correctString + std::to_string(i);
        }
            
    }

    std::string getNextCorrectFeedback() {
        std::string next = correctFeedback[pointerCorrectArray];
        pointerCorrectArray = (pointerCorrectArray + 1) % 5;
        return next;
    }

    std::string getNextInCorrectFeedback() {
        std::string next = incorrectFeedback[pointerIncorrectArray];
        pointerIncorrectArray = (pointerIncorrectArray + 1) % 5;
        return next;
    }
};