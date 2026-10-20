#include <iostream>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <cstddef>

constexpr std::size_t MAX_WORD_LENGTH = 12;
constexpr std::size_t BUFFER_CAPACITY = MAX_WORD_LENGTH + 1;
constexpr int INITIAL_ATTEMPTS = 3;
constexpr int INITIAL_HINTS = 2;

bool encryptWord(const char* original, char* encrypted, std::size_t capacity, int key) {
    if (capacity == 0) return false;

    std::size_t length = std::strlen(original);

    if (length + 1 > capacity) {
        encrypted[0] = '\0';
        return false;
    }

    const char* p = original;
    char* q = encrypted;

    while (*p != '\0') {
        *q = static_cast<char>((*p - 'a' + key) % 26 + 'a');
        ++p;
        ++q;
    }

    *q = '\0';
    return true;
}


bool revealOneLetter(const char* original, char* revealed, std::size_t capacity) {
    const char* p = original;
    char* r = revealed;
    std::size_t length = 0;

    while (*p != '\0') {
        ++p;
        ++length;
    }

    if (capacity < length + 1) {
        return false;
    }

    p = original;
    r = revealed;

    while (*p != '\0') {
        if (*r == '_') {
            *r = *p;
            return true;
        }

        ++p;
        ++r;
    }

    return false;
}

bool checkAnswer(const char* userGuess, const char* original,
                 int& attempts) {
    if (std::strcmp(userGuess, original) == 0) {
        return true;
    }

    --attempts;
    return false;
}

enum class ReadStatus { Ok, TooLong, End, Error };

ReadStatus readLine(char* buffer, std::size_t capacity) {
    std::size_t size = 0;
    bool tooLong = false;
    char ch = '\0';

    while (std::cin.get(ch)) {
        if (ch == '\n') {
            buffer[size] = '\0';
            return tooLong ? ReadStatus::TooLong : ReadStatus::Ok;
        }

        if (size < capacity - 1) {
            buffer[size++] = ch;
        } else {
            tooLong = true;
        }
    }

    buffer[size] = '\0';

    if (std::cin.bad()) return ReadStatus::Error;
    if (size == 0 && !tooLong) return ReadStatus::End;

    return tooLong ? ReadStatus::TooLong : ReadStatus::Ok;
}

bool isLowercaseWord(const char* text) {
    if (*text == '\0') return false;

    for (const char* p = text; *p != '\0'; ++p) {
        if (*p < 'a' || *p > 'z') return false;
    }

    return true;
}

int main() {
    const char original[] = "pointer";
    char encrypted[BUFFER_CAPACITY];
    char revealed[BUFFER_CAPACITY];
    char userGuess[BUFFER_CAPACITY];

    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    int key = std::rand() % 5 + 1;

    if (!encryptWord(original, encrypted, BUFFER_CAPACITY, key)) {
        std::cout << "Error: could not encrypt the word.\n";
        return 1;
    }

    std::size_t length = std::strlen(original);

    for (std::size_t i = 0; i < length; ++i) {
        revealed[i] = '_';
    }
    revealed[length] = '\0';

    int attempts = INITIAL_ATTEMPTS;
    int hints = INITIAL_HINTS;
    bool won = false;

    std::cout << "=== WORD DECIPHER ===\n";
    std::cout << "Decode this word: " << encrypted << '\n';
    std::cout << "You have " << attempts << " attempts and " << hints << " hints.\n";
    std::cout << "Type 'hint' to reveal a letter.\n\n";

    while (attempts > 0 && !won) {
        std::cout << "Your answer: ";

        ReadStatus status = readLine(userGuess, BUFFER_CAPACITY);

        if (status == ReadStatus::End) {
            std::cout << "\nInput ended. Game over.\n";
            return 0;
        }

        if (status == ReadStatus::Error) {
            std::cout << "\nInput error.\n";
            return 1;
        }

        if (status == ReadStatus::TooLong) {
            std::cout << "Input is too long. Try again.\n\n";
            continue;
        }

        if (std::strcmp(userGuess, "hint") == 0) {
            if (hints == 0) {
                std::cout << "No hints remaining!\n\n";
                continue;
            }

            if (revealOneLetter(original, revealed, BUFFER_CAPACITY)) {
                --hints;
                std::cout << "Hint: " << revealed << '\n';
                std::cout << "Hints remaining: " << hints << "\n\n";
            } else {
                std::cout << "All letters have already been revealed!\n\n";
            }

            continue;
        }

        if (!isLowercaseWord(userGuess)) {
            std::cout << "Enter a lowercase English word.\n\n";
            continue;
        }

        won = checkAnswer(userGuess, original, attempts);

        if (won) {
            std::cout << "Congratulations! You deciphered the word: "
                      << original << '\n';
        } else {
            std::cout << "Incorrect answer! Attempts remaining: "
                      << attempts << "\n\n";
        }
    }

    if (!won) {
        std::cout << "Game over! The correct word was: "
                  << original << '\n';
    }

    return 0;
}