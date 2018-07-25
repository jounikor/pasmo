// memmap.h
// revision 8-Jul-2018

#ifndef PASMO_SRC_MEMMAP_H_
#define PASMO_SRC_MEMMAP_H_

#include "pasmotypes.h"

#include <vector>

//

struct Page {
    bool rom;
    bool isbanked;
    bool gotpaged;
    address base;
    int bank;
};

class Bank {
private:
    byte mem_[16384];   // shortcut.. force 16K pages.
    address minused_;
    address maxused_;
    bool recordaccess_;
public:
    static const int banksize;
    ~Bank();
    Bank();
    byte& operator[](address);
    void init();
    address getminused() const { return minused_; }
    address getmaxused() const { return maxused_; }
    void stopaccessrecording() { recordaccess_ = false; }
    void suspendaccessrecording() { recordaccess_ = false; }
    void resumeaccessrecording() { recordaccess_ = true; }
};

class MemMap {
private:
    Page pages_[4];
    std::vector<Bank> banks_;
    bool gotpaged_;
    int currentbank_;
    byte* mem;
public:
    virtual ~MemMap();
    MemMap(int=8);
    void initmap(int,int,bool=false,bool=false);
    void setcurrentbank(int);
    int getcurrentbank() const;
    bool gotpaged() const { return gotpaged_; }
    virtual void setmapping(int);
    int getnumbanks() const;
    address getcurrentbankminused() const;
    address getcurrentbankmaxused() const;
    bool iscurrentbankused() const;
    byte& operator[](address);
    byte* const operator+(int); 
    void dumpmapping() const;
};

// target specific classes

class MapPlain : public MemMap {
private:
public:
    MapPlain();
    ~MapPlain();
};

class MapSpectrum48 : public MemMap {
private:
public:
    MapSpectrum48();
    ~MapSpectrum48();
};

class MapSpectrum128 : public MemMap {
private:
public:
    MapSpectrum128();
    ~MapSpectrum128();
};

class MapPlus3dos : public MemMap {
private:
public:
    MapPlus3dos();
    ~MapPlus3dos();
    void setmapping(int);
};

#endif  // PASMO_SRC_MEMMAP_H_


