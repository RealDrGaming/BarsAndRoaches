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
    {"Лекции (⚡-- / ☻ - / ♥- / 📖++)", 0, -20, -10, -10, 20},
    {"Вкъщи сам (⚡- / ☻ --- / ♥- / 📖+++)", 0, -10, -30, -10, 30},
    {"Навън с приятели (⚡- / ☻ + / ♥- / 📖+)", 0, -10, 10, -10, 10}
};

const action FOOD_ACTIONS[] = {
    {"Стол (-8€ / ⚡++ / ☻ -)", -8, 20, -10, 0, 0},
    {"Дюнер (-12€ / ⚡++ / ☻ + / ♥--)", -12, 20, 10, -20, 0},
    {"Вкъщи (-12€ / ⚡++)", -12, 20, 0, 0, 0}
};

const action FUN_ACTIONS[] = {
    {"Бар (-15€ / ⚡- / ☻ ++ / ♥-)", -15, -10, 20, -10, 0},
    {"Концерт (-50€ / ⚡-- / ☻ +++ / ♥--)", -50, -20, 30, -20, 0},
    {"Разходка (0€ / ⚡- / ☻ + / ♥+)", 0, -10, 10, 10, 0}
};

const action REST_ACTIONS[] = {
    {"Сън (⚡+++ / ☻ +)", 0, 30, 10, 0, 0},
    {"Играй игри (⚡- / ☻ + / ♥-)", 0, -10, 10, -10, 0},
    {"Фитнес (⚡-- / ☻ ++ / ♥++)", 0, -20, 20, 20, 0}
};

const action WORK_ACTIONS[] = {
    {"Почасово (+30€ / ⚡- / ☻ -)", 30, -10, -10, 0, 0},
    {"Касиер (+50€ / ⚡-- / ☻ --)", 50, -20, -20, 0, 0},
    {"Таксиджия (+70€ / ⚡--- / ☻ ---)", 70, -30, -30, 0, 0}
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
    int i = 0;
    
    while (str[i] == ' ') i++; // Skip leading whitespaces

    int sign = 1;
    if (str[i] == '-' || str[i] == '+')
    {
        if (str[i] == '-') sign = -1;
        i++;
    }

    while (str[i] >= '0' && str[i] <= '9')
    {
        result = result * 10 + (str[i] - '0');
        i++;
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
            << s.passed_exams << " "
            << s.difficulty << std::endl;

        file.close();
    }
}

bool loadGame(const char* fileName, student& s, int& dayOut)
{
    std::ifstream file(fileName);
    if (!file.is_open()) return false;

    int day, energy, psyche, physical, knowledge, exams, diff;
    double money;
    bool success = false;

    while (file >> day >> money >> energy >> psyche >> physical >> knowledge >> exams >> diff)
    {
        dayOut = day;
        s.money = money;
        s.energy = energy;
        s.psyche = psyche;
        s.physical = physical;
        s.knowledge = knowledge;
        s.passed_exams = exams;
        s.difficulty = diff;
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
        std::cout << "  " << (diff > 0 ? "+" : "") << diff << " " << label << suffix << "\n";
    }
}

void printProgressBar(int current)
{
    int totalBlocks = 10;
    int filled = (current * totalBlocks) / 100;

    for (int i = 0; i < filled; i++) std::cout << "▓";

    for (int i = filled; i < totalBlocks; i++) std::cout << "░";
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
        << "     » Енергия: ";
    printProgressBar(s.energy);
    std::cout << " (" << s.energy << ")\n"
        << "     » Психика: ";
    printProgressBar(s.psyche);
    std::cout << " (" << s.psyche << ")\n"
        << "     » Здраве: ";
    printProgressBar(s.physical);
    std::cout << " (" << s.physical << ")\n"
        << "     » Знание:  ";
    printProgressBar(s.knowledge);
    std::cout << " (" << s.knowledge << ")\n"
        << "     » Взети изпити:  " << s.passed_exams << "/" << NUMBER_OF_EXAMS << "\n";
    std::cout << "╰─────────────────────────────────────╯" << std::endl;
}
// ---


// --- Stat and action calculation
int calculateNewStat(int current, int delta, int divisor, int maxVal)
{
    int change = (delta > 0) ? (delta / divisor) : delta;
    return my_clamp(current + change, 0, maxVal);
}

void applyAction(student& s, const action& act, bool checkEfficiency = true)
{
    student oldStud = s;

    bool isSuccess = true;

    if (checkEfficiency)
    {
        int chanceToSucceed = 0;

        if (s.energy > 80) chanceToSucceed = 100;
        else if (s.energy > 40) chanceToSucceed = 75;
        else chanceToSucceed = 50;

        int roll = randomWithMax(100);

        if (roll >= chanceToSucceed)
        {
            isSuccess = false;
            std::cout << "Уморен си! Действието не беше напълно ефективно!\n";
        }
    }

    int divisor = (!isSuccess && checkEfficiency) ? 2 : 1;

    double moneyChange = act.cost;
    if (!isSuccess && moneyChange > 0) moneyChange /= 2;
    s.money += moneyChange;

    s.energy = calculateNewStat(s.energy, act.deltaEnergy, divisor, MAX_PLAYER_ENERGY);
    s.psyche = calculateNewStat(s.psyche, act.deltaPsyche, divisor, MAX_PLAYER_PSYCHE);
    s.physical = calculateNewStat(s.physical, act.deltaPhysical, divisor, MAX_PLAYER_PHYSICAL);
    s.knowledge = calculateNewStat(s.knowledge, act.deltaKnowledge, divisor, MAX_PLAYER_KNOWLEDGE);

    std::cout << "\n ──────────────────────────\n";

    if (s.money != oldStud.money)
    {
        double diff = s.money - oldStud.money;
        std::cout << "  " << (diff > 0 ? "+" : "") << diff << "€\n";
    }
    printStatDiff("Енергия", oldStud.energy, s.energy);
    printStatDiff("Психика", oldStud.psyche, s.psyche);
    printStatDiff("Здраве", oldStud.physical, s.physical);
    printStatDiff("Знание", oldStud.knowledge, s.knowledge);

    std::cout << " ──────────────────────────\n";

    waitForKey();
}

bool isActionAllowed(const student& s, const char* actionName)
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

    if (isSleep && s.energy > 90)
    {
        std::cout << "\n ⚠ Не можеш да заспиш, имаш твърде много енергия \n"
            << "   Направи нещо друго, за да се умориш.\n";
        waitForKey();
        return false;
    }

    return true;
}
// ---


// --- Chance based and mandatory events
void triggerSideEffect(student& s, const char* category)
{
    if (stringsMatch(category, "Хранене"))
    {
        // Spoiled food event
        if (randomWithMax(12) == 0)
        {
            std::cout << "\n О, не! Храната ти беше развалена... (-10 Енергия)\n";
            s.energy = my_clamp(s.energy - 10, 0, MAX_PLAYER_ENERGY);
        }
    }
    else if (stringsMatch(category, "Учене"))
    {
        // Eureka event
        if (randomWithMax(20) == 0)
        {
            std::cout << "\n Еврика! Разбра материала перфектно! (+10 Знания)\n";
            s.knowledge = my_clamp(s.knowledge + 10, 0, MAX_PLAYER_KNOWLEDGE);
        }
    }
    else if (stringsMatch(category, "Излизане"))
    {
        // Lucky event ig
        if (randomWithMax(30) == 0)
        {
            std::cout << "\n Намери 20€ на земята пред дискотеката! (+20 Пари)\n";
            s.money += 20;
        }
    }
}

void attemptExam(student& s, int examIndex)
{
    int luckCoeff = randomWithMax(100);
    int penalty = examIndex * 7;

    int difficultyMod = 0;
    if (s.difficulty == 1) difficultyMod = 10;
    else if (s.difficulty == 3) difficultyMod = -10;

    double successChance = (s.knowledge * 0.75)
        + (s.psyche * 0.1)
        + (s.energy * 0.1)
        + (luckCoeff * 0.2)
        - penalty
        + difficultyMod;

    std::cout << "\n--- РЕЗУЛТАТ ОТ ИЗПИТА ---\n";
    if (successChance > 50)
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
    }
    return false;
}
// ---


// --- Open-up sub menu for daily actions
bool runSubMenu(student& s, const char* title, const action actions[], int count)
{
    std::cout << "╭───────────────────────────────────────────────────╮\n"
        << "            " << title << "\n"
        << " ───────────────────────────────────────────────────\n";

    for (int i = 0; i < count; i++)
    {
        std::cout << " [" << (i + 1) << "] " << actions[i].name << "\n";
    }

    std::cout << " [" << (count + 1) << "] Назад \n";
    std::cout << "╰───────────────────────────────────────────────────╯\n";

    int choice = getValidInput(1, count + 1);

    if (choice == count + 1) return false;

    const action& selectedAction = actions[choice - 1];

    if (!isActionAllowed(s, selectedAction.name))
    {
        return false;
    }

    bool riskOfFailure = true;
    if (stringsMatch(title, "Почивка"))
    {
        riskOfFailure = false;
    }

    applyAction(s, selectedAction, riskOfFailure);
    triggerSideEffect(s, title);

    return true;
}
// ---


// --- Game loop functions
bool initializeGame(student& s, int& day, char* saveFileName)
{
    while (true)
    {
        system("cls");
        std::cout << "╭──────────────────────────╮ \n"
            << "│     Bars and Roaches     │ \n"
            << "│       [1] Нов файл       │ \n"
            << "│       [2] Продължи       │ \n"
            << "│       [3] Напусни        │ \n"
            << "╰──────────────────────────╯ \n";

        int choice = getValidInput(1, 3);

        if (choice == 1) // new game
        {
            while (true) 
            {
                std::cout << "Как искаш да кръстиш новия файл? {Без празни места!}" << '\n';
                std::cout << " > ";
    
                std::cin.getline(saveFileName, INPUT_LINE_MAX_SIZE);
                
                if (containsSpace(saveFileName)) 
                {
                    std::cout << "Името не трябва да има празни места!\n";
                    continue;
                }
                
                if (saveFileName[0] == '\0') 
                {
                    std::cout << "Името не може да е празно!\n";
                    continue;
                }

                if (saveFileExists(saveFileName))
                {
                    std::cout << "Файл с това име вече съществува!\n";
                    continue;
                }
                
                break;
            }

            std::cout << "╭───────────────────────────────────────────────╮ \n"
                << "│          Избери своята специалност:           │ \n"
                << "│      [1] Софтуерно инженерство  |  ЛЕСНО      │ \n"
                << "│        [2] Компютърни науки  |  СРЕДНО        │ \n"
                << "│           [3] Информатика  |  ТРУДНО          │ \n"
                << "│                  [4] Назад                    │ \n"
                << "╰───────────────────────────────────────────────╯" << std::endl;

            int diff = getValidInput(1, 4);

            // if player choses 4 we go back
            if (diff == 4) continue;
            
            // informatika with the highest starting stats, cuz easiest to get in, so the least burnout
            int baseValue = diff * 10 + BASE_STAT_VALUE;

            s =
            {
                BASE_STAT_VALUE, baseValue, baseValue,
                baseValue, baseValue, 0, diff
            };
            day = 1;
            
            registerSaveFile(saveFileName);
            
            return true;
        }
        else if (choice == 2) // continuing
        {
            printAvailableSaves();

            std::cout << "Въведи името на файла точно: ";
            std::cin.getline(saveFileName, INPUT_LINE_MAX_SIZE);

            if (loadGame(saveFileName, s, day))
            {
                std::cout << "Успешно заредено! Продължаваме от ден " << day << ".\n";
                waitForKey();
                return true;
            }

            // if we are here, file was not found
            std::cout << "Файлът не е намерен.\n";
            std::cout << "Връщане към началното меню...\n";
            waitForKey();
        }
        else if (choice == 3)
        {
            return false;
        }
    }
}

bool checkAndHandleFaint(student& s, int day)
{
    if (s.energy > 0) return false;

    printHUD(day, s);

    std::cout << "\n--- Претовари се! ---\n";
    std::cout << "Припадна от умора, пропускаш деня и енергията ти се възстановява частично";
    waitForKey();

    int recoveredEnergy = 40 - s.difficulty + randomWithMax(20);
    action faint = {"Faint", 0, recoveredEnergy, -10, 0, 0};
    applyAction(s, faint, false);

    return true; // True means character fainted so we skip the day
}

bool handleDailyActions(student& s, int day)
{
    bool hasActed = false;
    while (!hasActed)
    {
        printHUD(day, s);

        std::cout << "Какво искаш да направиш днес? {Ден: " << day << "}" << '\n'
            << "[1] Учиш \n"
            << "[2] Храниш се \n"
            << "[3] Излизаш \n"
            << "[4] Почивка \n"
            << "[5] Работиш \n"
            << "[6] Излез от играта" << std::endl;

        int mainChoice = getValidInput(1, 6);

        if (mainChoice == 1) hasActed = runSubMenu(s, "Учене", STUDY_ACTIONS, 3);
        else if (mainChoice == 2) hasActed = runSubMenu(s, "Хранене", FOOD_ACTIONS, 3);
        else if (mainChoice == 3) hasActed = runSubMenu(s, "Излизане", FUN_ACTIONS, 3);
        else if (mainChoice == 4) hasActed = runSubMenu(s, "Почивка", REST_ACTIONS, 3);
        else if (mainChoice == 5) hasActed = runSubMenu(s, "Работа", WORK_ACTIONS, 3);
        else if (mainChoice == 6) return false;
    }

    return true;
}

bool checkGameOver(const student& s)
{
    if (s.money <= 0)
    {
        std::cout << "╭────────────────────────────────────────╮ \n"
            << "│                Загуба!                 │ \n"
            << "│           Свършиха ти парите           │ \n"
            << "╰────────────────────────────────────────╯ \n" << std::endl;
        waitForKey();
        return true;
    }
    if (s.psyche <= 0)
    {
        std::cout << "╭────────────────────────────────────────╮ \n"
            << "│                Загуба!                 │ \n"
            << "│        Психиката ти не издържа         │ \n"
            << "╰────────────────────────────────────────╯ \n" << std::endl;
        waitForKey();
        return true;
    }
    return false;
}

void applyNightlyDecay(student& s)
{
    int amountToLose = s.knowledge / 15;

    if (s.psyche > 80)
    {
        amountToLose /= 2;
    }
    else if (s.psyche < 30)
    {
        amountToLose += 5;
    }

    if (s.knowledge > 0 && amountToLose < 1) amountToLose = 1;

    int previousKnowledge = s.knowledge;
    s.knowledge = my_clamp(s.knowledge - amountToLose, 0, MAX_PLAYER_KNOWLEDGE);

    if (previousKnowledge > s.knowledge && amountToLose > 1)
    {
        std::cout << "През нощта част от знанията ти избледняха...\n"
            << "   (-" << (previousKnowledge - s.knowledge) << " Знания)\n";
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

    // randomize third exam date
    EXAM_DAYS[3] = 27 + randomWithMax(18);
    
    student mainCharacter;
    int currentDay = 1;
    char saveFileName[INPUT_LINE_MAX_SIZE];

    // initializing game, start menu, difficulty choosing, save file choosing
    if (!initializeGame(mainCharacter, currentDay, saveFileName)) return 0;

    // end of semester flag
    bool semesterPassed = false;

    // main game loop
    for (int day = currentDay; day <= SEMESTER_LENGTH; day++)
    {
        saveGameState(saveFileName, day, mainCharacter); // autosave character stats

        int examIndex = -1;
        for (int i = 0; i < NUMBER_OF_EXAMS; i++)
        {
            if (EXAM_DAYS[i] == day) examIndex = i;
        }

        if (examIndex != -1)
        {
            printHUD(day, mainCharacter);
            std::cout << "[1] Яви се на изпит \n";

            getValidInput(1, 1);
            attemptExam(mainCharacter, examIndex);

            continue;
        }

        if (checkAndHandleFaint(mainCharacter, day)) continue;

        if (triggerDailyEvent(mainCharacter)) continue;

        // returns false if player chose to leave game and true if he chose anything else
        if (!handleDailyActions(mainCharacter, day)) return 0;

        if (checkGameOver(mainCharacter)) return 0;

        // character survived, we apply decay to knowledge to prevent sleep spamming
        applyNightlyDecay(mainCharacter);

        if (day == SEMESTER_LENGTH) semesterPassed = true;
    }

    if (semesterPassed)
    {
        system("cls");
        if (mainCharacter.passed_exams == NUMBER_OF_EXAMS)
        {
            std::cout << "╭────────────────────────────────────────╮ \n"
                << "│              ПОЗДРАВЛЕНИЯ!             │ \n"
                << "│       Печелиш, взе всички изпити       │ \n"
                << "╰────────────────────────────────────────╯ \n" << std::endl;
        }
        else
        {
            std::cout << "╭────────────────────────────────────────╮ \n"
                << "│                ЗАГУБА!                 │ \n"
                << "│   Невзети изпити по време на сесията   │ \n"
                << "╰────────────────────────────────────────╯ \n" << std::endl;
        }

        waitForKey();
    }
    
    return 0;
}