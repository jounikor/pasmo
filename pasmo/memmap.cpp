// memmap.cpp
// revision

#include "pasmotypes.h"
#include "memmap.h"

#include <iostream>
#include <vector>
#include <stdexcept>
#include <cstring>

//

using std::runtime_error;
using std::logic_error;

// exceptions..

logic_error MemMapPages("Memory map slots occypy more than 64K of RAM");
logic_error MemMapBanks("Unsupported memory map bank size");
logic_error BankOutOfBounds("Invalid memory bank index");

runtime_error MemMapInvalidBank("Current bank index not set");
runtime_error MemMapROMAccess("Access to a ROM page");
runtime_error MemMapConverted("Memory map already converted to a flat mem array.");
runtime_error EmitPagedAsPlain("Accessing paged memory map as a flat mem array is not possible. Check for BANK statements.");

//
const int Bank::banksize = 1<<14;   // 16384

void Bank::init()
{
    minused_ = 0xffff;
    maxused_ = 0x0000;
    recordaccess_ = true;
}

Bank::Bank() 
{
    init();
}

byte& Bank::operator[](address addr)
{
    if (recordaccess_) {
        if (addr > maxused_) {
            maxused_ = addr;
        } else if (addr < minused_) {
            minused_ = addr;
        }
    }
    if (addr >= banksize) {
        addr &= (banksize-1);
    }
    return mem_[addr];
}

Bank::~Bank()
{ }


int MemMap::getnumbanks() const
{
    return banks_.size();
}

address MemMap::getcurrentbankminused() const
{
#if 1
    if (mem != NULL) {
        throw MemMapConverted;
    }
#endif
    if (currentbank_ < 0) {
        throw MemMapInvalidBank;
    }
    return banks_[currentbank_].getminused();
}

address MemMap::getcurrentbankmaxused() const
{
#if 1
    if (mem != NULL) {
        throw MemMapConverted;
    }
#endif
    if (currentbank_ < 0) {
        throw MemMapInvalidBank;
    }
    return banks_[currentbank_].getmaxused();
}

bool MemMap::iscurrentbankused() const
{
#if 1
    if (mem != NULL) {
        throw MemMapConverted;
    }
#endif
    if (currentbank_ < 0) {
        throw MemMapInvalidBank;
    }
    return banks_[currentbank_].getmaxused() >= banks_[currentbank_].getminused();
}

byte& MemMap::operator[](address addr)
{
    int page = addr / Bank::banksize;
    int bank;	

#if 1
    if (mem != NULL) {
        throw MemMapConverted;
    }
#endif
    if (pages_[page].rom == true) {
        throw MemMapROMAccess;
    }
    if (pages_[page].isbanked) {
        if (currentbank_ < 0) {
            currentbank_ = pages_[page].bank;
        }
        if (currentbank_ != pages_[page].bank) {
            pages_[page].gotpaged = true;
            pages_[page].bank = currentbank_;
            gotpaged_ = true;
        }
    }
    bank = pages_[page].bank;
    return banks_[bank][addr];
}

byte* const MemMap::operator+(int pos)
{
    int n, i = banks_.size(); 
    int j, p = 0;

    if (mem == NULL) {
        if (gotpaged_) {
            throw EmitPagedAsPlain;
        }

        mem = new byte[Bank::banksize*4];

        for (n=0; n < 4; n++) {
            if (!pages_[n].rom) {
                j = pages_[n].bank;

                if (j < 0 || j > i) {
                    throw BankOutOfBounds;
                }  

                Bank& bank = banks_[j];
                bank.suspendaccessrecording();     
    
    	        for (j=0; j < Bank::banksize; j++) {
                    mem[p++] = bank[j];
                }
                bank.resumeaccessrecording();     
            } else {
                for (j=0; j < Bank::banksize; j++) {
                    mem[p++] = 0;
                }
            }
        }
    }
    return mem+pos;
}


void MemMap::setcurrentbank(int bank) {
    currentbank_ = bank;
}

int MemMap::getcurrentbank() const {
    return currentbank_;
}

MemMap::MemMap(int banks) : 
    banks_(banks)
{
    gotpaged_ = false;
    currentbank_ = -1;   // invalid bank..
    mem = NULL;
}

MemMap::~MemMap() 
{
    if (mem != NULL) {
        delete mem;
    }
}

void MemMap::initmap(int page, int bank, bool banked, bool rom)
{
    pages_[page].base = page*Bank::banksize;
    pages_[page].rom = rom;
    pages_[page].isbanked = banked;
    pages_[page].bank = bank;
    pages_[page].gotpaged = false;
}


void MemMap::setmapping(int map)
{ }

void MemMap::dumpmapping() const
{
    int page, bank;

    std::cout <<     "\nMemory mapping informartion ------------------\n";
    std::cout <<     "  Number of banks: " << banks_.size() << "\n";
    std::cout <<     "  pages used:      " << gotpaged_ << "\n\n";

    for (page = 0; page < 4; page++) {
        std::cout << "Page " << page << " --------------------------\n";
        std::cout << " base address: " << hex4(pages_[page].base) << "\n";  
        std::cout << " rom:          " << pages_[page].rom << "\n";  
        std::cout << " isbanked:     " << pages_[page].isbanked << "\n";  
        std::cout << " gotpaged:     " << pages_[page].gotpaged << "\n";  
        std::cout << " bank:         " << pages_[page].bank << "\n\n";  
    }

    for (bank = 0; bank < banks_.size(); bank++) {
        std::cout << "Bank " << bank << " --------------------------\n";
        std::cout << " minused address: " << hex4(banks_[bank].getminused()) << "\n";  
        std::cout << " maxused address: " << hex4(banks_[bank].getmaxused()) << "\n\n";  
    }
}

//
//
// Linear 64K RAM memory map without paging..

MapPlain::MapPlain() : 
    MemMap(4)
{
    // Initialize simple 64K RAM without paging
    for (int i = 0; i < 4; i++) {
        initmap(i,i);
    }
}

MapPlain::~MapPlain()
{ }

// ZX Spectrun48 memory map.. this actually mimics party 128K memory layout

MapSpectrum48::MapSpectrum48() : 
    MemMap(6)
{
    initmap(0,-1,false,true);   // ROM
    initmap(1,5);
    initmap(2,2);
    initmap(3,0);
}

MapSpectrum48::~MapSpectrum48()
{ }

// ZX Spectrum128K memory map..

MapSpectrum128::MapSpectrum128() :
    MemMap(8)
{
    initmap(0,-1,false,true);   // ROM
    initmap(1,5);
    initmap(2,2);
    initmap(3,0,true);          // PAGE3 is banked
    setcurrentbank(0);
}

MapSpectrum128::~MapSpectrum128()
{ }

// ZX Spectrum+3 memory map..

MapPlus3dos::MapPlus3dos() :
    MemMap(8)
{
    setmapping(-1);
}

MapPlus3dos::~MapPlus3dos()
{ }

void MapPlus3dos::setmapping(int mapping)
{
    switch (mapping) {
    case 0:
        initmap(0,0);
        initmap(1,1);
        initmap(2,2);
        initmap(3,3);
        break;
    case 1:
        initmap(0,4);
        initmap(1,5);
        initmap(2,6);
        initmap(3,7);
        break;
    case 2:
        initmap(0,4);
        initmap(1,5);
        initmap(2,6);
        initmap(3,3);
        break;
    case 3:
        initmap(0,4);
        initmap(1,7);
        initmap(2,6);
        initmap(3,3);
        break;
    default:
        initmap(0,-1,false,true);   // ROM
        initmap(1,5);
        initmap(2,2);
        initmap(3,0,true);          // PAGE3 is banked
        break;
    }
}
