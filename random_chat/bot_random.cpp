
#include "bot_random.h"

#include <random>
#include <iterator>
#include <chrono>

#include "fmt/format.h"
#include "fmt/ostream.h"


// Unicode for emojis: https://apps.timwhitlock.info/emoji/tables/unicode

namespace emoji {
    const char* const robot = "🤖";
};

namespace {
    template<typename Iter, typename RandomGenerator>
    Iter select_randomly(Iter start, Iter end, RandomGenerator& g) {
        std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
        std::advance(start, dis(g));
        return start;
    }
    
    template<typename Iter>
    Iter select_randomly(Iter start, Iter end) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        return select_randomly(start, end, gen);
    }
}


BotRandom::BotRandom(const char* token) : _bot(token) {
    fmt::print("Bot username: @{}\n", _bot.getApi().getMe()->username.c_str());
    _bot.getApi().deleteWebhook();
}

void BotRandom::initialize() {
    _bot.getEvents().onCommand("start", [this](TgBot::Message::Ptr message){ this->start(message);});
    _bot.getEvents().onCommand("stop", [this](TgBot::Message::Ptr message){ this->stop(message, true);});
    _bot.getEvents().onCommand("help", [this](TgBot::Message::Ptr message){ this->help(message);});
    _bot.getEvents().onCommand("report", [this](TgBot::Message::Ptr message){ this->report(message);});
    
    _bot.getEvents().onAnyMessage([this](TgBot::Message::Ptr message){ this->on_message(message);});
}

void BotRandom::run() {
    this->initialize();
    
    TgBot::TgLongPoll longPoll(_bot);
    while (true) {
        //fmt::print("Long poll started\n");
        longPoll.start();
    }
}

void BotRandom::bot_says(int32_t chat, const std::string& message) {
    _bot.getApi().sendMessage(chat, fmt::format("{} >> {}", emoji::robot, message), false, 0,
                              std::make_shared<TgBot::ReplyKeyboardRemove>(), "Markdown");
}

void BotRandom::help(TgBot::Message::Ptr message) {
    std::string desc = "This bot selects a random user to talk with. Use command /start to link "\
                       "to a new user or to become available for the next one requesting a partner. "\
                       "Command /stop to unlink (you will become unavailable). Command /report "\
                       "will unlink you from the current conversation and send a warning to "\
                       "moderators. "\
                       "Command /help will show this message.";
    this->bot_says(message->chat->id, desc);
}

void BotRandom::start(TgBot::Message::Ptr message) {
    auto username = message->from->username;
    if (username.empty()) {
        username = message->from->firstName + " " + message->from->lastName;
    }
    fmt::print("{}: /start\n", username);
    
    // Store username
    _users[message->chat->id] = username;
    
    // Unlink from previous conversation (this user will become unavailable)
    this->stop(message, false);
    
    // Search for a new match
    std::vector<int32_t> _availables;
    for (const auto& item: _status) {
        if (item.second == Status::AVAILABLE) {
            _availables.push_back(item.first);
        }
    }
    if (!_availables.empty()) {
        auto it = select_randomly(_availables.begin(), _availables.end());
        assert(it != _availables.end());
        _matches.left.insert(std::make_pair(message->chat->id, *it));
        _status[message->chat->id] = Status::MATCHED;
        _status[*it] = Status::MATCHED;
        this->bot_says(message->chat->id, "You have been matched with " + _users[*it] + ". Say hi!");
        this->bot_says(*it, _users[message->chat->id] + " has been matched with you!");
    }
    else {
        this->bot_says(message->chat->id, "There aren't users available, we are waiting for one");
        _status[message->chat->id] = Status::AVAILABLE;
    }
}

void BotRandom::stop(TgBot::Message::Ptr message, bool direct_command) {
    if (direct_command) {
        fmt::print("{}: /stop\n", _users[message->chat->id]);
    }
    
    auto it_left = _matches.left.find(message->chat->id);
    auto it_right = _matches.right.find(message->chat->id);
    
    if (it_left != _matches.left.end() or it_right != _matches.right.end()) {
        int32_t other;
        
        if (it_left != _matches.left.end()) {
            assert(it_right == _matches.right.end());
            other = it_left->second;
        }
        
        if (it_right != _matches.right.end()) {
            assert(it_left == _matches.left.end());
            other = it_right->second;
        }
        
        this->bot_says(message->chat->id, "You left conversation with " + _users[other]);
        this->bot_says(other, _users[message->chat->id] + " left this conversation");
        _status[other] = Status::AVAILABLE;
    }
    else if (direct_command) {
        this->bot_says(message->chat->id, "You were already unavailable");
    }
    _status[message->chat->id] = BotRandom::Status::UNAVAILABLE;
}

void BotRandom::report(TgBot::Message::Ptr message) {
    fmt::print("{}: /report\n", _users[message->chat->id]);
    
    this->bot_says(message->chat->id, "Report is not implemented yet, but you will be disconnected from the user you are talking to");
    this->stop(message, false);
}

void BotRandom::on_message(TgBot::Message::Ptr message) {
    if (StringTools::startsWith(message->text, "/")) {
        return;
    }
    
    try {
        // If the user is matched, the message must be interchanged.
        auto it_left = _matches.left.find(message->chat->id);
        auto it_right = _matches.right.find(message->chat->id);
        
        // Forward message to the other one
        if (it_left != _matches.left.end() or it_right != _matches.right.end()) {
            if (it_left != _matches.left.end()) {
                assert(it_right == _matches.right.end());
                _bot.getApi().sendMessage(it_left->second, message->text);
                fmt::print("{} > {}: {}\n", _users[message->chat->id], _users[it_left->second], message->text);
            }
            
            if (it_right != _matches.right.end()) {
                assert(it_left == _matches.left.end());
                _bot.getApi().sendMessage(it_right->second, message->text);
                fmt::print("{} > {}: {}\n", _users[message->chat->id], _users[it_right->second], message->text);
            }
        }
        else {
            this->bot_says(message->chat->id, "You are not matched with anyone. Use command /start and find someone!");
        }
    }
    catch(std::runtime_error) {
        this->bot_says(message->chat->id, "Invalid message, it wont arrive to destination.");
    }
}
