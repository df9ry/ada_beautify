#include "formatter.hpp"
#include "symbol.hpp"
#include "version.hpp"

#include <iostream>
#include <fstream>
#include <filesystem>

#include <getopt.h>

using namespace std;
using namespace filesystem;

static istream& open_input(const path &fs) {
    static ifstream ifs;
    ifs.open(fs);
    if (!ifs.is_open())
        throw runtime_error(string("Unable to open input file \"") +
                                   fs.filename().string() + "\"");
    return ifs;
}

static ostream& open_output(const path &fs) {
    static ofstream ofs;
    ofs.open(fs);
    if (!ofs.is_open())
        throw runtime_error(string("Unable to open output file \"") +
                            fs.filename().string() + "\"");
    return ofs;
}

static void help(const char *name)
{
    cerr << "Usage: " << name << " [options]" << endl
         << "\t-i <input_file>  ... Read input from <input_file>" << endl
         << "\t-o <output_file> ... Write output to <output_file>" << endl
         << "\t-h ................. Print help (this message)" << endl
         << "\t-v ................. Verbose" << endl;
}

int main(int argc, char *argv[])
{
    int option{0};
    path input_file{""};
    path output_file{""};
    bool helped{false};
    int  verbose{0};

    // Get options:
    while ((option = getopt(argc, argv, "i:o:hv")) >= 0) {
        switch (option) {
        case 'i':
            if (input_file.empty()) {
                input_file = optarg;
                break;
            } else {
                help(argv[0]);
                return EXIT_FAILURE;
            }
        case 'o':
            if (output_file.empty()) {
                output_file = optarg;
                break;
            } else {
                help(argv[0]);
                return EXIT_FAILURE;
            }
        case 'h':
            help(argv[0]);
            return EXIT_SUCCESS;
        case 'v':
            ++ verbose;
            break;
        default:
            if (helped) {
                break;
            } else {
                help(argv[0]);
                helped = true;
            }
        } // end switch //
    } // end while //

    if (verbose)
        cerr << APP_NAME << " " << APP_VERSION << endl;

    try {
        istream& is{input_file.empty() ? cin : open_input(input_file)};
        ostream& os{output_file.empty() ? cout : open_output(output_file)};

        Symbol::open(is);
        while (!Symbol::get()); // Start reading

        Formatter formatter;
        Symbol::Ref sym;
        do {
            sym = Symbol::get();
            if (verbose > 1)
                cerr << "Insert " << sym->to_str() << endl;
            formatter.add(sym);
        } while (sym != Symbol::Kind::END);
        formatter.print(os);
    }
    catch(const exception &ex) {
        cerr << "Fatal error: " << ex.what() << endl;
        return EXIT_FAILURE;
    }
    catch(...) {
        cerr << "Unidentified fatal error" << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
