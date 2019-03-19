
#pragma once

#include <tgbot/tgbot.h>
#include <wnb/core/wordnet.hh>


class Bot {
    public:
        Bot(const char* token, const wnb::wordnet&);

        void run();

        void start(TgBot::Message::Ptr message);
        void stop(TgBot::Message::Ptr message);
        void play(TgBot::Message::Ptr message);
        void help(TgBot::Message::Ptr message);

        //void query(TgBot::CallbackQuery::Ptr query);
        void on_message(TgBot::Message::Ptr message);
    protected:
        void initialize();

    protected:
        TgBot::Bot _bot;
        const wnb::wordnet& _wn;

        typedef std::map<std::string, std::function<void (TgBot::Message::Ptr)>> _t_actions;
        std::map<int32_t, _t_actions> _callbacks;
};
