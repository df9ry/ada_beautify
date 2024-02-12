#ifndef UNIT_HPP
#define UNIT_HPP

#include "scope.hpp"
#include "item.hpp"

#include <vector>
#include <string>

class Unit: public Scope, public Item
{
public:
    Unit(): Scope(0), Item() {}
    Unit(int level): Scope(level), Item() {}

    virtual void print(std::ostream& os);
    virtual std::string parse(Formatter::SymbolListType::iterator& iter);

private:
    std::vector<Item::Ref> items{};
};

#endif // UNIT_HPP
