// pasmo.cxx

#include "asm.h"
#include "asmerror.h"
#include "memmap.h"     // *JiK*

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <stdexcept>

using std::cout;
using std::cerr;

namespace
{

using std::string;
using std::vector;
using std::runtime_error;

// *JiK* Some hackery to make this possible with CMake.. 
#define STRINGIZE(x) #x
#define STRINGIZE_VALUE_OF(x) STRINGIZE(x)
const string pasmoversion(STRINGIZE_VALUE_OF(VERSION));

class Usage { };

class NeedArgument : public runtime_error
{
public:
    NeedArgument(const string & option) :
        runtime_error("Option " + option + " requires argument")
    { }
};

class InvalidOption : public runtime_error
{
public:
    InvalidOption(const string & option) :
        runtime_error("Invalid option: " + option)
    { }
};

std::ostream * perr = & cerr;

const string opt1("-1");
const string opt8("-8");
const string optd("-d");
const string optv("-v");
const string optB("-B");
const string optE("-E");
const string optI("-I");

const string opt86        ("--86");
const string optalocal    ("--alocal");
const string optamsdos    ("--amsdos");
const string optbin       ("--bin");
const string optbracket   ("--bracket");
const string optcdt       ("--cdt");
const string optcdtbas    ("--cdtbas");
const string optcmd       ("--cmd");
const string optequ       ("--equ");
const string opterr       ("--err");
const string opthex       ("--hex");
const string optmsx       ("--msx");
const string optname      ("--name");
const string optnocase    ("--nocase");
const string optpass3     ("--pass3");
const string optplus3dos  ("--plus3dos");
const string optprl       ("--prl");
const string optpublic    ("--public");
const string optsdrel     ("--sdrel");
const string opttap       ("--tap");
const string opttapbas    ("--tapbas");
const string opttap128    ("--tap128");         // *JiK*
const string opttapbas128 ("--tapbas128");      // *JiK*
const string optmemmap    ("--memmap");         // *JiK*
const string opttrs       ("--trs");
const string opttzx       ("--tzx");
const string opttzxbas    ("--tzxbas");
const string optw8080     ("--w8080");
const string optwerror    ("--werror");

class Options
{
public:
    Options(int argc, char * * argv);
    ~Options();                                         // *JiK*

    typedef void(Asm::* emitfunc_t) (std::ostream &);

    emitfunc_t getemit() const { return emitfunc; }
    bool redirerr() const { return redirecterr; }
    bool publiconly() const { return emitpublic; }
    bool getpass3() const { return pass3; }
    string getfilein() const { return filein; }
    string getfileout() const { return fileout; }
    string getfilesymbol() const { return filesymbol; }
    string getfilepublic() const;
    string getheadername() const { return headername; }
    void apply(Asm & assembler) const;
    MemMap& getmemmap();                                // *JiK*
    bool getdumpmemmap() const { return dumpmemmap; }   // *JiK*
private:
    emitfunc_t emitfunc;
    static const emitfunc_t emitdefault;
    MemMap* memmap;                                     // *JiK*
    bool dumpmemmap;                                    // *JiK*

    bool verbose;
    bool emitpublic;
    Asm::DebugType debugtype;
    bool redirecterr;
    bool nocase;
    bool autolocal;
    bool bracketonly;
    bool warn8080;
    bool mode86;
    bool werror;
    bool pass3;

    vector <string> includedir;
    vector <string> labelpredef;

    string filein;
    string fileout;
    string filesymbol;
    string filepublic;
    string headername;
};

const Options::emitfunc_t Options::emitdefault(& Asm::emitobject);

// *JiK*
MemMap& Options::getmemmap()
{
    if (memmap == NULL) {
        // If memory mapping was not explicit then create the
        // default plain 64K RAM mapping..
        memmap = new MapPlain();
    }

    return *memmap;
}

// *JiK*
Options::~Options()
{
    if (memmap) {
        delete memmap;
    }
}

Options::Options(int argc, char * * argv) :
    emitfunc(emitdefault),
    verbose(false),
    emitpublic(false),
    debugtype(Asm::NoDebug),
    redirecterr(false),
    nocase(false),
    autolocal(false),
    bracketonly(false),
    warn8080(false),
    mode86(false),
    werror(false),
    pass3(false),
    dumpmemmap(false),      // *JiK*
    memmap(NULL)            // *JiK*
{
    int argpos;
    for (argpos = 1; argpos < argc; ++argpos)
    {
        const string arg(argv [argpos] );
        if (arg == optbin)
            emitfunc = & Asm::emitobject;
        else if (arg == opthex)
            emitfunc = & Asm::emithex;
        else if (arg == optprl)
            emitfunc = & Asm::emitprl;
        else if (arg == optcmd)
            emitfunc = & Asm::emitcmd;
        else if (arg == optsdrel)
            emitfunc = & Asm::emitsdrel;
        else if (arg == optpass3)
            pass3 = true;
        else if (arg == optplus3dos) {
            memmap = new MapPlus3dos;           // *JiK*
            emitfunc = & Asm::emitplus3dos;
        } else if (arg == opttap) {
            memmap = new MapSpectrum48;         // *JiK*
            emitfunc = & Asm::emittap;
        } else if (arg == opttrs)
            emitfunc = & Asm::emittrs;
        else if (arg == opttzx) {
            memmap = new MapSpectrum48;         // *JiK*
            emitfunc = & Asm::emittzx;
        } else if (arg == optcdt)
            emitfunc = & Asm::emitcdt;
        else if (arg == opttapbas) {
            memmap = new MapSpectrum48;         // *JiK*
            emitfunc = & Asm::emittapbas;
        } else if (arg == opttapbas128) {       // *JiK*
            memmap = new MapSpectrum128;
            emitfunc = &Asm::emittapbas128;     // *JiK*
        } else if (arg == opttap128) {
            memmap = new MapSpectrum128;
            emitfunc = &Asm::emittap128;
        } else if (arg == opttzxbas)
            emitfunc = & Asm::emittzxbas;
        else if (arg == optcdtbas)
            emitfunc = & Asm::emitcdtbas;
        else if (arg == optamsdos)
            emitfunc = & Asm::emitamsdos;
        else if (arg == optmsx)
            emitfunc = & Asm::emitmsx;
        else if (arg == optpublic)
            emitpublic = true;
        else if (arg == optname)
        {
            ++argpos;
            if (argpos >= argc)
                throw NeedArgument(optname);
            headername = argv [argpos];
        }
        else if (arg == optmemmap)          // *JiK*
            dumpmemmap = true;
        else if (arg == optv)
            verbose = true;
        else if (arg == optd)
            debugtype = Asm::DebugSecondPass;
        else if (arg == opt1)
            debugtype = Asm::DebugAll;
        else if (arg == opterr)
            redirecterr = true;
        else if (arg == optnocase)
            nocase = true;
        else if (arg == optalocal)
            autolocal = true;
        else if (arg == optB || arg == optbracket)
            bracketonly = true;
        else if (arg == opt8 || arg == optw8080)
            warn8080 = true;
        else if (arg == opt86)
            mode86 = true;
        else if (arg == optwerror)
            werror = true;
        else if (arg == optI)
        {
            ++argpos;
            if (argpos >= argc)
                throw NeedArgument(optI);
            includedir.push_back(argv [argpos] );
        }
        else if (arg == optE || arg == optequ)
        {
            ++argpos;
            if (argpos >= argc)
                throw NeedArgument(arg);
            labelpredef.push_back(argv [argpos] );
        }
        else if (arg == "--")
        {
            ++argpos;
            break;
        }
        else if (arg.substr(0, 1) == "-")
            throw InvalidOption(arg);
        else
            break;
    }

    // File parameters.

    if (argpos >= argc)
        throw Usage();
    filein = argv [argpos];
    ++argpos;
    if (argpos >= argc)
        throw Usage();

    fileout = argv [argpos];
    ++argpos;

    if (argpos < argc)
    {
        filesymbol = argv [argpos];
        ++argpos;

        if (! emitpublic && argpos < argc)
        {
            filepublic = argv [argpos];
            ++argpos;
        }

        if (argpos < argc)
            cerr << "WARNING: Extra arguments ignored\n";
    }

    if (headername.empty() )
        headername = fileout;
}

string Options::getfilepublic() const
{
    if (emitpublic)
        return filesymbol;
    else
        return filepublic;
}

void Options::apply(Asm & assembler) const
{
    assembler.setdebugtype(debugtype);

    if (verbose)
        assembler.verbose();
    if (redirecterr)
        assembler.errtostdout();
    if (nocase)
        assembler.caseinsensitive();
    if (autolocal)
        assembler.autolocal();
    if (bracketonly)
        assembler.bracketonly();
    if (warn8080)
        assembler.warn8080 ();
    if (mode86)
        assembler.set86 ();
    if (werror)
        assembler.setwerror ();
    if (pass3)
        assembler.setpass3 ();

    for (size_t i = 0; i < includedir.size(); ++i)
        assembler.addincludedir(includedir [i] );

    for (size_t i = 0; i < labelpredef.size(); ++i)
        assembler.addpredef(labelpredef [i] );

    assembler.setheadername(headername);
}

int doit(int argc, char * * argv)           // *JiK*
{
    // Process command line options.

    Options option(argc, argv);

    if (option.redirerr() )
        perr = & cout;

    // Assemble.

    Asm assembler(option.getmemmap());      // *JiK*
    option.apply(assembler);

    assembler.loadfile(option.getfilein() );
    assembler.processfile();

    // Generate ouptut file.

    std::ofstream out(option.getfileout().c_str(),
        std::ios::out | std::ios::binary);
    if (! out.is_open() )
        throw runtime_error("Error creating object file");


    (assembler.* option.getemit() ) (out);

    out.close();

    // Generate symbol table and public symbol table if required.

    string filesymbol = option.getfilesymbol();
    if (! option.publiconly() && ! filesymbol.empty() )
    {
        std::ofstream sout;
        std::streambuf * cout_buf = 0;
        if (filesymbol != "-")
        {
            sout.open(filesymbol.c_str() );
            if (! sout.is_open() )
                throw runtime_error("Error creating symbols file");
            cout_buf = cout.rdbuf();
            cout.rdbuf(sout.rdbuf() );
        }
        assembler.dumpsymbol(cout);
        if (cout_buf)
        {
            cout.rdbuf(cout_buf);
            sout.close();
        }
    }

    const string filepublic = option.getfilepublic();
    if (! filepublic.empty() )
    {
        std::ofstream sout;
        std::streambuf * cout_buf = 0;
        if (filepublic != "-")
        {
            sout.open(filepublic.c_str() );
            if (! sout.is_open() )
                throw runtime_error("Error creating public symbols file");
            cout_buf = cout.rdbuf();
            cout.rdbuf(sout.rdbuf() );
        }
        assembler.dumppublic(cout);
        if (cout_buf)
        {
            cout.rdbuf(cout_buf);
            sout.close();
        }
    }

    if (option.getdumpmemmap())             // *JiK*
        option.getmemmap().dumpmapping();

    return 0;
}

} // namespace

static void usage(char *name)               // *JiK*
{
    *perr << "Usage: pasmo [options] file.asm file.bin [file.symbol [file.publics]]\n\n";
    *perr << "  Where 'file.asm' is the source file, 'file.bin' is the object\n" \
             "  file to be created and optionally 'file.symbol' is the file where\n" \
             "  the symbol table will be written and 'file.publics' is the file for\n" \
             "  the public symbols table.\n\n";

    *perr << "  Options are one of more of the following:\n";
    *perr << "    --help        This help output.\n";
    *perr << "    -d            Show debug info during second pass of assembly.\n";
    *perr << "    -1            Show debug info during both passes of assembly.\n";
    *perr << "    -8, --w8080   Show warnings when Z80 instructions that have no\n" \
             "                  equivalent in 8080 are used. Makes easy to write\n" \
             "                  programs for 8080 processor using Z80 asm syntax.\n";
    *perr << "    -v            Verbose mode.\n";
    *perr << "    -I path       Add <path> to search files for INCLUDE and INCBIN.\n";
    *perr << "    -B, --bracket Use bracket only mode. In this mode the parenthesis\n" \
             "                  are valid only in expressions, for indirections brackets\n" \
             "                  must be used.\n";
    *perr << "    -E, --equ     Predefine a symbol. Predefined symbol are treated in a\n" \
             "                  similar way as defineds with EQU.\n";
    //*perr << "    --usr0        Controls the SNA 128K port $7ffd ROM paging. Default is\n" \
    //         "                  ZX Spectrum 128K ROM and --usr0 selects ZX Spectrum 48K\n" \
    //         "                  Basic ROM.\n";
    *perr << "    --trdos       Controls the SNA 128K TR-DOS ROM paging. --trdos pages in\n"\
             "                  the TR-DOS ROM.\n";
    *perr << "    --public      Only the public symbols table is generated, using the\n" \
             "                  file.symbol name, file.symbol must not be specified when\n" \
             "                  using this option.\n";
    *perr << "    --name name   Name to put in the header in the formats that use it. If\n" \
             "                  unspecified the object file name will be used.\n";
    *perr << "    -err          Direct error messages to standard output instead of error\n" \
             "                  output (except for errors in options).\n";
    *perr << "    --nocase      Make identifiers case insensitive.\n";
    *perr << "    --alocal      Use autolocal mode. In this mode all labels that begins\n" \
             "                  with '_' are locals.\n";
    *perr << "    --memmap      Dumps the paged memory map information and details of all\n" \
             "                  banks.\n";
    *perr << "    --pass3       Set three pass assembling mode.\n"; 

    *perr << "  Code generation options:\n";
    *perr << "    --bin         Generate the object file in raw binary format.\n";
    *perr << "    --hex         Generate the object file in Intel HEX format.\n";
    *perr << "    --prl         Generate the object file in CP/M PRL format.\n";
    *perr << "    --cmd         Generate the object file in CP/M 86 CMD mode, using the\n" \
             "                  8080 memory model of CP/M 86. Used in conjuction with\n" \
             "                  the --86 option can easily generate CP/M 86 executables\n" \
             "                  from CP/M 80 sources with minimal changes.\n";
    *perr << "    --tap         Generates tap file with a code block, with the loading\n" \
             "                  position set to the beginnig of the code so you can load\n" \
             "                  it from Basic with a 'LOAD\"\" CODE' instruction.\n";
    *perr << "    --tzx         Same as --tap but using TZX format.\n";
    *perr << "    --cdt         Generate a cdt file with a code block, with the loading\n" \
             "                  position set to the beginning of the code and the start\n" \
             "                  address to the start point specified in the source, if\n" \
             "                  any, so you can use RUN to execute it or LOAD to load it.\n";
    *perr << "    --tapbas      Generates a TAP file with two parts: a Basic loader and a\n" \
             "                  code block with the object code. \n";
    *perr << "    --tap128      The same as --tapbas128 but without the Basic loader.\n";
    *perr << "    --tapbas128   Generates a TAP file with two or more parts: a Basic loader\n" \
             "                  with a banking support and one or more code blocks with the\n" \
             "                  object code and loads the code into appropriate banks.\n";
    *perr << "    --tzxbas      Same as --tapbas but using TZX format instead of TAP.\n";
    *perr << "    --cdtbas      Same as --tapbas but using CDT format instead of TAP and\n" \
             "                  with a Locomotive Basic loader instead of Spectrum Basic.\n";
    *perr << "    --plus3dos    Generates an object file in plus3dos format, used by the\n" \
             "                  Spectrum +3 disks.\n";
    *perr << "    --amsdos      Generates an object file with Amsdos header, used by the\n" \
             "                  Amstrad CPC on disk files.\n";
    *perr << "    --msx         Generates an object file with header for use with BLOAD in\n" \
             "                  MSX Basic.\n";
    //*perr << "    --sna         Generates an object file in SNA format.\n";
    //*perr << "    --sna128      Generates an object file in SNA 128K format.\n";
    *perr << "    --86          Generates 8086 code instead of Z80. This feature is\n" \
             "                  experimental.\n";
}


int main(int argc, char * * argv)
{
    //Asm assembler;

    // Call doit and show possible errors.
    try
    {
        //return doit(assembler, argc, argv);   // *JiK*
        return doit(argc, argv);                // *JiK*
    }
    catch (AsmError & err)
    {
        MemMap *mem = new MapPlain();           // *JiK*
        Asm assembler(*mem);                     // *JiK*
        assembler.showerrorinfo(* perr, err.getline(), err.message());
        delete mem;                             // *JiK*
    }
    catch (std::logic_error & e)
    {
        * perr << "ERROR: " << e.what() << '\n' <<
            "This error is unexpected, please "
            "send a bug report.\n";
    }
    catch (std::exception & e)
    {
        * perr << "ERROR: " << e.what() << '\n';
    }
    catch (Usage &)
    {
        cerr <<    "Pasmo v. " << pasmoversion <<
            " (C) 2004-2021 Julian Albo (2018-2023 Jouni Korhonen)\n\n"            // *JiK*
            "Usage:\n\n"
            "\tpasmo [options] source object [symbol]\n\n"
            "See the README file for details.\n";
    
            usage(argv[0]);
    }
    catch (ErrorAlreadyReported)
    {
        // What it says on the tin
    }
    catch (...)
    {
        cerr << "ERROR: Unexpected exception.\n"
            "Please send a bug report.\n";
    }

    // Added to fix Debian bug report #394733
    return 1;
}

// End
