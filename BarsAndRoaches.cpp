#include <fstream>
#include <iostream>
#include <windows.h> // needed for console encoding switch

struct student
{
    double money;
    int energy;
    int psyche;
    int knowledge;
    int physical;
    int passed_exams;
};

const int INPUT_LINE_MAX_SIZE = 100;
const int SEMESTER_LENGTH = 45;
const char BASE_FILE_NAME[] = "SaveSlots.txt";
const int STAT_BAR_MAX_SIZE = 64;

const double MAX_PLAYER_MONEY = 99999;
const int MAX_PLAYER_ENERGY = 100;
const int MAX_PLAYER_PSYCHE = 100;
const int MAX_PLAYER_KNOWLEDGE = 100;
const int MAX_PLAYER_PHYSICAL = 100;
const int MAX_PLAYER_PASSED_EXAMS = 5;

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

void calculateStatBar(const int statValue, char* statBar, const int totalBlocks)
{
    int filled = statValue / 10; 

    const char fullBlock[] = "▓";
    const char emptyBlock[] = "░";
    int offset = 0;
    
    for (int i = 0; i < filled; i++) 
    {
        statBar[offset] = fullBlock[0];
        statBar[offset + 1] = fullBlock[1];
        statBar[offset + 2] = fullBlock[2];

        offset += 3;
    }

    for (int i = filled; i < totalBlocks; i++) 
    {
        statBar[offset] = emptyBlock[0];
        statBar[offset + 1] = emptyBlock[1];
        statBar[offset + 2] = emptyBlock[2];
        
        offset += 3;
    }
    
    // we have to do it like this, because ▓ and ░ are 3 bytes and cannot be added by statBar[i] = '▓';
    
    statBar[offset] = '\0';
}

void printStatRow(const char* label, int current, int max, int totalBlocks)
{
    char statBarBuffer[STAT_BAR_MAX_SIZE]; 
    
    calculateStatBar(current, statBarBuffer, totalBlocks);
    
    std::cout << "      » " << label << statBarBuffer << " (" << current << '/' << max << ')' << "\n";
}

int main(int argc, char* argv[])
{
    SetConsoleOutputCP(CP_UTF8); // switch to utf-8 encoding, so the console recognizes the ascii symbols
    
    // alternative title: Cock(tails + roaches)

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
                  << "│       [1] Софтуерно инженерство {ЛЕСНО}       │ \n"
                  << "│       [2] Компютърни науки {СРЕДНО}           │ \n"
                  << "│       [3] Информатика {ТРУДНО}                │ \n"
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
        mainCharacter.money = 1000;
        mainCharacter.energy = difficultyLevel * 10 + 60;
        mainCharacter.psyche = difficultyLevel * 10 + 60;
        mainCharacter.physical = difficultyLevel * 10 + 60;
        mainCharacter.knowledge = difficultyLevel * 10 + 60;
        mainCharacter.passed_exams = 0;
    } // ! informatika with the highest starting stats, cuz easiest to get in, so the least burnout
    else
    {
        //set mainCharacter stats to ones read from the saveFile
        
        /*
        mainCharacter.money = 1000;
        mainCharacter.energy = difficultyLevel * 10 + 60;
        mainCharacter.psyche = difficultyLevel * 10 + 60;
        mainCharacter.physical = difficultyLevel * 10 + 60;
        mainCharacter.knowledge = difficultyLevel * 10 + 60;
        mainCharacter.passed_exams = 0;
        */
    }
    
    for (int day = 1; day <= SEMESTER_LENGTH; day++) // day should start from day rEad from file
    {
        double playerMoney = mainCharacter.money;
        
            // ! get statBars of every stat and concat them to the output:
        int playerEnergy = mainCharacter.energy;
        int playerPsyche = mainCharacter.psyche;
        int playerPhysical = mainCharacter.physical;
        int playerKnowledge = mainCharacter.knowledge;
        int playerPassedExams = mainCharacter.passed_exams;
        
        std::cout << "╭───────────────────────────╮\n"
                  << "     Ден " << day << " от " <<  SEMESTER_LENGTH << "\n"
                  << " ─────────────────────────── \n"
                  << "     » Пари: " << playerMoney << "\n" << std::endl;
                printStatRow("Енергия: ", playerEnergy, MAX_PLAYER_ENERGY, 10);
                printStatRow("Психика: ", playerPsyche, MAX_PLAYER_PSYCHE, 10);
                printStatRow("Здраве:  ", playerPhysical, MAX_PLAYER_PHYSICAL, 10);
                printStatRow("Знания: ", playerKnowledge, MAX_PLAYER_KNOWLEDGE, 10);
                printStatRow("Взети изпити: ", playerPassedExams, MAX_PLAYER_PASSED_EXAMS, 5);
        std::cout << "╰───────────────────────────╯" << std::endl;
        
        std::cout << "Какво искаш да направиш днес? {Ден: " << day << "}" << '\n'
            << "[1] Учиш \n"
            << "[2] Храниш се \n"
            << "[3] Излизаш \n"
            << "[4] Почиваш \n"
            << "[5] Работиш \n"
            << "[6] Явяваш се на изпит (ако е време) \n" // only show if its exam day
            << "[11] Излез от играта" << std::endl;

        // more actions, make some dynamic way to output them
        
        std::cin >> commandLine;

        if (commandLine == 1)
        {
            // u study
        }
            
        else if (commandLine == 2)
        {
            // u eat
        } 
        
        // more cases for each action

        else if (commandLine == 11)
        {
            break;
        }
        else
        {
            std::cout << "Невалидно действие, опитай отново! \n";
            day--;
        }
    }
}