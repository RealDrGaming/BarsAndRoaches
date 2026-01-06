#include <fstream>
#include <iostream>
#include <windows.h> // needed for console encoding switch
#include <cstdlib> // needed for srand and rand functions
#include <ctime> // needed for time variation

struct student
{
    double money;
    int energy;
    int psyche;
    int knowledge;
    int physical;
    int passed_exams;
};

struct action {
    const char* name;
    double cost;
    int dEnergy;
    int dPsyche;
    int dPhys;
    int dKnow;
};

const int INPUT_LINE_MAX_SIZE = 100; // make dynamic
const char BASE_FILE_NAME[] = "SaveSlots.txt";
const int SEMESTER_LENGTH = 45;
const int NUMBER_OF_EXAMS = 5;
int EXAM_DAYS[] = {8, 17, 26, 0, 45}; // fourth date has to be random

const int BASE_STAT_VALUE = 60;
const int MAX_PLAYER_ENERGY = 100;
const int MAX_PLAYER_PSYCHE = 100;
const int MAX_PLAYER_KNOWLEDGE = 100;
const int MAX_PLAYER_PHYSICAL = 100;

int my_clamp(int value, int minVal, int maxVal) {
    if (value < minVal) return minVal;
    if (value > maxVal) return maxVal;
    return value;
}

void applyAction(student& s, const action& act, bool checkEfficiency = true) {
    
    bool isSuccess = true; 

    if (checkEfficiency)
    {
        int successChance = 0;
        
        if (s.energy > 80)
        {
            successChance = 100;
        } 
        else if (s.energy > 40)
        {
            successChance = 75;
        } 
        else 
        {
            successChance = 50;
        }

        int roll = std::rand() % 100;
        
        if (roll < successChance)
        {
            isSuccess = true;
        } 
        else
        {
            isSuccess = false;
            std::cout << "Уморен си! Действието не беше напълно ефективно!\n";
        }
    }
    
    double moneyChange = act.cost;
    if (!isSuccess && moneyChange > 0) moneyChange /= 2;
    s.money += moneyChange;
    
    int energyChange = act.dEnergy;
    if (!isSuccess && energyChange > 0) energyChange /= 2;
    s.energy = my_clamp(s.energy + energyChange, 0, MAX_PLAYER_ENERGY);
    
    int psycheChange = act.dPsyche;
    if (!isSuccess && psycheChange > 0) psycheChange /= 2;
    s.psyche = my_clamp(s.psyche + psycheChange, 0, MAX_PLAYER_PSYCHE);
    
    int physChange = act.dPhys;
    if (!isSuccess && physChange > 0) physChange /= 2;
    s.physical = my_clamp(s.physical + physChange, 0, MAX_PLAYER_PHYSICAL);
    
    int knowChange = act.dKnow;
    if (!isSuccess && knowChange > 0) knowChange /= 2;
    s.knowledge = my_clamp(s.knowledge + knowChange, 0, MAX_PLAYER_KNOWLEDGE);
}

int getValidInput(int min, int max) {
    int choice;
    while (true) {
        std::cout << " > ";
        if (std::cin >> choice && choice >= min && choice <= max) {
            return choice;
        }
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        std::cout << "Невалидна команда, опитай отново!\n";
    }
}

void writeInFile(const char* fileName, const char* content)
{
    std::ofstream outputStream (fileName, std::ios_base::app);

    if (!outputStream.is_open())
    {
        std::cout << "Файлът не се записа успешно!" << std::endl;
        return;
    }

    outputStream << content << std::endl;
    
    outputStream.close();
}

bool readFile(const char* fileName)
{
    std::ifstream inputStream (fileName);

    if (!inputStream.is_open())
    {
        std::cout << "Файлът не се прочете успешно!" << std::endl;
        return false;
    }

    std::cout << inputStream.rdbuf(); // print all lines (USE ANOTHER WAY), index them so a player can choose one of them
    
    inputStream.close();
    return true;
}

bool lineExistsIn(const char* fileName, const char* line)
{
    std::ifstream inputStream (fileName);
    
    if (!inputStream.is_open())
    {
        std::cout << "Файлът не се прочете успешно!" << std::endl;
        return false;
    }
    
    // check if the line is contained in the file and return true if yes
    
    inputStream.close();
    return false;
}

void printProgressBar(int current)
{
    int totalBlocks = 10;
    int filled = (current * totalBlocks) / 100;
    
    for (int i = 0; i < filled; i++) std::cout << "▓";
    
    for (int i = filled; i < totalBlocks; i++) std::cout << "░";
}

int getNextExamDay(int currentDay, const int examDays[], int totalExams)
{
    for (int i = 0; i < totalExams; i++)
    {
        if (examDays[i] >= currentDay)
        {
            return examDays[i];
        }
    }
    return -1; // Exam was not found
}

void printHUD(int day, const student& s)
{
    int nextExamDay = getNextExamDay(day, EXAM_DAYS, NUMBER_OF_EXAMS);
    
    std::cout << "╭─────────────────────────────────────╮\n"
              << "     Ден " << day << " от " << SEMESTER_LENGTH << "\n"
              << "     Следващият изпит е на ден " << nextExamDay << "\n"
              << " ───────────────────────────────────── \n"
              << "     » Пари: " << s.money << "€\n"
              << "     » Енергия: "; printProgressBar(s.energy); std::cout << " (" << s.energy << ")\n"
              << "     » Психика: "; printProgressBar(s.psyche); std::cout << " (" << s.psyche << ")\n"
              << "     » Здраве: "; printProgressBar(s.physical); std::cout << " (" << s.physical << ")\n"
              << "     » Знание:  "; printProgressBar(s.knowledge); std::cout << " (" << s.knowledge << ")\n"
              << "     » Взети изпити:  " << s.passed_exams << "/" << NUMBER_OF_EXAMS << "\n";
    std::cout << "╰─────────────────────────────────────╯" << std::endl;
}

bool stringsMatch(const char* a, const char* b) {
    int i = 0;
    
    while (a[i] != '\0' && b[i] != '\0')
    {
        if (a[i] != b[i]) return false;
        i++;
    }
    
    return (a[i] == '\0' && b[i] == '\0');
}

void triggerSideEffect(student& s, const char* category) 
{
    if (stringsMatch(category, "Хранене")) { // Spoiled food event
        if (std::rand() % 12 == 0)
        {
            std::cout << "\n О, не! Храната ти беше развалена... (-10 Енергия)\n";
            s.energy = my_clamp(s.energy - 10, 0, MAX_PLAYER_ENERGY);
        }
    }
    else if (stringsMatch(category, "Учене")) { // Eureka event
        if (std::rand() % 20 == 0) 
        {
            std::cout << "\n Еврика! Разбра материала перфектно! (+10 Знания)\n";
            s.knowledge = my_clamp(s.knowledge + 10, 0, MAX_PLAYER_KNOWLEDGE);
        }
    }
    else if (stringsMatch(category, "Излизане")) { // Lucky event ig
        if (std::rand() % 30 == 0) 
        {
            std::cout << "\n Намери 20€ на земята пред дискотеката! (+20 Пари)\n";
            s.money += 20;
        }
    }
}

void runSubMenu(student& s, const char* title, const action actions[], int count)
{
    std::cout << "╭───────────────────────────────────────────────────╮\n"
              << "            " << title << "\n"
              << " ───────────────────────────────────────────────────\n";

    for (int i = 0; i < count; i++) {
        std::cout << " [" << (i + 1) << "] " << actions[i].name << "\n";
    }
    
    std::cout << "╰───────────────────────────────────────────────────╯\n";

    int choice = getValidInput(1, count);
    
    applyAction(s, actions[choice - 1]);
    
    triggerSideEffect(s, title);
}

void attemptExam(student& s, int examIndex) 
{
    int luckCoeff = std::rand() % 100;
    int penalty = examIndex * 5;
    
    double successChance = (s.knowledge * 0.75) + (s.psyche * 0.1) + (s.energy * 0.1) + (luckCoeff * 0.2) - penalty + 10;

    std::cout << "\n--- РЕЗУЛТАТ ОТ ИЗПИТА ---\n";
    if (successChance > 75) 
    {
        std::cout << "Изпита е взет!\n";
        s.passed_exams++;
        action reward = {"Pass", 0, -20, 20, 0, 0};
        applyAction(s, reward, false);
    } 
    else 
    {
        std::cout << "Скъсан си!\n";
        action fail = {"Fail", 0, -20, -30, 0, 0};
        applyAction(s, fail, false);
    }
}

const action DAILY_EVENTS[] = {
    {"Мама и тате ти пращат пари!", 30, 0, 0, 0, 0},
    {"Приятел те черпи кафе", 0, 0, 10, 0, 0},
    {"Разболял си се", 0, -20, 0, 0, 0},
    {"Няма ток в блока", 0, 0, 0, 0, 0}
};

void waitForKey()
{
    std::cout << "Натисни Enter за да продължиш...";
    std::cin.ignore();
    std::cin.get();
}

bool triggerDailyEvent(student& s)
{
    if (std::rand() % 30 == 0)
    {
        int eventIndex = std::rand() % 4;
        
        std::cout << "╭──────────────────────────────────────────╮\n"
                  << "│            СЛУЧАЙНО СЪБИТИЕ!             │\n"
                  << " ────────────────────────────────────────── \n"
                  << "      " << DAILY_EVENTS[eventIndex].name << "\n"
                  << "╰──────────────────────────────────────────╯" << std::endl;
        
        if (eventIndex == 3)
        {
            std::cout << " (Пропускаш действието за деня...)\n";
            waitForKey();
            
            return true;
        }
        
        applyAction(s, DAILY_EVENTS[eventIndex], false);
    }
    return false;
}

const action STUDY_ACTIONS[] = {
    {"Лекции (Знания++ / Енергия-- / Психика- / Здраве-)", 0, -20, -10, -10, 20},
    {"Вкъщи сам (Знания+++ / Енергия- / Психика--- / Здраве-)", 0, -10, -30, -10, 30},
    {"Навън с приятели (Знания+ / Енергия- / Психика+ / Здраве-)", 0, -10, 10, -10, 10}
};

const action FOOD_ACTIONS[] = {
    {"Стол (-4€ / Енергия++ / Психика-)", -4, 20, -10, 0, 0},
    {"Дюнер (-6€ / Енергия++ / Психика+ / Здраве--)", -6, 20, 10, -20, 0},
    {"Вкъщи (-6€ / Енергия++)", -6, 20, 0, 0, 0}
};

const action FUN_ACTIONS[] = {
    {"Бар (-15€ / Енергия- / Психика++ / Здраве-)", -15, -10, 20, -10, 0},
    {"Концерт (-40€ / Енергия-- / Психика+++ / Здраве--)", -40, -20, 30, -20, 0},
    {"Разходка (0€ / Енергия- / Психика+ / Здраве+)", 0, -10, 10, 10, 0}
};
            
const action REST_ACTIONS[] = {
    {"Сън (Психика+ / Енергия+++)", 0, 30, 10, 0, 0},
    {"Играй игри (Психика+ / Енергия- / Здраве-)", 0, -10, 10, -10, 0},
    {"Фитнес (Психика++ / Енергия-- / Здраве++)", 0, -20, 20, 20, 0}
};

const action WORK_ACTIONS[] = {
    {"Почасово (+30€ / Енергия- / Психика-)", 30, -10, -10, 0, 0},
    {"Касиер (+50€ / Енергия-- / Психика--)", 50, -20, -20, 0, 0},
    {"Таксиджия (+70€ / Енергия--- / Психика---)", 70, -30, -30, 0, 0}
};

int main(int argc, char* argv[])
{
    SetConsoleOutputCP(CP_UTF8); // switch to utf-8 encoding, so the console recognizes the ascii symbols
    std::srand(std::time(0)); // change the seed of the rand function
    
    int randomExamDate = 26 + (std::rand() % 20);    
    EXAM_DAYS[3] = randomExamDate;
    
    int commandLine = 0;
    
    // ! save-file load system
    std::cout << "╭──────────────────────────╮ \n"
              << "│     Bars and Roaches     │ \n"
              << "│       [1] Нов файл       │ \n"
              << "│       [2] Продължи       │ \n"
              << "╰──────────────────────────╯ \n" << std::endl; // ! need both new lines here

    do
    {
        std::cin >> commandLine;

        char saveFileName[INPUT_LINE_MAX_SIZE];
        
        if (commandLine == 1)
        {
            std::cout << "Как искаш да кръстиш новия записан файл? {Без празни места!}" << '\n';
            std::cin >> saveFileName;

            if (!lineExistsIn(BASE_FILE_NAME, saveFileName))
                writeInFile(BASE_FILE_NAME, saveFileName);
            
            break;
        }
        if (commandLine == 2)
        {
            std::cout << "От кой записан файл искаш да продължиш?" << '\n';

            if (!readFile(BASE_FILE_NAME))
                continue;
            
            // make a way to choose from printed files
            break;
        }
        
        std::cout << "Невалидна команда, опитай отново!" << '\n';
    }
    while (true);

    bool isNewGame = commandLine % 2; // true or false depending on if saveFile is new
    
    int difficultyLevel = 0;
    
    // ! difficulty choosing if new save file
    if (isNewGame)
    {
        std::cout << "╭───────────────────────────────────────────────╮ \n"
                  << "│          Избери своята специалност:           │ \n"
                  << "│       [1] Софтуерно инженерство  |  ЛЕСНО     │ \n"
                  << "│       [2] Компютърни науки  |  СРЕДНО         │ \n"
                  << "│       [3] Информатика  |  ТРУДНО              │ \n"
                  << "╰───────────────────────────────────────────────╯" << std::endl;

        do
        {
            std::cin >> difficultyLevel;
        
            if (difficultyLevel >= 1 && difficultyLevel <= 3)
                break;
        
            std::cout << "Невалидна команда, опитай отново!" << '\n';
        }
        while (true);
    }
    
    student mainCharacter;
    
    if (isNewGame)
    {
        mainCharacter.money = 50;
        mainCharacter.energy = difficultyLevel * 10 + BASE_STAT_VALUE;
        mainCharacter.psyche = difficultyLevel * 10 + BASE_STAT_VALUE;
        mainCharacter.physical = difficultyLevel * 10 + BASE_STAT_VALUE;
        mainCharacter.knowledge = difficultyLevel * 10 + BASE_STAT_VALUE;
        mainCharacter.passed_exams = 0;
    } // ! informatika with the highest starting stats, cuz easiest to get in, so the least burnout
    else
    {
        //set mainCharacter stats to ones read from the saveFile
        
        /*
        mainCharacter.money = 50;
        mainCharacter.energy = difficultyLevel * 10 + 60;
        mainCharacter.psyche = difficultyLevel * 10 + 60;
        mainCharacter.physical = difficultyLevel * 10 + 60;
        mainCharacter.knowledge = difficultyLevel * 10 + 60;
        mainCharacter.passed_exams = 0;
        */
    }
    
    for (int day = 1; day <= SEMESTER_LENGTH; day++) // day should start from day rEad from file
    {
        system("cls");
        
        printHUD(day, mainCharacter);
        
        int todayExamIndex = -1;
        for (int i = 0; i < NUMBER_OF_EXAMS; i++) {
            if (EXAM_DAYS[i] == day) todayExamIndex = i;
        }

        if (todayExamIndex != -1) {
            std::cout << "[1] Яви се на изпит \n";
            getValidInput(1, 1);
            attemptExam(mainCharacter, todayExamIndex);
        } 
        else {
            if (mainCharacter.energy > 0)
            {
                bool skipDay = triggerDailyEvent(mainCharacter);
                if (skipDay) 
                {
                    continue; 
                }
                
                std::cout << "Какво искаш да направиш днес? {Ден: " << day << "}" << '\n'
                      << "[1] Учиш \n"
                      << "[2] Храниш се \n"
                      << "[3] Излизаш \n"
                      << "[4] Почивка \n"
                      << "[5] Работиш \n"
                      << "[6] Излез от играта" << std::endl;
            
                int mainChoice = getValidInput(1, 6);

                if (mainChoice == 1) runSubMenu(mainCharacter, "Учене", STUDY_ACTIONS, 3);
                else if (mainChoice == 2) runSubMenu(mainCharacter, "Хранене", FOOD_ACTIONS, 3);
                else if (mainChoice == 3) runSubMenu(mainCharacter, "Излизане", FUN_ACTIONS, 3);
                else if (mainChoice == 4) runSubMenu(mainCharacter, "Почивка", REST_ACTIONS, 3);
                else if (mainChoice == 5) runSubMenu(mainCharacter, "Работа", WORK_ACTIONS, 3);
                else break;
            }
            else
            {
                std::cout << "\n--- Претовари се! ---\n";
                std::cout << "Придна от умора, пропускаш деня и енергията ти се възстановява частично";
                
                int energyBoost = std::rand() % 40;
                
                action faint = {"Faint", 0, energyBoost, -10, 0, 0};
                applyAction(mainCharacter, faint, false);
            }
        }
        
        if (mainCharacter.money <= 0)
        {
            std::cout << "╭────────────────────────────────────────╮ \n"
                      << "│                Загуба!                 │ \n"
                      << "│           Свършиха ти парите           │ \n"
                      << "╰────────────────────────────────────────╯ \n" << std::endl;
            break;
        }
        if (mainCharacter.psyche <= 0)
        {
            std::cout << "╭────────────────────────────────────────╮ \n"
                      << "│                Загуба!                 │ \n"
                      << "│        Психиката ти не издържа         │ \n"
                      << "╰────────────────────────────────────────╯ \n" << std::endl;
            break;
        }
    }
    
    if (mainCharacter.passed_exams == NUMBER_OF_EXAMS)
    {
        std::cout << "╭────────────────────────────────────────╮ \n"
                  << "│              Поздравления!             │ \n"
                  << "│       Печелиш, взе всички изпити       │ \n"
                  << "╰────────────────────────────────────────╯ \n" << std::endl;
    }
    else
    {
        std::cout << "╭────────────────────────────────────────╮ \n"
                  << "│                Загуба!                 │ \n"
                  << "│   Невзети изпити по време на сесията   │ \n"
                  << "╰────────────────────────────────────────╯ \n" << std::endl;
    }
}