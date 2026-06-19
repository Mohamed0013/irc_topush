#include "../headers/bot.hpp"
#include "../headers/server.hpp"
#include "../headers/Client.hpp"
#include <sstream>
#include <ctime>
#include <cstdlib>

#ifndef BOT_NICK
# define BOT_NICK "IRCBot"
#endif

#ifndef BOT_USER
# define BOT_USER "ircbot"
#endif

#ifndef BOT_HOST
# define BOT_HOST "localhost"
#endif

Bot::Bot()
    : _nick(BOT_NICK), _user(BOT_USER), _host(BOT_HOST)
{
    std::srand(std::time(0));
    initFacts();
    initJokes();
}

Bot::~Bot() {}

const std::string& Bot::getNick() const { return _nick; }
const std::string& Bot::getUser() const { return _user; }
const std::string& Bot::getHost() const { return _host; }

void Bot::initFacts()
{
    _facts.push_back("The first IRC was created by Jarkko Oikarinen in 1988.");
    _facts.push_back("IRC can support up to 512 bytes per message.");
    _facts.push_back("The term 'bot' comes from the word 'robot'.");
    _facts.push_back("C++ was designed by Bjarne Stroustrup in 1985.");
    _facts.push_back("Epoll is Linux-specific; kqueue is used on BSD/macOS.");
    _facts.push_back("The first computer virus was created in 1983.");
    _facts.push_back("HTTP/2 was standardized in 2015.");
    _facts.push_back("Ethernet was invented at Xerox PARC in 1973.");
    _facts.push_back("The Python programming language was released in 1991.");
    _facts.push_back("Git was created by Linus Torvalds in 2005.");
    _facts.push_back("The Internet was born from ARPANET in 1969.");
    _facts.push_back("A 'modem' stands for modulator-demodulator.");
    _facts.push_back("JavaScript was created in 10 days by Brendan Eich.");
    _facts.push_back("TCP/IP became the standard protocol in 1983.");
    _facts.push_back("The first website ever is still online: info.cern.ch.");
}

void Bot::initJokes()
{
    _jokes.push_back("Why do C++ programmers get Halloween and Christmas mixed up? Because DEC 25 is OCT 31.");
    _jokes.push_back("Why did the programmer quit his job? He didn't get arrays.");
    _jokes.push_back("A SQL query goes into a bar, walks up to two tables and asks: 'Can I join you?'");
    _jokes.push_back("There are only 10 types of people in the world: those who understand binary, and those who don't.");
    _jokes.push_back("Why do Java developers wear glasses? Because they can't C#.");
    _jokes.push_back("How many programmers does it take to change a light bulb? None, that's a hardware problem.");
    _jokes.push_back("I would tell you a joke about UDP, but you might not get it.");
    _jokes.push_back("Why was the JavaScript developer sad? Because he didn't Node how to Express himself.");
    _jokes.push_back("A programmer puts two glasses on his bedside table before going to sleep: a full one in case he gets thirsty, and an empty one in case he doesn't.");
    _jokes.push_back("Why do programmers prefer dark mode? Because light attracts bugs.");
}

std::string Bot::getTimeResponse() const
{
    std::time_t now = std::time(0);
    std::string result = "The current server time is: ";
    result += std::ctime(&now);
    // Remove trailing newline from ctime
    if (!result.empty() && result[result.size() - 1] == '\n')
        result.erase(result.size() - 1);
    return result;
}

std::string Bot::getDateResponse() const
{
    std::time_t now = std::time(0);
    std::string result = "Today's date is: ";
    result += std::ctime(&now);
    if (!result.empty() && result[result.size() - 1] == '\n')
        result.erase(result.size() - 1);
    return result;
}

std::string Bot::getHelp() const
{
    std::string help;
    help += "I am " + _nick + ", your IRC helper bot!\r\n";
    help += "Available commands (use !command or PRIVMSG " + _nick + " :!command):\r\n";
    help += "  !help        - Show this help message\r\n";
    help += "  !time        - Show current server time\r\n";
    help += "  !date        - Show today's date\r\n";
    help += "  !info        - Show server statistics\r\n";
    help += "  !fact        - Get a random interesting fact\r\n";
    help += "  !joke        - Tell a programming joke\r\n";
    help += "  !fortune     - Get a fortune cookie message\r\n";
    help += "  !ping        - Check if the bot is alive\r\n";
    help += "  !version     - Show bot version\r\n";
    help += "  !calc <expr> - Evaluate a simple arithmetic expression\r\n";
    help += "  !echo <msg>  - Make the bot repeat your message\r\n";
    return help;
}

std::string Bot::getInfoResponse(const Server& server) const
{
    const std::map<int, Client>& clients = server.get_mapClient();
    const std::map<std::string, class chanel>& channels = server.get_Chanel();

    std::string info;
    std::ostringstream oss;

    info += "=== Server Information ===\r\n";
    info += "Bot: " + _nick + "\r\n";

    oss << clients.size();
    info += "Connected users: " + oss.str() + "\r\n";
    oss.str("");

    oss << channels.size();
    info += "Active channels: " + oss.str() + "\r\n";
    oss.str("");

    // Count total channel members
    int total_members = 0;
    std::map<std::string, class chanel>::const_iterator it;
    for (it = channels.begin(); it != channels.end(); ++it)
    {
        total_members += it->second.get_All_Cchanel().size();
    }
    oss << total_members;
    info += "Total channel memberships: " + oss.str() + "\r\n";

    info += "==========================";

    return info;
}

std::string Bot::getFortune() const
{
    const char* fortunes[] = {
        "Good news will come to you by mail.",
        "A beautiful, smart, and loving person will be coming into your life.",
        "A dubious friend may be an enemy in camouflage.",
        "A fresh start will put you on your way.",
        "A golden opportunity is coming your way — seize it!",
        "A lifetime of happiness lies ahead of you.",
        "You will make many changes before settling down happily.",
        "Adventure can be real happiness.",
        "All is well. Proceed with confidence.",
        "Believe it can be done.",
        "Your hard work will soon pay off.",
        "You will be singled out for a promotion.",
        "The best time to start is now.",
        "Patience is a virtue. Good things come to those who wait.",
        "You will soon be surrounded by good friends."
    };
    size_t index = std::rand() % (sizeof(fortunes) / sizeof(fortunes[0]));
    return std::string("🔮 ") + fortunes[index];
}

std::string Bot::getJoke() const
{
    if (_jokes.empty())
        return "I don't have any jokes right now!";
    size_t index = std::rand() % _jokes.size();
    return "😂 " + _jokes[index];
}

std::string Bot::processCommand(const std::string& cmd, const std::vector<std::string>& args, const Server& server) const
{
    if (cmd == "help" || cmd == "?")
        return getHelp();
    if (cmd == "time")
        return getTimeResponse();
    if (cmd == "date")
        return getDateResponse();
    if (cmd == "info")
        return getInfoResponse(server);
    if (cmd == "fact")
    {
        if (_facts.empty())
            return "I don't have any facts right now!";
        size_t index = std::rand() % _facts.size();
        return "💡 " + _facts[index];
    }
    if (cmd == "joke")
        return getJoke();
    if (cmd == "fortune")
        return getFortune();
    if (cmd == "ping")
        return "🏓 Pong! I'm alive.";
    if (cmd == "version")
        return _nick + " version 1.0.0 | C++98 IRC Bot";
    if (cmd == "echo")
    {
        std::string msg;
        for (size_t i = 0; i < args.size(); ++i)
        {
            if (i > 0) msg += " ";
            msg += args[i];
        }
        return "📢 " + msg;
    }
    if (cmd == "calc")
    {
        if (args.empty())
            return "Usage: !calc <expression> (e.g., !calc 2 + 3)";
        std::string expr;
        for (size_t i = 0; i < args.size(); ++i)
        {
            if (i > 0) expr += " ";
            expr += args[i];
        }
        return evaluateExpr(expr);
    }
    return std::string();
}

std::string Bot::evaluateExpr(const std::string& expr) const
{
    // Simple arithmetic evaluator for basic expressions
    // Format: num op num (e.g., "5 + 3", "10 * 2")
    std::string clean;
    for (size_t i = 0; i < expr.size(); ++i)
    {
        if (expr[i] != ' ')
            clean += expr[i];
    }

    // Find operator
    std::string ops = "+-*/%";
    size_t op_pos = std::string::npos;
    char op = 0;
    for (size_t i = 0; i < ops.size(); ++i)
    {
        size_t pos = clean.find(ops[i], 1); // skip first char in case of negative
        if (pos != std::string::npos && (op_pos == std::string::npos || pos < op_pos))
        {
            if (i == 0 && pos == 0) continue ; // leading '+'
            if ((i == 1 || i == 4) && pos == 0) continue ; // leading '-'
            op_pos = pos;
            op = ops[i];
        }
    }

    if (op_pos == std::string::npos)
        return "Error: No operator found. Use format: num op num (e.g., 5 + 3)";

    std::string left_str = clean.substr(0, op_pos);
    std::string right_str = clean.substr(op_pos + 1);

    if (left_str.empty() || right_str.empty())
        return "Error: Invalid expression";

    // Validate digits
    for (size_t i = 0; i < left_str.size(); ++i)
        if (!std::isdigit(left_str[i]) && !(i == 0 && left_str[i] == '-'))
            return "Error: Invalid number: " + left_str;
    for (size_t i = 0; i < right_str.size(); ++i)
        if (!std::isdigit(right_str[i]) && !(i == 0 && right_str[i] == '-'))
            return "Error: Invalid number: " + right_str;

    std::istringstream lss(left_str);
    std::istringstream rss(right_str);
    long left, right;
    lss >> left;
    rss >> right;

    std::ostringstream result;
    result << "🔢 ";

    switch (op)
    {
        case '+': result << (left + right); break ;
        case '-': result << (left - right); break ;
        case '*': result << (left * right); break ;
        case '/':
            if (right == 0) return "Error: Division by zero";
            result << (left / right);
            break ;
        case '%':
            if (right == 0) return "Error: Modulo by zero";
            result << (left % right);
            break ;
        default:
            return "Error: Unknown operator";
    }

    return result.str();
}

std::string Bot::handleMessage(const std::string& message, const Server& server) const
{
    // Check if message starts with '!' (channel command)
    if (message.empty())
        return "";

    if (message[0] == '!')
    {
        // Parse the command
        std::string cmd_line = message.substr(1); // skip '!'
        std::vector<std::string> cmd_parts;
        std::string current;
        for (size_t i = 0; i < cmd_line.size(); ++i)
        {
            if (cmd_line[i] == ' ' && !current.empty())
            {
                cmd_parts.push_back(current);
                current.clear();
            }
            else if (cmd_line[i] != ' ')
            {
                current += cmd_line[i];
            }
        }
        if (!current.empty())
            cmd_parts.push_back(current);

        if (cmd_parts.empty())
            return "Bot commands start with !. Try !help";

        std::string cmd = cmd_parts[0];
        std::vector<std::string> cmd_args;
        for (size_t i = 1; i < cmd_parts.size(); ++i)
            cmd_args.push_back(cmd_parts[i]);

        return processCommand(cmd, cmd_args, server);
    }

    return "";
}

std::string Bot::handleDirectCommand(const std::string& cmd, const std::vector<std::string>& args, const Server& server) const
{
    return processCommand(cmd, args, server);
}