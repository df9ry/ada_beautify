#ifndef SYMBOL_HPP
#define SYMBOL_HPP

#include "utils.hpp"

#include <istream>
#include <memory>

class Symbol
{
public:
    typedef std::shared_ptr<Symbol> Ref;

    enum class Kind {
        END,
        OPERATOR,
        IDENTIFIER,
        NUMBER,
        NL,
        BYTE,
        CHAR,
        STRING,
        COMMENT,
    };

    static void open(std::istream &is);
    static const Ref get();
    static const Ref current() { return cur_sym; }
    static const Ref next() { return next_sym; }

    virtual const Kind kind() const = 0;
    virtual const std::string to_str() const = 0;

    const std::string& value() const { return _value; }

protected:
    Symbol(const std::string& value): _value{value} {}

private:
    static Ref cur_sym;
    static Ref next_sym;

    const std::string _value;
};

inline bool operator==(const Symbol::Ref r, Symbol::Kind k) { return k == r->kind(); }
inline bool operator!=(const Symbol::Ref r, Symbol::Kind k) { return k != r->kind(); }

class SymbolEnd: public Symbol {
public:
    SymbolEnd(): Symbol("\0") {};

    virtual const Kind kind() const { return Kind::END; }
    virtual const std::string to_str() const { return "END"; }
};

class SymbolOperator: public Symbol {
public:
    SymbolOperator(const std::string& op): Symbol(op) {};

    virtual const Kind kind() const { return Kind::OPERATOR; }
    virtual const std::string to_str() const { return "OP " + value(); }
};

class SymbolIdentifier: public Symbol {
public:
    SymbolIdentifier(const std::string& token): Symbol(token) {};

    virtual const Kind kind() const { return Kind::IDENTIFIER; }
    virtual const std::string to_str() const { return "ID " + value(); }
};

class SymbolNumber: public Symbol {
public:
    SymbolNumber(const std::string& token): Symbol(token) {};

    virtual const Kind kind() const { return Kind::NUMBER; }
    virtual const std::string to_str() const { return "NU " + value(); }
};

class SymbolNewLine: public Symbol {
public:
    SymbolNewLine(): Symbol("\n") {};

    virtual const Kind kind() const { return Kind::NL; }
    virtual const std::string to_str() const { return "NL"; }
};

class SymbolByte: public Symbol {
public:
    SymbolByte(const unsigned char b): Symbol("#" + to_hex(b)) {};

    virtual const Kind kind() const { return Kind::BYTE; }
    virtual const std::string to_str() const { return "BY " + value(); }
};

class SymbolChar: public Symbol {
public:
    SymbolChar(const char c): Symbol("'" + std::string(1, c) + "'") {};

    virtual const Kind kind() const { return Kind::CHAR; }
    virtual const std::string to_str() const { return "CH " + value(); }
};

class SymbolString: public Symbol {
public:
    SymbolString(const std::string& text): Symbol("\"" + text + "\"") {};

    virtual const Kind kind() const { return Kind::STRING; }
    virtual const std::string to_str() const { return "ST " + value(); }
};

class SymbolComment: public Symbol {
public:
    SymbolComment(const std::string& text): Symbol("--  " + text) {};

    virtual const Kind kind() const { return Kind::COMMENT; }
    virtual const std::string to_str() const { return "CO " + value(); }
};

#endif // SYMBOL_HPP
