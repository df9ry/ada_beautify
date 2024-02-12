#include "document.hpp"
#include "scope.hpp"

#include <iostream>
#include <array>

using namespace std;

static array<char, Document::maxIndent> space;

static void copyOver(Document& doc)
{
    Scope& scope = doc.scope();
    string line = scope.lineBuffer.str();
    if (!line.empty()) {
        scope.content.push_back(line);
        scope.lineBuffer.str("");
    }
    doc.lines.insert(doc.lines.end(),
                     scope.comments.begin(),
                     scope.comments.end());
    scope.comments.clear();
    doc.lines.insert(doc.lines.end(),
                     scope.content.begin(),
                     scope.content.end());
    scope.content.clear();
}

static void newLine(Scope& scope)
{
    string line = scope.lineBuffer.str();
    if (!line.empty()) {
        scope.content.push_back(line);
        scope.lineBuffer.str("");
    }
}

static void handleEnd(const string& token, Document& doc)
{
    Scope& scope = doc.scope();
    // Copy what is already there:
    newLine(scope);
    // Output the end token with reduced indent:
    scope.lineBuffer << doc.indent(-1) << token;
    scope.end = true;
    scope.is = false;
}

static void handleSemicolon(const string& token, Document& doc)
{
    Scope& scope = doc.scope();
    // Append token to buffer:
    scope.lineBuffer << token;
    // And output the line:
    newLine(scope);
    if (scope.end) { // Copy everything over to doc:
        copyOver(doc);
        doc.closeScope();
    }
    scope.end = false;
}

static void handleIdentifier(const string& token, Document& doc) {
    Scope& scope = doc.scope();
    if (scope.lineBuffer.str().empty())
        scope.lineBuffer << doc.indent() << token;
    else
        scope.lineBuffer << (scope.dot ? "" : " ") << token;
    scope.dot = false;
}

static void handleIs(const string& token, Document& doc) {
    // Append the token:
    handleIdentifier(token, doc);
    Scope& scope = doc.scope();
    // Start a new line:
    newLine(scope);
    // Copy over:
    copyOver(doc);
    // And start new scope:
    Scope& newScope = doc.openScope();
    newScope.is = true;
}

static void handleElse(const string& token, Document& doc) {
    Scope& scope = doc.scope();
    newLine(scope);
    scope.lineBuffer << doc.indent(-1) << token;
    newLine(scope);
}

static void handleBegin(const string& token, Document& doc) {
    Scope& scope = doc.scope();
    newLine(scope);
    scope.lineBuffer << doc.indent(scope.is ? -1 : 0) << token;
    newLine(scope);
    if (scope.is) {
        scope.is = false;
    } else {
        copyOver(doc);
        doc.openScope();
    }
}

static void handleLoop(const string& token, Document& doc) {
    Scope& scope = doc.scope();
    if (scope.end) { // Special case for "end loop"
        scope.lineBuffer << " " << token;
        return;
    }
    newLine(scope);
    scope.lineBuffer << doc.indent(scope.is ? -1 : 0) << token;
    newLine(scope);
    if (scope.is) {
        scope.is = false;
    } else {
        copyOver(doc);
        doc.openScope();
    }
}

static void handleDot(const string& token, Document& doc) {
    Scope& scope = doc.scope();
    if (scope.lineBuffer.str().empty())
        scope.lineBuffer << doc.indent() << token;
    else
        scope.lineBuffer << token;
    scope.dot = true;
}

static void handleNoLeft(const string& token, Document& doc) {
    Scope& scope = doc.scope();
    if (scope.lineBuffer.str().empty())
        scope.lineBuffer << doc.indent() << token;
    else
        scope.lineBuffer << token;
    scope.dot = false;
}

static void handleNoRight(const string& token, Document& doc) {
    Scope& scope = doc.scope();
    if (scope.lineBuffer.str().empty())
        scope.lineBuffer << doc.indent() << token;
    else
        scope.lineBuffer << (scope.dot ? "" : " ") << token;
    scope.dot = true;
}

static void handleAndThen(const string& token, Document& doc) {
    Scope& scope = doc.scope();
    if (scope.lineBuffer.str().empty())
        scope.lineBuffer << doc.indent() << token;
    else
        scope.lineBuffer << (scope.dot ? "" : " ") << token;
    newLine(scope);
    scope.lineBuffer << doc.indent(+1);
    scope.dot = true;
}

const Document::handlerMapType Document::handlerMap {
    { "begin",     handleBegin     },
    { "end",       handleEnd       },
    { "else",      handleElse      },
    { "is",        handleIs        },
    { "loop",      handleLoop      },
    { "then",      handleBegin     },
    { ";",         handleSemicolon },
    { ".",         handleDot       },
    { ",",         handleNoLeft    },
    { ")",         handleNoLeft    },
    { "(",         handleNoRight   },
    { "<<",        handleNoRight   },
    { ">>",        handleNoLeft    },
    { "and then",  handleAndThen   },
    { "or else",   handleAndThen   },
};

Document::Document() {
    space.fill(' ');
    space[space.size() - 1] = '\0';
    stack.push(Scope(*this));
}

void Document::addComment(string s) {
    Scope& currentScope = scope();
    string i = indent();
    size_t pos = 0;
    while ((pos = s.find('\n')) != string::npos) {
        currentScope.comments.push_back(i + s.substr(0, pos));
        s.erase(0, pos + 1);
    }
    currentScope.comments.push_back(i + s);
}

void Document::put(const Symbol& sym)
{
    //cerr << "Put " << sym.value() << endl;
    switch (sym.kind()) {
    case Symbol::Kind::END :
        return; // Nothing to do with that
    case Symbol::Kind::COMMENT :
        addComment(sym.value());
        return;
    case Symbol::Kind::NL :
        // Ignore:
        return;
    default :
        string token = sym.value();
        auto item = handlerMap.find(token);
        if (item == handlerMap.end()) {
            handleIdentifier(token, *this);
        } else {
            item->second(token, *this);
        }
    } // end switch //
}

const string Document::indent(int offset) const
{
    int count = min((stack.size() - 1 + offset) * spacesPerLevel,
                    space.size() - 1);
    count = max(count, 0);
    //cerr << "Indent: " << count << endl;
    return string(&space[space.size() - 1 - count]);
}

void Document::print(std::ostream& os) const
{
    for_each(lines.begin(), lines.end(), [&os] (const string& s) {
        os << s << endl;
    });
}

Scope& Document::scope()
{
    if (stack.empty())
        throw runtime_error("Stack underflow");
    return stack.top();
}

Scope& Document::openScope()
{
    stack.push(Scope(*this));
    return stack.top();
}

void Document::closeScope()
{
    if (stack.size() > 1) // Avoid runtime failures
        stack.pop();      // On regular end
}
