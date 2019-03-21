
#include <iostream>
#include <sstream>
#include <stdio.h>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include "bot_random.h"


int main(int argc,char** argv) {

    const char* token = std::getenv("MADRIDCCPPUG_BOT_TOKEN");
    if (!token) {
        std::cerr << "Provide env variable MADRIDCCPPUG_BOT_TOKEN\n";
        return 1;
    }

    namespace fs = boost::filesystem;
    fs::path full_path = fs::system_complete( fs::path( argv[0] ) );
    fs::path users_db = full_path.parent_path() / "users.txt";

    BotRandom bot(token, fs::canonical(users_db).string());

    try {
        bot.run();
    }
    catch (TgBot::TgException& e) {
        printf("error: %s\n", e.what());
    }

    return 0;
}
