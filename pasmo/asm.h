// asm.h

#include <iostream>
#include <string>

#include "pasmotypes.h"
#include "memmap.h"         // *JiK*

class Tokenizer;

class Asm
{
public:
    Asm(MemMap&);
    ~Asm();

    void verbose();
    enum DebugType { NoDebug, DebugSecondPass, DebugAll };
    void setdebugtype(DebugType type);
    void errtostdout();
    void setbase(address addr);
    void caseinsensitive();
    void autolocal();
    void bracketonly();
    void warn8080();
    void set86();
    void setpass(int npass);
    void setpass3();
    void setwerror();

    void showerrorinfo(std::ostream & os,
        size_t nline, const std::string message) const;

    void addincludedir(const std::string & dirname);
    void addpredef(const std::string & predef);
    const std::string & getheadername() const;
    void setheadername(const std::string & headername_n);

    void parseline(Tokenizer & tz);
    void loadfile(const std::string & filename);
    void processfile();

    void emitobject(std::ostream & out);
    void emitplus3dos(std::ostream & out);

    void emitpagedtap (std::ostream & out);     // *JiK*
    void emittapbas128 (std::ostream & out);    // *JiK*
    void emittap128 (std::ostream & out);       // *JiK*

    int getnumbanks() const;                    // *JiK*
    void setcurrentbank(int);                   // *JiK*
    bool iscurrentbankused() const;             // *JiK*


    void emittap(std::ostream & out);
    void emittrs(std::ostream & out);
    void emittzx(std::ostream & out);
    void emitcdt(std::ostream & out);

    void emittapbas(std::ostream & out);
    void emittzxbas(std::ostream & out);
    void emitcdtbas(std::ostream & out);

    void emithex(std::ostream & out);
    void emitamsdos(std::ostream & out);

    void emitprl(std::ostream & out);
    void emitcmd(std::ostream & out);

    void emitsdrel(std::ostream & out);

    void emitmsx(std::ostream & out);
    void dumppublic(std::ostream & out);
    void dumpsymbol(std::ostream & out);

    const byte * getmem() const;
    byte peekbyte(address addr) const;
    address peekword(address addr) const;
    address getvalue(const std::string & var);
    address getcodesize() const;
    address getminused() const;
    bool hasentrypoint() const;
    address getentrypoint() const;
    void writebincode(std::ostream & out) const;
private:
    Asm(const Asm & a); // Forbidden
    void operator = (const Asm &); // Forbidden
public:
    // Make it public to simplify implementation.
    class In;
    friend class In;
private:
    In * pin;
};

// End
