#ifndef INCLUDE_SPECTRUM_H
#define INCLUDE_SPECTRUM_H

// spectrum.h
// Revision 5-dec-2004

#include "pasmotypes.h"

#include <string>

namespace spectrum {

class Plus3Head {
public:
	Plus3Head ();
	void setsize (address size);
	void setstart (address start);
	void write (std::ostream & out);
private:
	static const size_t headsize= 128;
	byte plus3 [headsize];
};


// Spectrum Basic generation.


extern const std::string tokNumPrefix;
extern const std::string tokEndLine;
extern const std::string tokCODE;
extern const std::string tokUSR;
extern const std::string tokLOAD;
extern const std::string tokPOKE;
extern const std::string tokRANDOMIZE;
extern const std::string tokCLEAR;
extern const std::string tokREM;
extern const std::string tokPEEK;
extern const std::string tokREAD;
extern const std::string tokDATA;
extern const std::string tokLET;
extern const std::string tokSTOP;
extern const std::string tokVAL;
extern const std::string tokFOR;
extern const std::string tokTO;
extern const std::string tokNEXT;
extern const std::string tokIF;
extern const std::string tokTHEN;
extern const std::string tokGOTO;
extern const std::string tokGE;

std::string number (address n);
std::string VALnumber (address n);
std::string linenumber (address n);
std::string linelength (address n);
std::string basicline (address linenum, const std::string & line);


} // namespace spectrum


#endif

// End of spectrum.h
