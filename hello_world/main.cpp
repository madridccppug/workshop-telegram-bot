
#include <iostream>
#include <sstream>
#include <stdio.h>

#include <tgbot/tgbot.h>


int main(int argc,char** argv) {

    const char* token = std::getenv("MADRIDCCPPUG_BOT_TOKEN");
    if (!token) {
        std::cerr << "Provide env variable MADRIDCCPPUG_BOT_TOKEN\n";
        return 1;
    }

    // Initialize the bot
    TgBot::Bot bot("PLACE YOUR TOKEN HERE");

    // Connect to events
    bot.getEvents().onCommand("start", [&bot](TgBot::Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "Hi!");
    });
    bot.getEvents().onAnyMessage([&bot](TgBot::Message::Ptr message) {
        printf("User wrote %s\n", message->text.c_str());
        if (StringTools::startsWith(message->text, "/start")) {
            return;
        }
        bot.getApi().sendMessage(message->chat->id, "Your message is: " + message->text);
    });

    // Run the infinite loop
    try {
        printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());
        TgBot::TgLongPoll longPoll(bot);
        while (true) {
            printf("Long poll started\n");
            longPoll.start();
        }
    } catch (TgBot::TgException& e) {
        printf("error: %s\n", e.what());
    }
    return 0;
}
