#include <algorithm>
#include "Context.hpp"
#include "helpers.hpp"
#include <iostream>
#include "../libs/raylib/src/raylib.h"

void Context::load()
{
    Theme arch;
    arch.name = u8"Arch";
    arch.background = {6, 7, 9, 255};
    arch.text = {92, 96, 133, 255};
    arch.cursor = {214, 227, 255, 255};
    arch.wrong = RED;
    arch.correct = arch.cursor;
    arch.highlight = arch.correct;
    this->themes.push_back(arch);

    Theme black;
    black.name = u8"Black";
    black.background = {17, 17, 17, 255};
    black.text = {96, 96, 96, 255};
    black.cursor = {225, 225, 225, 255};
    black.wrong = {218, 51, 51, 255};
    black.correct = black.cursor;
    black.highlight = black.cursor;
    this->themes.push_back(black);

    Theme white;
    white.name = u8"White";
    white.background = {238, 235, 226, 255};
    white.text = {153, 148, 127, 255};
    white.cursor = WHITE;
    white.wrong = {209, 97, 67, 255};
    white.correct = black.background;
    white.highlight = black.background;
    this->themes.push_back(white);

    Theme espresso;
    espresso.name = u8"Espresso";
    espresso.background = {23, 18, 18, 255};
    espresso.text = {202, 176, 155, 255};
    espresso.cursor = {255, 185, 56, 255};
    espresso.wrong = {250, 31, 15, 255};
    espresso.correct = espresso.cursor;
    espresso.highlight = espresso.cursor;
    this->themes.push_back(espresso);

    Theme catppuccin;
    catppuccin.name = u8"Catppuccin";
    catppuccin.background = {27, 25, 35, 255};
    catppuccin.text = {198, 160, 246, 255};
    catppuccin.cursor = {166, 218, 149, 255};
    catppuccin.wrong = {227, 140, 143, 255};
    catppuccin.correct = {145, 215, 227, 255};
    catppuccin.highlight = catppuccin.cursor;
    this->themes.push_back(catppuccin);

    Theme cyberpunk;
    cyberpunk.name = u8"Cyberpunk";
    cyberpunk.background = {13, 13, 13, 255};
    cyberpunk.text = {84, 84, 84, 255};
    cyberpunk.cursor = {208, 237, 87, 255};
    cyberpunk.wrong = {248, 82, 74, 255};
    cyberpunk.correct = {26, 214, 118, 255};
    cyberpunk.highlight = WHITE;
    this->themes.push_back(cyberpunk);

    Theme material;
    material.name = u8"Material";
    material.background = {33, 33, 33, 255};
    material.text = {95, 99, 99, 255};
    material.cursor = {199, 146, 234, 255};
    material.wrong = {247, 140, 108, 255};
    material.correct = {195, 232, 141, 255};
    material.highlight = {127, 204, 196, 255};
    this->themes.push_back(material);

    std::string base = GetApplicationDirectory();

    int codepoint_count = 0x7F + 0x6F;
    int codepoints[codepoint_count] = {0};
    for (int i = 0; i < 0x7F; i++) codepoints[i] = i; // Basic Latin block
    for (int i = 0; i < 0x6F; i++) codepoints[0x7F + i] = 0x590 + i; // Hebrew block

    // Load fonts
    this->fonts.typingTestFont.size = 32;
    this->fonts.typingTestFont.font = LoadFontEx((base + "assets/fonts/Cousine-Regular.ttf").c_str(),
                                                 this->fonts.typingTestFont.size, codepoints, codepoint_count);
    this->fonts.titleFont.size = 40;
    this->fonts.titleFont.font = LoadFontEx((base + "assets/fonts/NotoSansHebrew-Regular.ttf").c_str(),
                                            this->fonts.titleFont.size, codepoints, codepoint_count);
    this->fonts.tinyFont.size = 18;
    this->fonts.tinyFont.font = LoadFontEx((base + "assets/fonts/Cousine-Regular.ttf").c_str(),
                                           this->fonts.tinyFont.size, codepoints, codepoint_count);
    this->fonts.bigFont.size = 90;
    this->fonts.bigFont.font = LoadFontEx((base + "assets/fonts/Cousine-Regular.ttf").c_str(),
                                          this->fonts.bigFont.size, codepoints, codepoint_count);

    int symbol_codepoint_count = 2;
    int symbol_codepoints[symbol_codepoint_count] = {0x2B91, 0x2423}; // Enter and Space

    this->fonts.symbolsFont = LoadFontEx((base + "assets/fonts/NotoSansSymbols-Regular.ttf").c_str(),
                                          this->fonts.bigFont.size, symbol_codepoints, symbol_codepoint_count);


    // Load word lists
    FilePathList filesList = LoadDirectoryFiles((base + "assets/word_lists/").c_str());
    char** files = filesList.paths;
    int numberOfFiles = filesList.count;
    std::sort(files, files + numberOfFiles);

    for (int i = 0; i < numberOfFiles; i++)
    {
        if (files[i][0] != '.')
        {
            WordList wordList;
            std::string name = files[i];
            name.replace(name.find(u8".txt"), sizeof(u8".txt") - 1, u8"");
            name.replace(name.find('_'), sizeof('_') - 1, u8" ");
            wordList.name = name;
            getFileContent(files[i], wordList.words);
            this->wordsLists.push_back(wordList);
        }
    }

    this->selectedTheme = loadStorageValue(0, 0);
    this->selectedWordList = loadStorageValue(1, 0);

    // Load sounds
    this->sounds.clickSound1 = LoadSound((base + "assets/audio/otemu_browns.wav").c_str());

    this->testSettings.selectedAmount = loadStorageValue(2, 1);
    this->testSettings.testMode = (TestMode)loadStorageValue(5, 0);
    this->cursorStyle = (CursorStyle)loadStorageValue(6, 0);

    this->soundOn = loadStorageValue(7, 1);

#if defined(PLATFORM_WEB)
    this->soundOn = 0; // On web sound won't start playing until the user clicks on the page
#endif
}

void Context::saveSettings()
{
    std::cout << "Settings saved" << std::endl;
    saveStorageValue(0, this->selectedTheme);
    saveStorageValue(1, this->selectedWordList);
    saveStorageValue(2, this->testSettings.selectedAmount);
    saveStorageValue(5, (int)this->testSettings.testMode);
    saveStorageValue(6, (int)this->cursorStyle);
    saveStorageValue(7, this->soundOn);
}

void Context::unload()
{
    UnloadFont(this->fonts.typingTestFont.font);
    UnloadFont(this->fonts.tinyFont.font);
    UnloadFont(this->fonts.bigFont.font);
    this->saveSettings();
}

