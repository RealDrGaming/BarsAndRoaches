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

void calculateStatBar(const int statValue, char* statBar)
{
    const int TOTAL_BLOCKS = 10;

    int filled = statValue / 10; 

    for (int i = 0; i < filled; i++) 
    {
        statBar[i] = '▓';
    }

    for (int i = filled; i < (TOTAL_BLOCKS - filled); i++) 
    {
        statBar[i] = '░';
    }
}

int main(int argc, char* argv[])
{
    SetConsoleOutputCP(CP_UTF8); // switch to utf-8 encoding, so the console recognizes the ascii symbols
    
    // alternative title: Cock(tails + roaches)

    int commandLine = 0;
    
        // save-file load system
    std::cout << "╭──────────────────────────╮ \n"
              << "│     Bars and Roaches     │ \n"
              << "│       [1] Нов файл       │ \n"
              << "│       [2] Продължи       │ \n"
              << "╰──────────────────────────╯" << std::endl;

    do
    {
        std::cin >> commandLine;

        char saveFileName[INPUT_LINE_MAX_SIZE];
        
        if (commandLine == 1)
        {
            std::cout << "Как искаш да кръстиш новия записан файл?" << '\n';
            std::cin >> saveFileName;

            writeInFile(BASE_FILE_NAME, saveFileName);
            
            break;
        }
        else if (commandLine == 2)
        {
            std::cout << "От кой записан файл искаш да продължиш?" << '\n';

            if (!readFile(BASE_FILE_NAME))
                continue;
            
            // make a way to choose from printed files
            break;
        }
        else
        {
            std::cout << "Невалидна команда, опитай отново!" << '\n';
        }
    }
    while (true);

    int difficultyLevel = 0;
    
    // difficulty choosing
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
    
    bool isNewGame = commandLine % 2; // true or false depending on if saveFile is new
    
    student mainCharacter;
    
    if (isNewGame)
    {
        mainCharacter.money = 1000;
        mainCharacter.energy = difficultyLevel * 10 + 60;
        mainCharacter.psyche = difficultyLevel * 10 + 60;
        mainCharacter.physical = difficultyLevel * 10 + 60;
        mainCharacter.knowledge = difficultyLevel * 10 + 60;
        mainCharacter.passed_exams = 0;
    } // informatika with the highest starting stats, cuz easiest to get in, so the least burnout

    for (int day = 1; day <= SEMESTER_LENGTH; day++)
    {
        std::cout << "╭───────────────────────────╮\n"
                  << "     Ден " << day << " от " <<  SEMESTER_LENGTH << "\n"
                  << " ─────────────────────────── \n"
                  << "     » Пари: " << 999999 << "\n"
                  << "     » Енергия: " << 100 << "\n"
                  << "     » Психика: " << 100 << "\n"
                  << "     » Здраве:  " << 100 << "\n"
                  << "     » Знания:  " << 100 << "\n"
                  << "     » Взети изпити:  " << 9 << "\n"
                  << "╰───────────────────────────╯" << std::endl;
        
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
            std::cout << "Невалидно действие, опитай отново!";
            day--;
        }
    }
}