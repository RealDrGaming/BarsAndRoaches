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

//-- helper functions
int randomWithMax(int oneInX) 
{
    return std::rand() % oneInX;
}

int my_clamp(int value, int minVal, int maxVal) {
    if (value < minVal) return minVal;
    if (value > maxVal) return maxVal;
    return value;
}

void waitForKey()
{
    std::cout << "Натисни Enter за да продължиш...";
    std::cin.ignore();
    std::cin.get();
}
//--

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

        int roll = randomWithMax(100);
        
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
    
    system("cls");
    
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
        if (randomWithMax(12) == 0)
        {
            std::cout << "\n О, не! Храната ти беше развалена... (-10 Енергия)\n";
            s.energy = my_clamp(s.energy - 10, 0, MAX_PLAYER_ENERGY);
        }
    }
    else if (stringsMatch(category, "Учене")) { // Eureka event
        if (randomWithMax(20) == 0) 
        {
            std::cout << "\n Еврика! Разбра материала перфектно! (+10 Знания)\n";
            s.knowledge = my_clamp(s.knowledge + 10, 0, MAX_PLAYER_KNOWLEDGE);
        }
    }
    else if (stringsMatch(category, "Излизане")) { // Lucky event ig
        if (randomWithMax(30) == 0) 
        {
            std::cout << "\n Намери 20€ на земята пред дискотеката! (+20 Пари)\n";
            s.money += 20;
        }
    }
}

bool runSubMenu(student& s, const char* title, const action actions[], int count)
{
    std::cout << "╭───────────────────────────────────────────────────╮\n"
              << "            " << title << "\n"
              << " ───────────────────────────────────────────────────\n";

    for (int i = 0; i < count; i++) {
        std::cout << " [" << (i + 1) << "] " << actions[i].name << "\n";
    }
    
    std::cout << " [" << (count + 1) << "] Назад \n";
    std::cout << "╰───────────────────────────────────────────────────╯\n";

    int choice = getValidInput(1, count + 1);

    if (choice == count + 1)
    {
        return false;
    }
    
    bool riskOfFailure = true;
    if (stringsMatch(title, "Почивка"))
    {
        riskOfFailure = false;
    }
    
    applyAction(s, actions[choice - 1], riskOfFailure);
    triggerSideEffect(s, title);
    
    return true;
}

void attemptExam(student& s, int examIndex) 
{
    int luckCoeff = randomWithMax(100);
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
    
    waitForKey();
}

const action DAILY_EVENTS[] = {
    {"Мама и тате ти пращат пари!", 30, 0, 0, 0, 0},
    {"Приятел те черпи кафе", 0, 0, 10, 0, 0},
    {"Разболял си се", 0, -20, 0, 0, 0},
    {"Няма ток в блока", 0, 0, 0, 0, 0}
};

bool triggerDailyEvent(student& s)
{
    if (randomWithMax(30) == 0)
    {
        int eventIndex = randomWithMax(4);
        
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
        waitForKey();
    }
    return false;
}

void registerSaveFile(const char* newFileName) 
{
    std::ifstream check(BASE_FILE_NAME);
    char tempName[INPUT_LINE_MAX_SIZE];
    
    if (check.is_open()) 
    {
        while (check >> tempName) 
        {
            if (stringsMatch(tempName, newFileName)) 
            {
                check.close();
                return; // Name exists already, we don't double up
            }
        }
        check.close();
    }
    
    std::ofstream list(BASE_FILE_NAME, std::ios::app);
    if (list.is_open()) 
    {
        list << newFileName << std::endl;
        list.close();
    }
}

void printAvailableSaves() 
{
    std::ifstream list(BASE_FILE_NAME);
    if (!list.is_open()) 
    {
        std::cout << " (Няма намерени записи)\n";
        return;
    }

    char tempName[INPUT_LINE_MAX_SIZE];
    std::cout << "--- Налични игри ---\n";
    while (list >> tempName) 
    {
        std::cout << " > " << tempName << "\n";
    }
    std::cout << "--------------------\n";
    list.close();
}

void saveGameState(const char* fileName, int day, const student& s) 
{
    std::ofstream file(fileName, std::ios::app); 
    
    if (file.is_open()) 
    {
        file << day << " " 
             << s.money << " " 
             << s.energy << " " 
             << s.psyche << " " 
             << s.physical << " "
             << s.knowledge << " " 
             << s.passed_exams << std::endl;
        
        file.close();
    }
}

bool loadGame(const char* fileName, student& s, int& dayOut) 
{
    std::ifstream file(fileName);
    if (!file.is_open()) return false;
    
    int day, energy, psyche, physical, knowledge, exams;
    double money;
    bool success = false;

    while (file >> day >> money >> energy >> psyche >> physical >> knowledge >> exams) {
        dayOut = day;
        s.money = money;
        s.energy = energy;
        s.psyche = psyche;
        s.physical = physical;;
        s.knowledge = knowledge;
        s.passed_exams = exams;
        success = true;
    }

    file.close();
    return success;
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
    
    int randomExamDate = 26 + randomWithMax(20);    
    EXAM_DAYS[3] = randomExamDate;
    
    student mainCharacter;
    int currentDay = 1;
    char saveFileName[INPUT_LINE_MAX_SIZE];
    bool isGameReady = false;
        
    do
    {
        system("cls");
        
        std::cout << "╭──────────────────────────╮ \n"
                  << "│     Bars and Roaches     │ \n"
                  << "│       [1] Нов файл       │ \n"
                  << "│       [2] Продължи       │ \n"
                  << "╰──────────────────────────╯ \n" << std::endl;
    
        int choice = getValidInput(1, 2);
        
        if (choice == 1)
        {
            std::cout << "Как искаш да кръстиш новия записан файл? {Без празни места!}" << '\n';
            std::cin >> saveFileName;

            registerSaveFile(saveFileName);
            
            std::cout << "╭───────────────────────────────────────────────╮ \n"
                      << "│          Избери своята специалност:           │ \n"
                      << "│       [1] Софтуерно инженерство  |  ЛЕСНО     │ \n"
                      << "│       [2] Компютърни науки  |  СРЕДНО         │ \n"
                      << "│       [3] Информатика  |  ТРУДНО              │ \n"
                      << "╰───────────────────────────────────────────────╯" << std::endl;
            
            int diff = getValidInput(1, 3);
            
            // informatika with the highest starting stats, cuz easiest to get in, so the least burnout
            int baseValue = diff * 10 + BASE_STAT_VALUE;
            
            mainCharacter = 
            { 
                50, baseValue, baseValue, 
                baseValue, baseValue, 0
            }; 
            
            isGameReady = true;
        }
        else if (choice == 2)
        {
            printAvailableSaves();
            
            std::cout << "Въведи името на файла точно: ";
            std::cin >> saveFileName;

            if (loadGame(saveFileName, mainCharacter, currentDay))
            {
                std::cout << "Успешно заредено! Продължаваме от ден " << currentDay << ".\n";
                waitForKey();
                isGameReady = true;
            } 
            else 
            {
                std::cout << "ГРЕШКА! Файлът не е намерен.\n";
                std::cout << "Връщане към началното меню...\n";
                waitForKey();
            }
        }
    } while (!isGameReady);
    
    for (int day = currentDay; day <= SEMESTER_LENGTH; day++)
    {
        saveGameState(saveFileName, day, mainCharacter); // autosave game state
        
        int todayExamIndex = -1;
        for (int i = 0; i < NUMBER_OF_EXAMS; i++)
        {
            if (EXAM_DAYS[i] == day) todayExamIndex = i;
        }

        if (todayExamIndex != -1)
        {
            printHUD(day, mainCharacter);
            std::cout << "[1] Яви се на изпит \n";
            
            getValidInput(1, 1);
            attemptExam(mainCharacter, todayExamIndex);
            
            continue;
        } 

        if (mainCharacter.energy <= 0)
        {
            printHUD(day, mainCharacter);
            
            std::cout << "\n--- Претовари се! ---\n";
            std::cout << "Придна от умора, пропускаш деня и енергията ти се възстановява частично";
            waitForKey();
                    
            int energyBoost = randomWithMax(40);
            action faint = {"Faint", 0, energyBoost, -10, 0, 0};
            applyAction(mainCharacter, faint, false);
                    
            continue;
        }
        
        printHUD(day, mainCharacter);
        
        bool skipDay = triggerDailyEvent(mainCharacter);
        if (skipDay) continue;
        
        bool hasActed = false;
        while (!hasActed)
        {
            printHUD(day, mainCharacter);
            
            std::cout << "Какво искаш да направиш днес? {Ден: " << day << "}" << '\n'
              << "[1] Учиш \n"
              << "[2] Храниш се \n"
              << "[3] Излизаш \n"
              << "[4] Почивка \n"
              << "[5] Работиш \n"
              << "[6] Излез от играта" << std::endl;
            
            int mainChoice = getValidInput(1, 6);

            if (mainChoice == 1) hasActed = runSubMenu(mainCharacter, "Учене", STUDY_ACTIONS, 3);
            else if (mainChoice == 2) hasActed = runSubMenu(mainCharacter, "Хранене", FOOD_ACTIONS, 3);
            else if (mainChoice == 3) hasActed = runSubMenu(mainCharacter, "Излизане", FUN_ACTIONS, 3);
            else if (mainChoice == 4) hasActed = runSubMenu(mainCharacter, "Почивка", REST_ACTIONS, 3);
            else if (mainChoice == 5) hasActed = runSubMenu(mainCharacter, "Работа", WORK_ACTIONS, 3);
            else if (mainChoice == 6) return 0;
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
    
    waitForKey();
    return 0;
}