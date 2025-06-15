#include "helpers.hpp"
#include <cctype>
#include <cstring>
#include <unordered_map>
#include "Context.hpp"

std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;

Vector2 getCenter(int width, int height) {
    Vector2 result;
    result.x = width/2.0;
    result.y = height/2.0;
    return result;
}

void drawMonospaceText(Font font, std::string text, Vector2 position, float fontSize, Color color, const bool rtl) {
    Vector2 sizeOfCharacter = MeasureTextEx(font, "a", fontSize, 1);

    if (rtl)
    {
        position.x += sizeOfCharacter.x * text.size();
    }

    for (int i = 0; i < text.size(); ++i)
    {
        unsigned char lb = text[i];
        char buf[5] = {0,0,0,0,0};

        int len = 0;

        if ((lb & 0x80) == 0) // lead bit is zero, must be a single ascii
            len = 1;
        else if ((lb & 0xE0) == 0xC0) // 110x xxxx
            len = 2;
        else if ((lb & 0xF0) == 0xE0) // 1110 xxxx
            len = 3;
        else if ((lb & 0xF8) == 0xF0) // 1111 0xxx
            len = 4;
        else
            continue;
        memcpy(buf, &text[i], len);

        DrawTextEx(font, buf, position, fontSize, 1, color);
        if (rtl)
        {
            position.x -= sizeOfCharacter.x;
        } else
        {
            position.x += sizeOfCharacter.x;
        }
        i += len - 1;
    }
}

bool textButton(Context& context, Vector2 positon, std::string text) {
    Vector2 sizeOfCharacter = MeasureTextEx(context.fonts.tinyFont.font, "a",
            context.fonts.tinyFont.size, 1);
    Theme theme = context.themes[context.selectedTheme];

    Rectangle rect = {
        positon.x,
        positon.y,
        sizeOfCharacter.x * text.size(),
        sizeOfCharacter.y
    };

    Color color = theme.text;

    if (CheckCollisionPointRec(GetMousePosition(), rect)) {
        context.mouseOnClickable  = true;
        color = theme.highlight;
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            return true;
        }
    }

    drawMonospaceText(context.fonts.tinyFont.font, text, positon, context.fonts.tinyFont.size, color);

    return false;
}

template<class BidiIter>
BidiIter random_unique(BidiIter begin, BidiIter end, size_t num_random) {
    size_t left = std::distance(begin, end);
    while (num_random--) {
        BidiIter r = begin;
        std::advance(r, rand()%left);
        std::swap(*begin, *r);
        ++begin;
        --left;
    }
    return begin;
}

bool useCaplitalNext = false;
bool previousWasDash = false;

char quotes[3][2] = {
    {'\'', '\''},
    {'\"', '\"'},
    {'(', ')'}
};

std::u32string generateSentence(Context &context, int numberOfWords) {
    auto words = context.wordsLists[context.selectedWordList].words;
    std::u32string output = U"";

    // Shuffle the amount of words we need
    random_unique(words.begin(), words.end(), numberOfWords);

    // Put the words in the senctence
    for(int i = 0; i < numberOfWords; ++i) {
        std::u32string word = words[i];

        bool inQuotes = context.testSettings.usePunctuation && (GetRandomValue(0, 10) == 10);
        bool itsDashTime = context.testSettings.usePunctuation && (GetRandomValue(0, 10) == 10) && !previousWasDash && !useCaplitalNext;
        bool itsNumber = context.testSettings.useNumbers && (GetRandomValue(0, 10) == 10);

        if (useCaplitalNext) {
            word[0] = toupper(word[0]);
        }

        if (inQuotes) {
            int quote = GetRandomValue(0, 2);
            output.push_back(quotes[quote][0]);
            word += quotes[quote][1];
        } else if(itsDashTime) {
            word = U"-";
        }

        if (itsNumber) {
            word = converter.from_bytes(std::to_string(GetRandomValue(0, 1000)));
        }

        output += word;

        useCaplitalNext = false;

        // Put . or , randomly
        if (GetRandomValue(0, 10) > 8 &&
                context.testSettings.usePunctuation &&
                !previousWasDash &&
                !itsDashTime &&
                !inQuotes) {
            switch (GetRandomValue(0, 3)) {
                case 0:
                    output.push_back(',');
                    break;
                case 1:
                    output.push_back('.');
                    useCaplitalNext = true;
                    break;
                case 2:
                    output.push_back('!');
                    useCaplitalNext = true;
                    break;
                case 3:
                    output.push_back('?');
                    useCaplitalNext = true;
                    break;
            }
        }

        previousWasDash = itsDashTime;

        output += U' ';
    }

    output.pop_back();

    return output;
}

void restartTest(Context &context, bool repeat) {
    if (!repeat) {
        int amount = context.testSettings.testModeAmounts[context.testSettings.selectedAmount];

        // If time mode is set put only 50 words after than it will be incremented as we type
        if (context.testSettings.testMode == TestMode::TIME) {
            amount = 120;
        }

        context.sentence = generateSentence(context, amount);

        if (context.testSettings.usePunctuation) {
            context.sentence[0] = toupper(context.sentence[0]);
            if (context.testSettings.testMode == TestMode::WORDS && !useCaplitalNext) {
                context.sentence += '.';
            }
        }
    }

    context.input = U"";
    context.currentScreen = Screen::TEST;
    context.wpm = 0;
    context.cpm = 0;
    context.accuracy = 0;
    context.raw = 0;
    context.testRunning = false;
    context.correctLetters = 0;
    context.incorrecLetters = 0;
    context.furthestVisitedIndex = -1;
}

void endTest(Context &context) {
    context.testRunning = false;
    context.currentScreen = Screen::RESULT;
    context.testEndTime = GetTime();
}

bool getFileContent(std::string fileName, std::vector<std::u32string> & vecOfStrs) {
    // Open the File
    std::ifstream in(fileName.c_str());

    // Check if object is valid
    if(!in) {
        std::cerr << "Cannot open the File : " << fileName << std::endl;
        return false;
    }

    std::string str;
    // Read the next line from File until it reaches the end.
    while (std::getline(in, str)) {
        // Line contains string of length > 0 then save it in vector
        if(!str.empty())
            vecOfStrs.push_back(converter.from_bytes(str));
    }

    //Close The File
    in.close();
    return true;
}

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
EM_JS(int, getStorageBrowser, (int position), {
  return localStorage.getItem(position);
});

EM_JS(void, setStorageBrowserDefault, (int position, int defaultValue), {
  if (localStorage.getItem(position) === null) {
    localStorage.setItem(position, defaultValue);
  }
});

EM_JS(void, setStorageBrowser, (unsigned int position, int value), {
  console.log("Setting value " + position + " " + value);
  localStorage.setItem(position, value);
});
#endif


// Save integer value to storage file (to defined position)
// NOTE: Storage positions is directly related to file memory layout (4 bytes each integer)
bool saveStorageValue(unsigned int position, int value) {
#if defined(PLATFORM_WEB)
    setStorageBrowser(position, value);
    return true;
#endif
    bool success = false;
    int dataSize = 0;
    unsigned int newDataSize = 0;
    const char *filePath = TextFormat("%s%s", GetApplicationDirectory(), STORAGE_DATA_FILE);
    unsigned char *fileData = LoadFileData(filePath, &dataSize);
    unsigned char *newFileData = NULL;

    if (fileData != NULL) {
        if (dataSize <= (position*sizeof(int))) {
            // Increase data size up to position and store value
            newDataSize = (position + 1)*sizeof(int);
            newFileData = (unsigned char *)RL_REALLOC(fileData, newDataSize);

            if (newFileData != NULL) {
                // RL_REALLOC succeded
                int *dataPtr = (int *)newFileData;
                dataPtr[position] = value;
            } else {
                // RL_REALLOC failed
                TraceLog(LOG_WARNING, "FILEIO: [%s] Failed to realloc data (%u), position in bytes (%u) bigger than actual file size", filePath, dataSize, position*sizeof(int));

                // We store the old size of the file
                newFileData = fileData;
                newDataSize = dataSize;
            }
        } else {
            // Store the old size of the file
            newFileData = fileData;
            newDataSize = dataSize;

            // Replace value on selected position
            int *dataPtr = (int *)newFileData;
            dataPtr[position] = value;
        }

        success = SaveFileData(filePath, newFileData, newDataSize);
        RL_FREE(newFileData);

        TraceLog(LOG_INFO, "FILEIO: [%s] Saved storage value: %i", filePath, value);
    } else {
        TraceLog(LOG_INFO, "FILEIO: [%s] File created successfully", filePath);

        dataSize = (position + 1)*sizeof(int);
        fileData = (unsigned char *)RL_MALLOC(dataSize);
        int *dataPtr = (int *)fileData;
        dataPtr[position] = value;

        success = SaveFileData(filePath, fileData, dataSize);
        UnloadFileData(fileData);

        TraceLog(LOG_INFO, "FILEIO: [%s] Saved storage value: %i", filePath, value);
    }

    return success;
}

// Load integer value from storage file (from defined position)
// NOTE: If requested position could not be found, value defaultValue is returned
int loadStorageValue(unsigned int position, int defaultValue) {
#if defined(PLATFORM_WEB)
    setStorageBrowserDefault(position, defaultValue);
    return getStorageBrowser(position);
#endif
    int value = defaultValue;
    int dataSize = 0;
    const char *filePath = TextFormat("%s%s", GetApplicationDirectory(), STORAGE_DATA_FILE);
    unsigned char *fileData = LoadFileData(filePath, &dataSize);

    if (fileData != NULL) {
        if (dataSize < (position*4)) {
            TraceLog(LOG_WARNING, "FILEIO: [%s] Failed to find storage position: %i", filePath, position);
        } else {
            int *dataPtr = (int *)fileData;
            value = dataPtr[position];
        }

        UnloadFileData(fileData);

        TraceLog(LOG_INFO, "FILEIO: [%s] Loaded storage value: %i", filePath, value);
    }

    return value;
}
