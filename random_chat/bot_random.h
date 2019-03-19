
#pragma once

#include <boost/bimap.hpp>
#include <tgbot/tgbot.h>


class BotRandom {
public:
    enum class Status {AVAILABLE, MATCHED, UNAVAILABLE};
public:
    BotRandom(const char* token);
    
    void run();
    
    void start(TgBot::Message::Ptr message);
    void stop(TgBot::Message::Ptr message, bool direct_message);
    void help(TgBot::Message::Ptr message);
    void report(TgBot::Message::Ptr message);
    
    void on_message(TgBot::Message::Ptr message);
protected:
    void initialize();
    void bot_says(int32_t, const std::string&);
    
protected:
    TgBot::Bot _bot;
    std::map<int32_t, std::string> _users; // Users by chat id
    
    boost::bimap<int32_t, int32_t> _matches;  // Active pairs
    std::map<int32_t, Status> _status;
    
};
