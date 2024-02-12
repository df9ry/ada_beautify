#ifndef FORMATTER_HPP
#define FORMATTER_HPP

#include "symbol.hpp"

#include <list>
#include <iostream>

class Formatter
{
public:
    typedef std::list<Symbol::Ref> SymbolListType;

    Formatter();

    void add(Symbol::Ref& sym) { symbol_list.push_back(sym); }
    void print(std::ostream& os);

private:
    SymbolListType symbol_list{};

    void optimize();
};

#endif // FORMATTER_HPP
