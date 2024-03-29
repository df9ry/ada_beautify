#ifndef SCOPE_HPP
#define SCOPE_HPP

#include <sstream>
#include <vector>

class Document;

class Scope
{
public:
    Scope(Document& _doc): doc{_doc} {}

    Document& doc;
    std::stringstream lineBuffer{};
    bool end{false};
    bool is{false};
    bool dot{false};
    bool loop{false};
    bool exit{false};
    bool type{false};
    std::string end_id{};
    int para{0};

    std::vector<std::string> comments{};
    std::vector<std::string> content{};
};

#endif // SCOPE_HPP
