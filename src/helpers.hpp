#ifndef HELPERS_HPP
#define HELPERS_HPP
#include "Context.hpp"
#include "../libs/raylib/src/raylib.h"
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <locale>
#include <codecvt>

#define STORAGE_DATA_FILE "storage.data"

typedef enum
{
    STORAGE_POSITION_SCORE = 0,
    STORAGE_POSITION_HISCORE = 1
} StorageData;

// Persistent storage functions
bool saveStorageValue(unsigned int position, int value);
int loadStorageValue(unsigned int position, int defaultValue);


Vector2 getCenter(int width, int height);

void drawMonospaceText(Font font, std::string text, Vector2 position, float fontSize, Color color, bool rtl = false);

bool textButton(Context& context, Vector2 positon, std::string text);

double getTimeInMin();

void restartTest(Context& context, bool repeat);

void endTest(Context& context);

bool getFileContent(std::string fileName, std::u32string& vecOfStrs);

std::u32string getRandomWord(const Context& context);

std::u32string getWords(Context& context);

extern std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;

#endif
