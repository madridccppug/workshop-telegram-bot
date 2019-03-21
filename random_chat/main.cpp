
#include <iostream>
#include <sstream>
#include <stdio.h>

#include "bot_random.h"


int main() {

    const char* token = std::getenv("MADRIDCCPPUG_BOT_TOKEN");
    if (!token) {
        std::cerr << "Provide env variable MADRIDCCPPUG_BOT_TOKEN\n";
        return 1;
    }

    BotRandom bot(token, "users.txt");

    try {
        bot.run();
    }
    catch (TgBot::TgException& e) {
        printf("error: %s\n", e.what());
    }

    return 0;
}
