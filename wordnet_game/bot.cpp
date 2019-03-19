
#include "bot.h"

#include <iostream>
#include "wordnet.h"


// Unicode for emojis: https://apps.timwhitlock.info/emoji/tables/unicode

namespace emoji {
    const char* const green_check = "\xE2\x9C\x85";
    const char* const red_cross = "\xE2\x9D\x8C";
    const char* const book = "\xF0\x9F\x93\x96";
};

namespace {
    TgBot::ReplyKeyboardMarkup::Ptr yes_no_keyb() {
        TgBot::ReplyKeyboardMarkup::Ptr keyboard(new TgBot::ReplyKeyboardMarkup);
        keyboard->oneTimeKeyboard = true;

        std::vector<TgBot::KeyboardButton::Ptr> row;
        TgBot::KeyboardButton::Ptr yes(new TgBot::KeyboardButton);
        yes->text = "yes";
        row.push_back(yes);
        TgBot::KeyboardButton::Ptr no(new TgBot::KeyboardButton);
        no->text = "no";
        row.push_back(no);

        keyboard->keyboard.push_back(row);
        return keyboard;
    }
}


Bot::Bot(const char* token, const wnb::wordnet& wn) : _bot(token), _wn(wn) {
    std::cout << "Bot username: " << _bot.getApi().getMe()->username.c_str() << "\n";
    _bot.getApi().deleteWebhook();
}

void Bot::run()
{
    this->initialize();

    TgBot::TgLongPoll longPoll(_bot);
    while (true) {
        // std::cout << "Long poll started\n";
        longPoll.start();
    }
}

void Bot::initialize() {
    _bot.getEvents().onCommand("start", [this](TgBot::Message::Ptr message){ this->start(message);});
    _bot.getEvents().onCommand("stop", [this](TgBot::Message::Ptr message){ this->stop(message);});
    _bot.getEvents().onCommand("play", [this](TgBot::Message::Ptr message){ this->play(message);});
    _bot.getEvents().onCommand("help", [this](TgBot::Message::Ptr message){ this->help(message);});

    _bot.getEvents().onAnyMessage([this](TgBot::Message::Ptr message){ this->on_message(message);});
}

void Bot::help(TgBot::Message::Ptr message) {
    std::string desc = "This is a little example of a Telegram bot implemented using C++. It uses " \
                       "WordNet to create a language game. At any moment you can use the command  " \
                       "/play to start the game or /stop to finish it. Use /help to print this "    \
                       "message";
    _bot.getApi().sendMessage(message->chat->id, desc);
}

void Bot::start(TgBot::Message::Ptr message) {
    std::cout << "Bot::start: " << message->from->username << std::endl;
    std::stringstream ss; ss << "Hi " << message->from->username << "!";
    _bot.getApi().sendMessage(message->chat->id, ss.str());

    this->help(message);

    std::string q = "... but right now, are your ready to play?";
    auto yes_no = yes_no_keyb();
    auto msg = _bot.getApi().sendMessage(message->chat->id, q, false, 0, yes_no);

    bool inserted; std::map<int32_t, _t_actions>::iterator it;
    std::tie(it, inserted) = _callbacks.insert(std::make_pair(msg->chat->id, _t_actions()));

    it->second["yes"] = [this](TgBot::Message::Ptr message){this->play(message);};
    it->second["no"] = [this](TgBot::Message::Ptr message){this->stop(message);};
}

void Bot::stop(TgBot::Message::Ptr message) {
    std::cout << "Bot::stop: " << message->from->username << std::endl;
    _bot.getApi().sendMessage(message->chat->id, "Ok. See you soon... Use /play to start a new game.",
                              false, 0, std::make_shared<TgBot::ReplyKeyboardRemove>());
}

void Bot::play(TgBot::Message::Ptr message) {
    std::cout << "Bot::play: " << message->from->username << std::endl;

    // Look for a word/definition

    TgBot::ReplyKeyboardMarkup::Ptr keyboard(new TgBot::ReplyKeyboardMarkup);
    keyboard->oneTimeKeyboard = true;

    std::map<std::string, std::string> q;
    auto chosen = random_q(_wn, q, 4);

    bool inserted; std::map<int32_t, _t_actions>::iterator it;
    std::tie(it, inserted) = _callbacks.insert(std::make_pair(message->chat->id, _t_actions()));

    for (auto& option: q) {
        if (option.first == chosen) {
            it->second[option.first] = [this](TgBot::Message::Ptr msg) {
                std::stringstream ss; ss << "Great! +1 point! " << emoji::green_check;
                _bot.getApi().sendMessage(msg->chat->id, ss.str());
                this->play(msg);
            };
        }
        else {
            it->second[option.first] = [this, q, chosen](TgBot::Message::Ptr msg) {
                std::stringstream ss; ss << emoji::red_cross << " Nooo... *" << msg->text << "* means " << emoji::book << " _";
                    ss << q.at(msg->text) << "_. We were looking for *" << chosen << "*.";
                _bot.getApi().sendMessage(msg->chat->id, ss.str(), false, 0, std::make_shared< TgBot::GenericReply >(), "Markdown");
                this->play(msg);
            };
        }
        std::vector<TgBot::KeyboardButton::Ptr> row;
        TgBot::KeyboardButton::Ptr button(new TgBot::KeyboardButton);
        button->text = option.first;
        row.push_back(button);
        keyboard->keyboard.push_back(row);
    }

    std::stringstream ss; ss << "Which word matches this definition " << emoji::book << " _" << q[chosen] << "_";
    auto msg = _bot.getApi().sendMessage(message->chat->id, ss.str(), false, 0, keyboard, "Markdown");
}

void Bot::on_message(TgBot::Message::Ptr message) {
    if (StringTools::startsWith(message->text, "/")) {
        return;
    }
    std::cout << "Bot::play: " << message->from->username << ": " << message->text << std::endl;

    auto it = _callbacks.find(message->chat->id);
    if (it != _callbacks.end()) {
        std::function<void (TgBot::Message::Ptr)> next;
        auto response = it->second.find(message->text);
        if (response != it->second.end()) {
            next = response->second;
        }
        else {
            next = [this](TgBot::Message::Ptr message){
                _bot.getApi().sendMessage(message->chat->id, "Unexpected input, start game again using /play",
                                          false, 0, std::make_shared<TgBot::ReplyKeyboardRemove>());
            };
        }
        _callbacks.erase(it);
        next(message);
        return;
    }
    std::string desc = "We aren't playing right now. Use command /play to start a game or /help " \
                       "if you want more information.";
    _bot.getApi().sendMessage(message->chat->id, desc);
}
