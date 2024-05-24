#include <Arduino.h>


#include "speaker.h"
#include "sd.h"
#include "question_getter.h"
 

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
 
void loop()
{
}