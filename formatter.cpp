#include "formatter.hpp"
#include "document.hpp"

#include <algorithm>

using namespace std;

Formatter::Formatter() {}

void Formatter::optimize()
{
    // Remove header comments:
    {
        while (true) {
            auto it = symbol_list.begin();
            if (it == symbol_list.end())
                break;
            auto kind = it->get()->kind();
            if (kind != Symbol::Kind::COMMENT && kind != Symbol::Kind::NL)
                break;
            symbol_list.erase(it);
        } // end while //
    }
    // "and then", "or else", "is new":
    {
        SymbolListType list2;
        auto iter = symbol_list.begin();
        while (iter != symbol_list.end()) {
            const Symbol::Ref ref1 = *iter;
            if (ref1->value() == "and") {
                ++iter;
                const Symbol::Ref ref2 = *iter;
                if (ref2->value() == "then") {
                    list2.push_back(Symbol::Ref(
                        new SymbolIdentifier("and then")));
                } else {
                    list2.push_back(ref1);
                    list2.push_back(ref2);
                }
            } else if (ref1->value() == "or") {
                ++iter;
                const Symbol::Ref ref2 = *iter;
                if (ref2->value() == "else") {
                    list2.push_back(Symbol::Ref(
                        new SymbolIdentifier("or else")));
                } else {
                    list2.push_back(ref1);
                    list2.push_back(ref2);
                }
            } else if (ref1->value() == "is") {
                ++iter;
                const Symbol::Ref ref2 = *iter;
                if (ref2->value() == "new") {
                    list2.push_back(Symbol::Ref(
                        new SymbolIdentifier("is new")));
                } else {
                    list2.push_back(ref1);
                    list2.push_back(ref2);
                }
            } else {
                list2.push_back(ref1);
            }
            ++iter;
        } // end while //
        symbol_list = list2;
    }
}

void Formatter::print(std::ostream& os)
{
    optimize();
    Document doc;
    bool ok = true;
    for_each(symbol_list.begin(), symbol_list.end(), [&doc, &ok] (const Symbol::Ref& sym) {
        try {
            doc.put(*sym);
        }
        catch (const exception& ex) {
            if (ok) {
                cerr << "Warning: " << ex.what() << endl;
                ok = false;
            }
        }
        catch (...) {
            if (ok) {
                cerr << "Warning: Unknown failure" << endl;
                ok = false;
            }
        }
    });
    doc.print(os);
}
