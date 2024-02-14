#include "scanner.hpp"
#include "symbol.hpp"
#include "utils.hpp"

#include <sstream>

Symbol::Ref Symbol::cur_sym;
Symbol::Ref Symbol::next_sym;

static scanner *sc{nullptr};

void Symbol::open(std::istream& is) {
    if (sc)
        throw std::runtime_error("Input stream already open");
    sc = new scanner(is);
    get();
}

const Symbol::Ref Symbol::get() {
    cur_sym = next_sym;
    if (!sc)
        throw std::runtime_error("Input stream not open");
    while (!sc->eof()) {
        sc->skip_whitespace();
        if (sc->eof()) {
            next_sym = Symbol::Ref(new SymbolEnd());
            return cur_sym;
        }
        switch(sc->cur_ch) {
        case '-':
            sc->get_ch();
            if (sc->cur_ch == '-') {
                sc->get_ch();
                if ((sc->cur_ch == ' ') ||
                    (sc->cur_ch == '\t') ||
                    (sc->cur_ch == '\n'))
                {
                    // This is a comment
                    sc->skip_whitespace();
                    std::stringstream ss;
                    while ((sc->cur_ch != '\n') && (sc->cur_ch != EOF)) {
                        ss << static_cast<char>(sc->cur_ch);
                        sc->get_ch();
                    } // end while //
                    // Suppress empty comments:
                    std::string s = ss.str();
                    rtrim(s);
                    if (s.empty())
                        next_sym = Symbol::Ref(new SymbolNewLine());
                    else
                        next_sym = Symbol::Ref(new SymbolComment(s));
                    return cur_sym;
                }
                // This is the -- operator
                next_sym = Symbol::Ref(new SymbolOperator("--"));
                return cur_sym;
            }
            // This is the - operator
            next_sym = Symbol::Ref(new SymbolOperator("-"));
            return cur_sym;
            break;
        case '+':
            sc->get_ch();
            if (sc->cur_ch == '+') {
                // This is the ++ operator
                sc->get_ch();
                next_sym = Symbol::Ref(new SymbolOperator("++"));
                return cur_sym;
            }
            if (sc->cur_ch == '=') {
                // This is the += operator
                sc->get_ch();
                next_sym = Symbol::Ref(new SymbolOperator("+="));
                return cur_sym;
            }
            // This is the + operator
            next_sym = Symbol::Ref(new SymbolOperator("+"));
            return cur_sym;
        case '*':
            sc->get_ch();
            if (sc->cur_ch == '*') {
                // This is the ** operator
                sc->get_ch();
                next_sym = Symbol::Ref(new SymbolOperator("**"));
                return cur_sym;
            }
            // This is the * operator
            next_sym = Symbol::Ref(new SymbolOperator("*"));
            return cur_sym;
        case '/':
            sc->get_ch();
            if (sc->cur_ch == '=') {
                // This is the /= operator
                sc->get_ch();
                next_sym = Symbol::Ref(new SymbolOperator("/="));
                return cur_sym;
            }
            // This is the / operator
            next_sym = Symbol::Ref(new SymbolOperator("/"));
            return cur_sym;
        case '=':
            sc->get_ch();
            if (sc->cur_ch == '=') {
                // This is the == operator
                sc->get_ch();
                next_sym = Symbol::Ref(new SymbolOperator("=="));
                return cur_sym;
            }
            if (sc->cur_ch == '>') {
                // This is the => operator
                sc->get_ch();
                next_sym = Symbol::Ref(new SymbolOperator("=>"));
                return cur_sym;
            }
            // This is the = operator
            next_sym = Symbol::Ref(new SymbolOperator("="));
            return cur_sym;
        case '>':
            sc->get_ch();
            if (sc->cur_ch == '>') {
                // This is the >> operator
                sc->get_ch();
                next_sym = Symbol::Ref(new SymbolOperator(">>"));
                return cur_sym;
            }
            if (sc->cur_ch == '=') {
                // This is the >= operator
                sc->get_ch();
                next_sym = Symbol::Ref(new SymbolOperator(">="));
                return cur_sym;
            }
            // This is the > operator
            next_sym = Symbol::Ref(new SymbolOperator(">"));
            return cur_sym;
        case '<':
            sc->get_ch();
            if (sc->cur_ch == '<') {
                // This is the << operator
                sc->get_ch();
                next_sym = Symbol::Ref(new SymbolOperator("<<"));
                return cur_sym;
            }
            if (sc->cur_ch == '=') {
                // This is the <= operator
                sc->get_ch();
                next_sym = Symbol::Ref(new SymbolOperator("<="));
                return cur_sym;
            }
            if (sc->cur_ch == '>') {
                // This is the <> operator
                sc->get_ch();
                next_sym = Symbol::Ref(new SymbolOperator("/="));
                return cur_sym;
            }
            // This is the < operator
            next_sym = Symbol::Ref(new SymbolOperator("<"));
            return cur_sym;
        case ':':
            sc->get_ch();
            if (sc->cur_ch == '=') {
                // This is the := operator
                sc->get_ch();
                next_sym = Symbol::Ref(new SymbolOperator(":="));
                return cur_sym;
            }
            // This is the : operator
            next_sym = Symbol::Ref(new SymbolOperator(":"));
            return cur_sym;
        case '.':
            sc->get_ch();
            if (sc->cur_ch == '.') {
                // This is the .. operator
                sc->get_ch();
                next_sym = Symbol::Ref(new SymbolOperator(".."));
                return cur_sym;
            }
            // This is the . operator
            next_sym = Symbol::Ref(new SymbolOperator("."));
            return cur_sym;
        case ',':
            sc->get_ch();
            // This is the , operator
            next_sym = Symbol::Ref(new SymbolOperator(","));
            return cur_sym;
        case ';':
            sc->get_ch();
            // This is the ; operator
            next_sym = Symbol::Ref(new SymbolOperator(";"));
            return cur_sym;
        case '(':
            sc->get_ch();
            // This is the ( operator
            next_sym = Symbol::Ref(new SymbolOperator("("));
            return cur_sym;
        case ')':
            sc->get_ch();
            // This is the ) operator
            next_sym = Symbol::Ref(new SymbolOperator(")"));
            return cur_sym;
        case '[':
            sc->get_ch();
            // This is the [ operator
            next_sym = Symbol::Ref(new SymbolOperator("["));
            return cur_sym;
        case ']':
            sc->get_ch();
            // This is the ] operator
            next_sym = Symbol::Ref(new SymbolOperator("]"));
            return cur_sym;
        case '&':
            sc->get_ch();
            // This is the & operator
            next_sym = Symbol::Ref(new SymbolOperator("&"));
            return cur_sym;
        case '|':
            sc->get_ch();
            // This is the | operator
            next_sym = Symbol::Ref(new SymbolOperator("|"));
            return cur_sym;
        case '\n':
            sc->get_ch();
            // This is new line
            next_sym = Symbol::Ref(new SymbolNewLine());
            return cur_sym;
        case '\'':
            sc->get_ch();
            // This is a character constant:
            next_sym = Symbol::Ref(new SymbolChar(static_cast<char>(sc->cur_ch)));
            sc->get_ch();
            if (sc->cur_ch == '\'')
                sc->get_ch();
            return cur_sym;
        case '#':
            {
                sc->get_ch();
                int x = fm_hex(sc->cur_ch) << 8;
                sc->get_ch();
                x |= fm_hex(sc->cur_ch);
                sc->get_ch();
                next_sym = Symbol::Ref(new SymbolByte(x));
                return cur_sym;
            }
        case '"':
            {
                std::stringstream ss;
                sc->get_ch();
                while (true) {
                    if (sc->cur_ch == '"') {
                        sc->get_ch();
                        if (sc->cur_ch == '"') {
                            ss << "\"";
                            sc->get_ch();
                            continue;
                        } else {
                            next_sym = Symbol::Ref(new SymbolString(ss.str()));
                            return cur_sym;
                        }
                    }
                    ss << static_cast<char>(sc->cur_ch);
                    sc->get_ch();
                } // end while //
            }
        case '\t':
        case '\r':
            sc->get_ch();
            break;
        default:
            {
                std::stringstream ss;
            if (is_tokenchar(sc->cur_ch)) {
                    do {
                        ss << static_cast<char>(sc->cur_ch);
                        sc->get_ch();
                    } while (is_tokenchar(sc->cur_ch));
                    std::string s{ss.str()};
                    if (s[0] >= '0' && s[0] <= '9')
                        next_sym = Symbol::Ref(new SymbolNumber(s));
                    else
                        next_sym = Symbol::Ref(new SymbolIdentifier(s));
                    return cur_sym;
                }
                next_sym = Symbol::Ref(new SymbolByte(sc->cur_ch));
                sc->get_ch();
                return cur_sym;
            }
        } // end switch //
    } // end while //
    next_sym = Symbol::Ref(new SymbolEnd());
    return cur_sym;
}
