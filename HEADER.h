#pragma once

constexpr int MEM_SIZE = 0x100;

#include <QString>
#include <QObject>
#include <QProcess>
#include <QDebug>
#include <QFile>
#include <QProcess>
#include <QCoreApplication>
#include <QTime>
#include <QDate>

using LWORD = unsigned __int128;    //128 bit
using QWORD = unsigned long long;   //64  bit
using DWORD = unsigned int;         //32  bit
using WORD = unsigned short;        //16  bit
using BYTE = unsigned char;         //8   bit


union Register64{
    QWORD   Reg64;
    DWORD   Reg32[2];
    WORD    Reg16[4];
    BYTE    Reg8[8];

    Register64(){
        this->Reg64 = 0;
    }
};

union Register32{
    DWORD   Reg32;
    WORD    Reg16[2];
    BYTE    Reg8[4];

    Register32(){
        this->Reg32 = 0;
    }
};
