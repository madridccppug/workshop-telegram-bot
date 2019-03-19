
#include <iostream>
#include <sstream>
#include <stdio.h>

#include <wnb/core/wordnet.hh>

#include "bot.h"


int main() {

    const char* token = std::getenv("MADRIDCCPPUG_BOT_TOKEN");
    const char* wnet_data = std::getenv("WORDNET_DATA_PATH");
    if (!token or !wnet_data) {
        std::cerr << "Provide env variables MADRIDCCPPUG_BOT_TOKEN and WORDNET_DATA_PATH (path to dict folder)\n";
        return 1;
    }

    wnb::wordnet wn(wnet_data);
    Bot bot(token, wn);

    try {
        bot.run();
    }
    catch (TgBot::TgException& e) {
        printf("error: %s\n", e.what());
    }

    return 0;
}
