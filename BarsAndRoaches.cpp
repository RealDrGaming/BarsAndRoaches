/**
*  
* Solution to course project # 11
* Introduction to programming course
* Faculty of Mathematics and Informatics of Sofia University
* Winter semester 2025/2026
*
* @author Yordan Neshev
* @idnumber 9MI0600572
* @compiler VC
*
* <Game logic for 11th assignment: Student Quest>
*
*/

#include <fstream>
#include <iostream>
#include <windows.h> // needed for console encoding switch
#include <cstdlib> // needed for srand and rand functions
#include <ctime> // needed for time variation


// --- Structs for the student and action
struct student
{
    double money;
    int energy;
    int psyche;
    int knowledge;
    int physical;
    int passed_exams;
    int difficulty;
};

struct action
{
    const char* name;
    double cost;
    int deltaEnergy;
    int deltaPsyche;
    int deltaPhysical;
    int deltaKnowledge;
};
// ---

// -- Constants and exam days array
const int INPUT_LINE_MAX_SIZE = 100;
const char BASE_FILE_NAME[] = "SaveSlots.txt";
const int SEMESTER_LENGTH = 45;
const int NUMBER_OF_EXAMS = 5;

const int BASE_STAT_VALUE = 50;
const int MAX_PLAYER_ENERGY = 100;
const int MAX_PLAYER_PSYCHE = 100;
const int MAX_PLAYER_KNOWLEDGE = 100;
const int MAX_PLAYER_PHYSICAL = 100;

const int RANDOM_EVENT_CHANCE = 30;
const int SPOILED_FOOD_EVENT_CHANCE = 12;
const int EUREKA_EVENT_CHANCE = 15;
const int FOUND_MONEY_EVENT_CHANCE = 20;

const int KNOWLEDGE_DECAY_DIVISOR = 15;

const double EXAM_WEIGHT_KNOWLEDGE = 0.75;
const double EXAM_WEIGHT_PSYCHE = 0.1;
const double EXAM_WEIGHT_ENERGY = 0.1;
const double EXAM_WEIGHT_LUCK = 0.2;
const int EXAM_PASS_THRESHOLD = 50;
const int EXAM_DIFFICULTY_MODIFIER = 10; 
const int EXAM_PENALTY_MULTIPLIER = 7;

const int ENERGY_THRESHOLD_HIGH = 80;
const int ENERGY_THRESHOLD_LOW = 40;
const int FAIL_CHANCE_MEDIUM = 25;
const int FAIL_CHANCE_HIGH = 50;

const int RECOVERED_ENERGY_BASE = 40;
const int RECOVERED_ENERGY_RANDOM_MAX = 11;
const int RECOVERED_ENERGY_DIFFICULTY_MULTIPLIER = 5;
// ---


// --- Action constants
const action DAILY_EVENTS[] = {
    {"Мама и тате ти пращат пари", 20, 0, 0, 0, 0},
    {"Приятел те черпи кафе", 0, 0, 10, 0, 0},
    {"Разболял си се", 0, -20, 0, 0, 0},
    {"Няма ток в блока", 0, 0, 0, 0, 0},
    {"Хлъзгаш се и падаш", 0, 0, 0, -10, 0},
    {"Има безплатен концерт на любимата ти група", 0, 0, 20, 0, 0}
};


const action STUDY_ACTIONS[] = {
    {"Лекции (⚡-- / ☻ -- / ♥- / 📖++)", 0, -20, -20, -10, 20},
    {"Вкъщи сам (⚡- / ☻ -- / ♥- / 📖+++)", 0, -10, -20, -10, 30},
    {"Навън с приятели (⚡- / ☻ + / ♥- / 📖+)", 0, -10, 10, -10, 10}
};

const action FOOD_ACTIONS[] = {
    {"Стол (-12€ / ⚡++ / ☻ -)", -12, 20, -10, 0, 0},
    {"Дюнер (-16€ / ⚡++ / ☻ + / ♥-)", -16, 20, 10, -10, 0},
    {"Вкъщи (-25€ / ⚡+++)", -25, 30, 0, 0, 0}
};

const action FUN_ACTIONS[] = {
    {"Бар (-20€ / ⚡- / ☻ ++ / ♥-)", -15, -10, 20, -10, 0},
    {"Концерт (-50€ / ⚡-- / ☻ +++ / ♥--)", -50, -20, 30, -20, 0},
    {"Разходка (0€ / ⚡- / ☻ + / ♥+)", 0, -10, 10, 10, 0}
};

const action REST_ACTIONS[] = {
    {"Сън (⚡++ / ☻ +)", 0, 20, 10, 0, 0},
    {"Играй игри (⚡- / ☻ + / ♥-)", 0, -10, 10, -10, 0},
    {"Фитнес (⚡-- / ☻ ++ / ♥++)", 0, -20, 20, 20, 0}
};

const action WORK_ACTIONS[] = {
    {"Почасово (+15€ / ⚡- / ☻ -)", 15, -10, -10, 0, 0},
    {"Касиер (+30€ / ⚡-- / ☻ --)", 30, -20, -20, 0, 0},
    {"Таксиджия (+50€ / ⚡--- / ☻ ---)", 50, -30, -30, 0, 0}
};
// ---


// --- helper functions
int randomWithMax(int oneInX)
{
    return std::rand() % oneInX;
}

bool containsSpace(const char* str)
{
    int i = 0;
    while (str[i] != '\0')
    {
        if (str[i] == ' ') return true;
        i++;
    }
    return false;
}

int my_clamp(int value, int minVal, int maxVal)
{
    if (value < minVal) return minVal;
    if (value > maxVal) return maxVal;
    return value;
}

void waitForKey()
{
    std::cout << "Натисни Enter за да продължиш...";
    std::cin.get();
}

bool stringsMatch(const char* a, const char* b)
{
    int i = 0;

    while (a[i] != '\0' && b[i] != '\0')
    {
        if (a[i] != b[i]) return false;
        i++;
    }

    return (a[i] == '\0' && b[i] == '\0');
}

int stringToInt(const char* str)
{
    int result = 0;
    int index = 0;
    
    while (str[index] == ' ') index++; // Skip leading whitespaces

    int sign = 1;
    if (str[index] == '-' || str[index] == '+')
    {
        if (str[index] == '-') sign = -1;
        index++;
    }

    while (str[index] >= '0' && str[index] <= '9')
    {
        result = result * 10 + (str[index] - '0');
        index++;
    }

    return result * sign; // *1 if sign was + and *(-1) if sign -
}

int getValidInput(int min, int max)
{
    char buffer[INPUT_LINE_MAX_SIZE];
    
    while (true)
    {
        std::cout << " > ";

        std::cin.getline(buffer, INPUT_LINE_MAX_SIZE);
        int choice = stringToInt(buffer);
        
        if (choice >= min && choice <= max) 
            return choice;
        
        std::cout << "Невалидна команда, опитай отново!\n";
    }
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
// ---


// --- save-load file system
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

bool saveFileExists(const char* fileName)
{
    std::ifstream list(BASE_FILE_NAME);
    if (!list.is_open()) return false;

    char tempName[INPUT_LINE_MAX_SIZE];
    while (list >> tempName)
    {
        if (stringsMatch(tempName, fileName))
        {
            list.close();
            return true; // file exists
        }
    }
    list.close();
    return false; // file does not exist
}

void printAvailableSaves()
{
    std::ifstream list(BASE_FILE_NAME);
    if (!list.is_open())
    {
        std::cout << " (Няма намерени записи)" << std::endl;
        return;
    }

    char tempName[INPUT_LINE_MAX_SIZE];
    std::cout << "--- Налични игри ---" << std::endl;
    while (list >> tempName)
    {
        std::cout << " > " << tempName << std::endl;
    }
    std::cout << "--------------------" << std::endl;
    list.close();
}

void saveGameState(const char* fileName, int day, const student& student)
{
    std::ofstream file(fileName, std::ios::app);

    if (file.is_open())
    {
        file << day << ' '
            << student.money << ' '
            << student.energy << ' '
            << student.psyche << ' '
            << student.physical << ' '
            << student.knowledge << ' '
            << student.passed_exams << ' '
            << student.difficulty << std::endl;

        file.close();
    }
}

bool loadGame(const char* fileName, student& student, int& dayOut)
{
    std::ifstream file(fileName);
    if (!file.is_open()) return false;

    int day, energy, psyche, physical, knowledge, exams, diff;
    double money;
    bool success = false;

    while (file >> day >> money >> energy >> psyche >> physical >> knowledge >> exams >> diff)
    {
        dayOut = day;
        student.money = money;
        student.energy = energy;
        student.psyche = psyche;
        student.physical = physical;
        student.knowledge = knowledge;
        student.passed_exams = exams;
        student.difficulty = diff;
        success = true;
    }

    file.close();
    return success;
}
// ---


// --- Printing to the console and UI
void printStatDiff(const char* label, int oldVal, int newVal, const char* suffix = "")
{
    if (oldVal != newVal)
    {
        int diff = newVal - oldVal;
        std::cout << "  " << (diff > 0 ? "+" : "") << diff << " " << label << suffix << std::endl;
    }
}

void printProgressBar(int current)
{
    int totalBlocks = 10;
    int filled = (current * totalBlocks) / 100;

    for (int i = 0; i < filled; i++) std::cout << "▓";

    for (int i = filled; i < totalBlocks; i++) std::cout << "░";
}

void printHUD(const student& student, int day, const int* exam_days)
{
    int nextExamDay = getNextExamDay(day, exam_days, NUMBER_OF_EXAMS);

    system("cls");

    std::cout << "╭─────────────────────────────────────╮\n"
        << "     Ден " << day << " от " << SEMESTER_LENGTH << "\n"
        << "     Следващият изпит е на ден " << nextExamDay << "\n"
        << " ───────────────────────────────────── \n"
        << "     » Пари: " << student.money << "€\n"
        << "     » ⚡ Енергия: ";
    printProgressBar(student.energy);
    std::cout << " (" << student.energy << ")\n"
        << "     » ☻ Психика: ";
    printProgressBar(student.psyche);
    std::cout << " (" << student.psyche << ")\n"
        << "     » ♥ Здраве: ";
    printProgressBar(student.physical);
    std::cout << " (" << student.physical << ")\n"
        << "     » 📖 Знание:  ";
    printProgressBar(student.knowledge);
    std::cout << " (" << student.knowledge << ")\n"
        << "     » Взети изпити:  " << student.passed_exams << "/" << NUMBER_OF_EXAMS << "\n";
    std::cout << "╰─────────────────────────────────────╯" << std::endl;
}
// ---


// --- Stat and action calculation
int calculateNewStat(int current, int delta, int divisor, int maxVal)
{
    int change = (delta > 0) ? (delta / divisor) : delta;
    return my_clamp(current + change, 0, maxVal);
}

void applyAction(student& studentToChange, const action& act, bool checkEfficiency = true)
{
    student oldStud = studentToChange;

    bool isSuccess = true;

    if (checkEfficiency)
    {
        int chanceToFail = 0;

        if (studentToChange.energy > ENERGY_THRESHOLD_HIGH) chanceToFail = 0;
        else if (studentToChange.energy > ENERGY_THRESHOLD_LOW) chanceToFail = FAIL_CHANCE_MEDIUM;
        else chanceToFail = FAIL_CHANCE_HIGH;

        int roll = randomWithMax(100);

        if (roll >= 100 - chanceToFail)
        {
            isSuccess = false;
            std::cout << "Уморен си! Действието не беше напълно ефективно!\n";
        }
    }

    int divisor = (!isSuccess && checkEfficiency) ? 2 : 1;

    double moneyChange = act.cost;
    if (!isSuccess && moneyChange > 0) moneyChange /= 2;
    studentToChange.money += moneyChange;

    studentToChange.energy = 
        calculateNewStat(studentToChange.energy, act.deltaEnergy, divisor, MAX_PLAYER_ENERGY);
    studentToChange.psyche = 
        calculateNewStat(studentToChange.psyche, act.deltaPsyche, divisor, MAX_PLAYER_PSYCHE);
    studentToChange.physical = 
        calculateNewStat(studentToChange.physical, act.deltaPhysical, divisor, MAX_PLAYER_PHYSICAL);
    studentToChange.knowledge = 
        calculateNewStat(studentToChange.knowledge, act.deltaKnowledge, divisor, MAX_PLAYER_KNOWLEDGE);

    std::cout << "\n ──────────────────────────\n";

    if (studentToChange.money != oldStud.money)
    {
        double diff = studentToChange.money - oldStud.money;
        std::cout << "  " << (diff > 0 ? "+" : "") << diff << "€\n";
    }
    printStatDiff("Енергия", oldStud.energy, studentToChange.energy);
    printStatDiff("Психика", oldStud.psyche, studentToChange.psyche);
    printStatDiff("Здраве", oldStud.physical, studentToChange.physical);
    printStatDiff("Знание", oldStud.knowledge, studentToChange.knowledge);

    std::cout << " ──────────────────────────\n";

    waitForKey();
}

bool isActionAllowed(const student& student, const char* actionName)
{
    const char* target = "Сън";
    bool isSleep = true;

    for (int i = 0; target[i] != '\0'; i++)
    {
        if (actionName[i] == '\0' || actionName[i] != target[i])
        {
            isSleep = false;
            break;
        }
    }

    if (isSleep && student.energy > 90)
    {
        std::cout << "\n Не можеш да заспиш, имаш твърде много енергия \n"
            << "   Направи нещо друго, за да се умориш." << std::endl;
        waitForKey();
        return false;
    }

    return true;
}
// ---


// --- Chance based and mandatory events
void triggerSideEffect(student& student, const char* category)
{
    if (stringsMatch(category, "Хранене"))
    {
        // Spoiled food event
        if (randomWithMax(SPOILED_FOOD_EVENT_CHANCE) == 0)
        {
            std::cout << "\n О, не! Храната ти беше развалена... (-10 Енергия)" << std::endl;
            student.energy = my_clamp(student.energy - 10, 0, MAX_PLAYER_ENERGY);
        }
    }
    else if (stringsMatch(category, "Учене"))
    {
        // Eureka event
        if (randomWithMax(EUREKA_EVENT_CHANCE) == 0)
        {
            std::cout << "\n Еврика! Разбра материала перфектно! (+10 Знания)" << std::endl;
            student.knowledge = my_clamp(student.knowledge + 10, 0, MAX_PLAYER_KNOWLEDGE);
        }
    }
    else if (stringsMatch(category, "Излизане"))
    {
        // Lucky event ig
        if (randomWithMax(FOUND_MONEY_EVENT_CHANCE) == 0)
        {
            std::cout << "\n Намери 20€ на земята пред дискотеката! (+20 Пари)" << std::endl;
            student.money += 20;
        }
    }
}

void attemptExam(student& student, int examIndex)
{
    int luckCoeff = randomWithMax(100);
    int penalty = examIndex * EXAM_PENALTY_MULTIPLIER;

    int difficultyMod = 0;
    if (student.difficulty == 1) difficultyMod = EXAM_DIFFICULTY_MODIFIER;
    else if (student.difficulty == 3) difficultyMod = -EXAM_DIFFICULTY_MODIFIER;

    double successChance = (student.knowledge * EXAM_WEIGHT_KNOWLEDGE)
        + (student.psyche * EXAM_WEIGHT_PSYCHE)
        + (student.energy * EXAM_WEIGHT_ENERGY)
        + (luckCoeff * EXAM_WEIGHT_LUCK)
        - penalty
        + difficultyMod;

    std::cout << "\n--- РЕЗУЛТАТ ОТ ИЗПИТА ---" << std::endl;
    if (successChance > EXAM_PASS_THRESHOLD)
    {
        std::cout << "Изпитът е взет!" << std::endl;
        student.passed_exams++;
        action reward = {"Pass", 0, -20, 20, 0, 0};
        applyAction(student, reward, false);
    }
    else
    {
        std::cout << "Скъсан си!" << std::endl;
        action fail = {"Fail", 0, -20, -30, 0, 0};
        applyAction(student, fail, false);
    }
}

void triggerRandomEvent(student& student, bool& shouldSkipDay)
{
    int eventIndex = randomWithMax(6);

    std::cout << "╭──────────────────────────────────────────╮\n"
        << "│            СЛУЧАЙНО СЪБИТИЕ!             │\n"
        << " ────────────────────────────────────────── \n"
        << "      " << DAILY_EVENTS[eventIndex].name << "\n"
        << "╰──────────────────────────────────────────╯" << std::endl;

    if (eventIndex == 3)
    {
        std::cout << " (Пропускаш действието за деня...)" << std::endl;
        waitForKey();
        
        shouldSkipDay = true;
    }

    applyAction(student, DAILY_EVENTS[eventIndex], false);
}
// ---


// --- Open-up sub menu for daily actions
bool runSubMenu(student& student, const char* title, const action actions[], int count)
{
    std::cout << "╭───────────────────────────────────────────╮\n"
        << "            " << title << "\n"
        << " ───────────────────────────────────────────\n";

    for (int i = 1; i <= count; i++)
    {
        std::cout << " [" << (i) << "] " << actions[i - 1].name << "\n";
    }

    std::cout << " [" << (count + 1) << "] Назад \n";
    std::cout << "╰───────────────────────────────────────────╯\n";

    int choice = getValidInput(1, count + 1);

    if (choice == count + 1) return false;

    const action& selectedAction = actions[choice - 1];

    if (!isActionAllowed(student, selectedAction.name))
    {
        return false;
    }

    bool riskOfFailure = true;
    if (stringsMatch(title, "Почивка"))
    {
        riskOfFailure = false;
    }

    applyAction(student, selectedAction, riskOfFailure);
    triggerSideEffect(student, title);

    return true;
}
// ---


// --- Game loop functions
void handleNewFileCreation(char* saveFileName)
{
    while (true)
    {
        std::cout << "Как искаш да кръстиш новия файл? {Без празни места!}" << std::endl;
        std::cout << " > ";
    
        std::cin.getline(saveFileName, INPUT_LINE_MAX_SIZE);
                
        if (containsSpace(saveFileName)) 
        {
            std::cout << "Името не трябва да има празни места!" << std::endl;
            continue;
        }
                
        if (saveFileName[0] == '\0') 
        {
            std::cout << "Името не може да е празно!" << std::endl;
            continue;
        }

        if (saveFileExists(saveFileName))
        {
            std::cout << "Файл с това име вече съществува!" << std::endl;
            continue;
        }
                
        break;
    }
}

void chooseDiffAndInitCharacter(student& student, int& day, bool& shouldGoBack)
{
    std::cout << "╭───────────────────────────────────────────────╮ \n"
                << "│          Избери своята специалност:           │ \n"
                << "│      [1] Софтуерно инженерство  |  ЛЕСНО      │ \n"
                << "│        [2] Компютърни науки  |  СРЕДНО        │ \n"
                << "│           [3] Информатика  |  ТРУДНО          │ \n"
                << "│                  [4] Назад                    │ \n"
                << "╰───────────────────────────────────────────────╯" << std::endl;

    int difficulty = getValidInput(1, 4);

    // if player choses 4 we go back
    if (difficulty == 4)
    {
        shouldGoBack = true;
        return;
    }
    
    // informatika with the highest starting stats, cuz easiest to get in, so the least burnout
    int statWithDifficultyMod = difficulty * 10 + BASE_STAT_VALUE;

    student =
    {
        BASE_STAT_VALUE, statWithDifficultyMod, statWithDifficultyMod,
        statWithDifficultyMod, statWithDifficultyMod, 0, difficulty
    };
    day = 1;
}

bool initializeGame(student& student, int& day, char* saveFileName)
{
    while (true)
    {
        system("cls");
        
        std::cout << "╭──────────────────────────╮\n"
            << "│     Bars and Roaches     │\n"
            << "│       [1] Нов файл       │\n"
            << "│       [2] Продължи       │\n"
            << "│       [3] Напусни        │\n"
            << "╰──────────────────────────╯" << std::endl;

        int choice = getValidInput(1, 3);

        if (choice == 1) // new game
        {
            handleNewFileCreation(saveFileName);

            bool shouldGoBack = false;
            chooseDiffAndInitCharacter(student, day, shouldGoBack);
            if (shouldGoBack) 
                continue;

            registerSaveFile(saveFileName);
            
            return true;
        }
        else if (choice == 2) // continuing
        {
            printAvailableSaves();

            std::cout << "Въведи името на файла точно: ";
            std::cin.getline(saveFileName, INPUT_LINE_MAX_SIZE);

            if (loadGame(saveFileName, student, day))
            {
                std::cout << "Успешно заредено! Продължаваме от ден " << day << std::endl;
                waitForKey();
                return true;
            }

            // if we are here, file was not found
            std::cout << "Файлът не е намерен.\n"
                << "Връщане към началното меню..." << std::endl;
            waitForKey();
        }
        else if (choice == 3)
        {
            return false;
        }
    }
}

void handleExam(student& student, int day, const int* exam_dates, bool& dayShouldSkip)
{
    int examIndex = -1;
    for (int i = 0; i < NUMBER_OF_EXAMS; i++)
    {
        if (exam_dates[i] == day) examIndex = i;
    }

    if (examIndex != -1)
    {
        printHUD(student, day, exam_dates);
        std::cout << "[1] Яви се на изпит" << std::endl;

        getValidInput(1, 1);
        attemptExam(student, examIndex);

        dayShouldSkip = true;
    }
}

void handleFaint(student& student, int day, const int* exam_days, bool& dayShouldSkip)
{
    printHUD(student, day, exam_days);

    std::cout << "\n--- Претовари се! ---\n"
        << "Припадна от умора. Пропускаш деня и енергията ти се възстановява частично" << std::endl;
    waitForKey();
    
    int recoveredEnergy = RECOVERED_ENERGY_BASE + 
            randomWithMax(RECOVERED_ENERGY_RANDOM_MAX) - 
            (student.difficulty * RECOVERED_ENERGY_DIFFICULTY_MULTIPLIER);
    
    action faint = {"Faint", 0, recoveredEnergy, -10, 0, 0};
    applyAction(student, faint, false);
    
    dayShouldSkip = true;
}

bool handleDailyAction(student& student, int day, const int* exam_days)
{
    bool hasActed = false;
    while (!hasActed)
    {
        printHUD(student, day, exam_days);

        std::cout << "Какво искаш да направиш днес?\n"
            << "[1] Учиш \n"
            << "[2] Храниш се \n"
            << "[3] Излизаш \n"
            << "[4] Почивка \n"
            << "[5] Работиш \n"
            << "[6] Излез от играта" << std::endl;

        int mainChoice = getValidInput(1, 6);

        if (mainChoice == 1) hasActed = runSubMenu(student, "Учене", STUDY_ACTIONS, 3);
        else if (mainChoice == 2) hasActed = runSubMenu(student, "Хранене", FOOD_ACTIONS, 3);
        else if (mainChoice == 3) hasActed = runSubMenu(student, "Излизане", FUN_ACTIONS, 3);
        else if (mainChoice == 4) hasActed = runSubMenu(student, "Почивка", REST_ACTIONS, 3);
        else if (mainChoice == 5) hasActed = runSubMenu(student, "Работа", WORK_ACTIONS, 3);
        else if (mainChoice == 6) return false;
    }

    return true;
}

bool checkGameOver(const student& student)
{
    if (student.money <= 0)
    {
        std::cout << "╭────────────────────────────────────────╮\n"
            << "│                Загуба!                 │\n"
            << "│           Свършиха ти парите           │\n"
            << "╰────────────────────────────────────────╯" << std::endl;
        waitForKey();
        return true;
    }
    if (student.psyche <= 0)
    {
        std::cout << "╭────────────────────────────────────────╮\n"
            << "│                Загуба!                 │\n"
            << "│        Психиката ти не издържа         │\n"
            << "╰────────────────────────────────────────╯" << std::endl;
        waitForKey();
        return true;
    }
    if (student.physical <= 0)
    {
        std::cout << "╭────────────────────────────────────────╮\n"
            << "│                Загуба!                 │\n"
            << "│     Физическата ти форма се стопи      │\n"
            << "╰────────────────────────────────────────╯" << std::endl;
        waitForKey();
        return true;
    }
    return false;
}

void applyNightlyDecay(student& student)
{
    int amountToLose = student.knowledge / KNOWLEDGE_DECAY_DIVISOR;

    if (student.psyche > 80)
    {
        amountToLose /= 2;
    }
    else if (student.psyche < 30)
    {
        amountToLose += 5;
    }

    if (student.knowledge > 0 && amountToLose < 1) amountToLose = 1; // minimum knowledge loss

    int previousKnowledge = student.knowledge;
    student.knowledge = my_clamp(student.knowledge - amountToLose, 0, MAX_PLAYER_KNOWLEDGE);

    if (previousKnowledge > student.knowledge && amountToLose > 1)
    {
        std::cout << "През нощта част от знанията ти избледняха...\n"
            << "   (-" << (previousKnowledge - student.knowledge) << " Знания)" << std::endl;
        waitForKey();
    }
}
// ---


int main(int argc, char* argv[])
{
    // switch to utf-8 encoding, so the console recognizes ascii symbols and cyrillic
    SetConsoleOutputCP(CP_UTF8);
    
    // randomize the seed of the rand function in relation to current local time
    std::srand(std::time(0));

    // initialize exam dates and make fourth date random
    int exam_days[] = {8, 17, 26, 0, 45}; // fourth date
    exam_days[3] = (exam_days[2] + 1) + randomWithMax(exam_days[4] - exam_days[2] - 1);
    
    student mainCharacter;
    int currentDay = 1;
    char saveFileName[INPUT_LINE_MAX_SIZE];

    // initializing game, start menu, difficulty choosing, save file choosing
    if (!initializeGame(mainCharacter, currentDay, saveFileName)) 
        return 0;

    // main game loop
    for (int day = currentDay; day <= SEMESTER_LENGTH; day++)
    {
        bool shouldSkipDay = false;
        
        saveGameState(saveFileName, day, mainCharacter); // autosave character stats

        handleExam(mainCharacter, day, exam_days, shouldSkipDay);
        if (shouldSkipDay) continue;

        if (mainCharacter.energy <= 0)
        {
            handleFaint(mainCharacter, day, exam_days, shouldSkipDay);
            if (shouldSkipDay) continue;
        }

        if (randomWithMax(RANDOM_EVENT_CHANCE) == 0)
        {
            triggerRandomEvent(mainCharacter, shouldSkipDay);
            if (shouldSkipDay) continue;
        }
        
        // returns false if player chose to leave game and true if he chose anything else
        if (!handleDailyAction(mainCharacter, day, exam_days)) return 0;

        if (checkGameOver(mainCharacter)) return 0;

        // character survived, we apply decay to knowledge to prevent sleep spamming
        applyNightlyDecay(mainCharacter);
    }
    
    system("cls");
        
    if (mainCharacter.passed_exams == NUMBER_OF_EXAMS)
    {
        std::cout << "╭────────────────────────────────────────╮\n"
            << "│              ПОЗДРАВЛЕНИЯ!             │\n"
            << "│       Печелиш, взе всички изпити       │\n"
            << "╰────────────────────────────────────────╯" << std::endl;
    }
    else
    {
        std::cout << "╭────────────────────────────────────────╮\n"
            << "│                ЗАГУБА!                 │\n"
            << "│   Невзети изпити по време на сесията   │\n"
            << "╰────────────────────────────────────────╯" << std::endl;
    }
    
    return 0;
}