// spectrum.cxx

#include "spectrum.h"
#include "asm.h"

#include <sstream>
#include <algorithm>

using std::fill;
using std::copy;

//**************************************************************

spectrum::Plus3Head::Plus3Head()
{
    static const byte ident[] =
    {
        'P', 'L', 'U', 'S', '3', 'D', 'O', 'S', // Identifier.
        0x1A, // CP/M EOF.
        1,    // Issue number.
        0,    // Version number.
    };
    fill(plus3, plus3 + headsize, byte(0) );
    copy(ident, ident + sizeof(ident), plus3);

    plus3[15]= 3; // Type: code.

    plus3[20]= 0x80; // Don't know if used for something.
    plus3[21]= 0x80;
}

void spectrum::Plus3Head::setsize(address size)
{
    size_t filesize = size + 128;

    // Size of file including code and header.
    plus3[11]= filesize & 0xFF;
    plus3[12]= (filesize >> 8) & 0xFF;
    plus3[13]= (filesize >> 16) & 0xFF;
    plus3[14]= (filesize >> 24) & 0xFF;

    // Size of code.
    plus3[16]= lobyte(size);
    plus3[17]= hibyte(size);
}

void spectrum::Plus3Head::setstart(address start)
{
    // Start address.
    plus3[18]= lobyte(start);
    plus3[19]= hibyte(start);
}

void spectrum::Plus3Head::write(std::ostream & out)
{
    // Checksum
    byte check = 0;
    for (int i = 0; i < 127; ++i)
        check += plus3[i];
    plus3[127]= check;

    // Write the header.
    out.write(reinterpret_cast <char *>(plus3), headsize);
}

//**************************************************************

// Spectrum Basic generation.

namespace spectrum
{

const std::string tokNumPrefix (1, '\x0E');
const std::string tokEndLine   (1, '\x0D');
const std::string tokCODE      (1, '\xAF');
const std::string tokUSR       (1, '\xC0');
const std::string tokLOAD      (1, '\xEF');
const std::string tokPOKE      (1, '\xF4');
const std::string tokRANDOMIZE (1, '\xF9');
const std::string tokCLEAR     (1, '\xFD');

// *JiK* start
const std::string tokREM       (1, '\xEA');
const std::string tokPEEK      (1, '\xBE');
const std::string tokREAD      (1, '\xE3');
const std::string tokDATA      (1, '\xE4');
const std::string tokLET       (1, '\xF1');
const std::string tokSTOP      (1, '\xE2');
const std::string tokVAL       (1, '\xB0');
const std::string tokFOR       (1, '\xEB');
const std::string tokTO        (1, '\xCC');
const std::string tokNEXT      (1, '\xF3');
const std::string tokIF        (1, '\xFA');
const std::string tokTHEN      (1, '\xCB');
const std::string tokGOTO      (1, '\xEC');
const std::string tokGE        (1, '\xC8');
const std::string tokPRINT     (1, '\xF5');
// *JiK* end



std::string number(address n)
{
    std::ostringstream oss;
    oss << n;
    std::string str(oss.str() );
    str+= tokNumPrefix;

    // Special format of Spectrum numbers for integers.
    str+= '\x00';
    str+= '\x00';
    str+= static_cast <unsigned char> (lobyte(n) );
    str+= static_cast <unsigned char> (hibyte(n) );
    str+= '\x00';
    return str;
}

// *JiK* start
std::string VALnumber (address n)
{
    std::ostringstream oss;
    oss << n;
    //std::string str (oss.str () );
    std::string str;

    str += tokVAL;
    str += '\"';
    str += oss.str();
    str += '\"';
    return str;
}
// *JiK* end


std::string linenumber(address n)
{
    std::string str(1, hibyte(n) );
    str+= lobyte(n);
    return str;
}

std::string linelength(address n)
{
    std::string str(1, lobyte(n) );
    str+= hibyte(n);
    return str;
}

std::string basicline(address linenum, const std::string & line)
{
    std::string result(linenumber(linenum) );
    result+= linelength(static_cast <address> (line.size() ) + 1);
    result+= line;
    result+= tokEndLine;
    return result;
}


std::string basicloader(const Asm & as)
{
    const address minused = as.getminused();
    std::string basic;

    // Line: 10 CLEAR before_min_used
    std::string line = tokCLEAR + number(minused - 1);
    basic+= basicline(10, line);

    // Line: 20 POKE 23610, 255
    // To avoid a error message when using +3 loader.
    line = tokPOKE + number(23610) + ',' + number(255);
    basic+= basicline(20, line);

    // Line: 30 LOAD "" CODE
    line = tokLOAD + "\"\"" + tokCODE;
    basic+= basicline(30, line);

    if (as.hasentrypoint())
    {
        // Line: 40 RANDOMIZE USR entry_point
        line = tokRANDOMIZE + tokUSR + number(as.getentrypoint());
        basic+= basicline(40, line);
    }

    return basic;
}

// *JiK* start

std::string pagedbasicloader (Asm & as)        // TODO
{
#if 0
        23608 -> lenght of a warning buzz (used to pass parameters)
        23388 -> last page address
        23635 -> address of basic program

        DI                   243
        LD  A,(23388)        58,92,91
        AND $F8              230,248
        LD  B,A              71
        LD  A,(23608)        58,56,92
        OR  B                176
        LD  (23388), A       50,92,91
        LD  BC, $7FFD        1,253,127
        OUT (C), A           237,121
        EI                   251
        RET                  201  -> 21 kpl
#endif
        std::string basic;
        int bank;
        const address minused = as.getminused();

        // 1 REM 012345678901234567890123456
        //std::string line = tokREM + "012345678901234567890123456";
        std::string line = tokREM + char(243) + char(58) + char(92) + char(91) \
                + char(230) + char(248) +  char(71) +  char(58) +  char(56) +  char(92) \
                + char(176) +  char(50) +  char(92) +  char(91) +   char(1) + char(253) \
                + char(127) + char(237) + char(121) + char(251) + char(201);

        basic+= basicline (1, line);

        // 10 CLEAR VAL "00000": LET c=(PEEK VAL "23635"+VAL "256"*PEEK VAL "23636")+VAL "5"
        line = tokCLEAR + VALnumber (minused-1) + ':' + tokLET + "c=(" + tokPEEK + VALnumber (23635) \
               + '+' + VALnumber(256) + '*' + tokPEEK + VALnumber(23636) + ")+" + VALnumber(5);
        basic+= basicline (10, line);

        // 20 LET a=PEEK VAL "23608"
        line = tokLET + "a=" + tokPEEK + VALnumber(23608);
        basic += basicline(20,line);

        // 40 READ b
        line = tokREAD + 'b';
        basic += basicline(40,line);

        // 50 IF b < VAL"8" THEN POKE VAL "23608",b: RANDOMIZE USR c: LOAD "" CODE
        line = tokIF + "b<" + VALnumber(8) + tokTHEN + tokPOKE +  VALnumber(23608) + ",b:" \
                + tokRANDOMIZE + tokUSR + "c:" \
                + tokLOAD + "\"\"" + tokCODE;
        basic += basicline(50,line);

        // 60 IF b = VAL"8" THEN RANDOMIZE USR VAL "00000",a: STOP
        line = tokIF + "b=" + VALnumber(8) + tokTHEN + tokPOKE + VALnumber(23608) + ",a:";

        if (as.hasentrypoint()) {
            // Line: 40 RANDOMIZE USR entry_point
            line = line + tokRANDOMIZE + tokUSR + VALnumber(as.getentrypoint()) + ':';
        }

        line += tokSTOP;
        basic += basicline(60,line);

        // 70 GO TO VAL "40"
        line = tokGOTO + VALnumber(40);
        basic += basicline(70,line);

        // output as many data entries as there are banks.. there is atleast one..
        // 110 DATA VAL "0", ...
        line = tokDATA;

        // This is  a bit ugly since we need MemMap information that is located
        // in Asm::In as a private member. Therefore, we have a wrapper methods
        // in Asm:: to call methods in Asm::In..

        for (bank = 0;  bank < as.getnumbanks(); bank++) {
            as.setcurrentbank(bank);

            if (as.iscurrentbankused()) {
                line += VALnumber(bank);
                line += ',';
            }
        }

        // end mark VAL "8"
        line += VALnumber(8);
        basic += basicline(110,line);

        return basic;
}


// *JiK* end

} // namespace spectrum


// End
