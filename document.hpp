#ifndef DOCUMENT_HPP
#define DOCUMENT_HPP

#include "scope.hpp"
#include "symbol.hpp"

#include <stack>
#include <string>
#include <functional>
#include <map>

class Document
{
public:
    typedef std::function<void (const std::string& token, Document& doc)> handlerType;
    typedef std::map<std::string, handlerType> handlerMapType;

    static const int spacesPerLevel = 3;
    static const int maxIndent = 80;

    Document();
    Document(const Document&) = delete;
    Document(Document&&) = delete;

    Scope& scope();
    const Scope& scope() const;
    void put(const Symbol& sym);
    void addComment(std::string comment);
    void print(std::ostream &os) const;
    void clear();
    Scope& openScope();
    void closeScope();
    const std::string indent(int offset = 0) const;
    int level() const { return stack.size(); }

    std::vector<std::string> lines{};

private:
    static const handlerMapType handlerMap;

    std::stack<Scope> stack{};
};

#endif // DOCUMENT_HPP
