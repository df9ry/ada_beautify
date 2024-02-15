#include "document.hpp"
#include "scope.hpp"

#include <iostream>
#include <array>

extern int verbose;

using namespace std;

static array<char, Document::maxIndent + 1> space;

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

static void newLine(Scope& scope, bool force = false)
{
    string line = scope.lineBuffer.str();
    if (force || !line.empty()) {
        scope.content.push_back(line);
        scope.lineBuffer.str("");
    }
}

static void handleEnd(const string& token, Document& doc)
{
    Scope& scope = doc.scope();
    scope.end = true;
    scope.is = false;
}

static void handleIdentifier(const string& token, Document& doc) {
    Scope& scope = doc.scope();
    if (scope.end) {
        scope.end_id = token;
    } else {
        if (scope.lineBuffer.str().empty())
            scope.lineBuffer << doc.indent() << token;
        else
            scope.lineBuffer << (scope.dot ? "" : " ") << token;
    }
    scope.dot = false;
}

static void handleSemicolon(const string& token, Document& doc)
{
    Scope& scope = doc.scope();
    if (scope.para) {
        if (scope.lineBuffer.str().empty())
            scope.lineBuffer << doc.indent() << token;
        else
            scope.lineBuffer << token;
    } else if (scope.end) {
        // Copy what is already there:
        newLine(scope);
        // Output the end token with reduced indent:
        if (scope.end_id == "case") {
            scope.lineBuffer << doc.indent(-2) << "end "
                             << scope.end_id << token;
            scope.end = false;
            scope.end_id = "";
            copyOver(doc);
            doc.closeScope();
            Scope& scope2 = doc.scope();
            copyOver(doc);
            doc.closeScope();
            Scope& scope3 = doc.scope();
        } else if (scope.end_id == "") {
            scope.lineBuffer << doc.indent(-1) << "end"
                             << scope.end_id << token;
            scope.end = false;
            scope.end_id = "";
            copyOver(doc);
            doc.closeScope();
            Scope& scope2 = doc.scope();
        } else {
            scope.lineBuffer << doc.indent(-1) << "end "
                             << scope.end_id << token;
            if (scope.end_id == "loop")
                scope.loop = false;
            scope.end = false;
            scope.end_id = "";
            copyOver(doc);
            doc.closeScope();
            Scope& scope2 = doc.scope();
        }
    } else {
        // Append token to buffer:
        scope.lineBuffer << token;
        // And output the line:
        newLine(scope);
    }
}

static void handleIs(const string& token, Document& doc) {
    // Append the token:
    handleIdentifier(token, doc);
    Scope& scope = doc.scope();
    if (scope.type) {
        scope.type = false;
    } else {
        // Start a new line:
        newLine(scope);
        // Copy over:
        copyOver(doc);
        // And start new scope:
        Scope& newScope = doc.openScope();
        newScope.is = true;
    }
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
        handleIdentifier(token, doc);
    } else {
        newLine(scope);
        scope.lineBuffer << doc.indent(scope.is ? -1 : 0) << token;
        newLine(scope);
        if (scope.is) {
            scope.is = false;
        } else {
            copyOver(doc);
            doc.openScope();
            Scope& scope2 = doc.scope();
            scope2.loop = true;
        }
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

static void handleArrow(const string& token, Document& doc) {
    Scope& scope = doc.scope();
    if (scope.para) {
        handleIdentifier(token, doc);
    } else {
        if (scope.lineBuffer.str().empty())
            scope.lineBuffer << doc.indent() << token;
        else
            scope.lineBuffer << (scope.dot ? "" : " ") << token;
        newLine(scope);
        copyOver(doc);
        doc.openScope();
        Scope& scope2 = doc.scope();
    }
}

static void handleWhen(const string& token, Document& doc) {
    Scope& scope = doc.scope();
    if (scope.para) {
        handleIdentifier(token, doc);
    } else {
        if (!(scope.is || scope.loop || scope.exit)) {
            copyOver(doc);
            doc.closeScope();
            Scope& scope2 = doc.scope();
        }
        scope.exit = false;
        scope.loop = false;
        scope.is = false;
        handleIdentifier(token, doc);
    }
}

static void handleCase(const string& token, Document& doc) {
    Scope& scope = doc.scope();
    handleIdentifier(token, doc);
}

static void handleLabel(const string& token, Document& doc) {
    Scope& scope = doc.scope();
    if (scope.lineBuffer.str().empty())
        scope.lineBuffer << doc.indent() << token;
    else
        scope.lineBuffer << token;
    newLine(scope);
}

static void handleOPara(const string& token, Document& doc) {
    handleNoRight(token, doc);
    Scope& scope = doc.scope();
    --scope.para;
}

static void handleCPara(const string& token, Document& doc) {
    handleNoLeft(token, doc);
    Scope& scope = doc.scope();
    ++scope.para;
}

static void handleExit(const string& token, Document& doc) {
    Scope& scope = doc.scope();
    if (scope.lineBuffer.str().empty())
        scope.lineBuffer << doc.indent() + token;
    else
        scope.lineBuffer << " " + token;
    scope.exit = true;
}

static void handleType(const string& token, Document& doc) {
    Scope& scope = doc.scope();
    scope.type = true;
}

static void handleNlBefore(const string& token, Document& doc) {
    Scope& scope = doc.scope();
    copyOver(doc);
    newLine(scope, true);
    handleIdentifier(token, doc);
}

static void handleRecord(const string& token, Document& doc) {
    Scope& scope = doc.scope();
    if (scope.end) {
        scope.end_id = "record";
    } else if (scope.is) {
        scope.is = false;
        scope.lineBuffer << " " << token;
    } else {
        newLine(scope);
        scope.lineBuffer << doc.indent() << token;
        newLine(scope);
        copyOver(doc);
        doc.openScope();
    }
}

const Document::handlerMapType Document::handlerMap {
    { "begin",     handleBegin     },
    { "end",       handleEnd       },
    { "else",      handleElse      },
    { "is",        handleIs        },
    { "loop",      handleLoop      },
    { "then",      handleBegin     },
    { "when",      handleWhen      },
    { "case",      handleCase      },
    { "exit",      handleExit      },
    { "type",      handleType      },
    { "record",    handleRecord    },
    { "package",   handleNlBefore  },
    { "procedure", handleNlBefore  },
    { "function",  handleNlBefore  },
    { ";",         handleSemicolon },
    { ".",         handleDot       },
    { ",",         handleNoLeft    },
    { ")",         handleCPara     },
    { "(",         handleOPara     },
    { "<<",        handleNoRight   },
    { ">>",        handleLabel     },
    { "=>",        handleArrow     },
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
    int count = (stack.size() + offset - 1) * spacesPerLevel;
    if (count < 0)
        count = 0;
    if (count > maxIndent)
        count = maxIndent;
    return string(&space[space.size() - 1 - count]);
}

void Document::print(std::ostream& os) const
{
    for_each(lines.begin(), lines.end(), [&os] (const string& s) {
        os << s << endl;
    });
}

void Document::clear()
{
    lines.clear();
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
    if (verbose)
        lines.push_back(
            string("--  << New Scope Level ") + to_string(level()) + " >>");
    return stack.top();
}

void Document::closeScope()
{
    if (stack.size() == 0)
        throw runtime_error("stack underflow");
    stack.pop();      // On regular end
    if (verbose)
        lines.push_back(
            string("--  << Cur Scope Level ") + to_string(level()) + " >>");
}
