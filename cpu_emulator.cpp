#include "cpu_emu.h"

using namespace std;

CPU::CPU(QObject* p) :
    QObject{p}
{
    reset();
}


int CPU::readFromFile(QString name){
    QFile file(name);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return -1;
    this->m_memory = file.readAll();
    this->m_memory.resize(MEM_SIZE);
    return 0;
}

void CPU::reset(){
    this->m_A.Reg32 =0;
    this->m_B.Reg32 =0;
    this->m_C.Reg32 =0;
    this->m_D.Reg32 =0;
    this->m_SP.Reg32 =MEM_SIZE-1;
    this->m_IP.Reg32 =0;

    //FLAGS
    this-> m_FZ= 0;
    this-> m_FO= 0;
    this-> m_FP= 0;

    //Common use registers
    this-> m_R0.Reg64 = 0;
    this-> m_R1.Reg64 = 0;
    this-> m_R2.Reg64 = 0;
    this-> m_R3.Reg64 = 0;
    this-> m_R4.Reg64 = 0;
    this-> m_R5.Reg64 = 0;
}

QString CPU::getOpenEditor(QString exePath, QString filePath){
    this->editor = new  QProcess(this);
    editor->start(exePath,QStringList(filePath));
    return filePath;
}

QString CPU::getOpenEditor(QString exePath){
    this->editor = new  QProcess(this);
    QString file = QCoreApplication::applicationDirPath();
    QString time = QTime::currentTime().toString("hh_mm");
    QString date = QDate::currentDate().toString("_dd_MM");
    file += "//" + time + date + ".prm";
    editor->start(exePath,QStringList(file));
    return file;

}

void CPU::getDataFromFile(QString name){
    QFile file(name);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    this->m_memory = file.readAll();
    reset();
    this->m_memory.resize(MEM_SIZE);
    return;
}



bool CPU::tick(){
    QByteArray curInstr[4][4];
    try {
        int i,j,k;
        k = static_cast<int>(m_IP.Reg32);
        if(k > MEM_SIZE)
            return false;
        for(i=0;i<4;i++){
            for(j=0;j<4;j++){
                curInstr[i][j] += m_memory.at(k+i*4+j);
            }
        }

    } catch (std::out_of_range){
        qDebug() << "\n\nProgram has ended incorrectly\nError code:out_of_range\n";
        return false;
    }
    m_IP.Reg32 = m_IP.Reg32 + 0x10;
    for(int prs = 0; prs < 4; prs++){
        uchar com,op1,op2h,op2l;
        ushort op2;
        com = static_cast<uchar>(curInstr[prs][0][0]);
        op1 = static_cast<uchar>(curInstr[prs][1][0]);
        op2h = static_cast<uchar>(curInstr[prs][2][0]);
        op2l = static_cast<uchar>(curInstr[prs][3][0]);
        op2 = static_cast<ushort>((op2h << 8) + op2l);

        QWORD dest,src,rez;
        switch (com) {
        case 0x01: // add
            dest = getRegisterValue(op1);
            src = getRegisterValue(op2);
            rez = dest + src;
            setRegisterValue(op1,rez);
            if (rez == 0)
                m_FZ = 1;
            break;
        case 0x02: //addn
            dest = getRegisterValue(op1);
            src = op2;
            rez = dest + src;
            setRegisterValue(op1,rez);
            if (rez == 0)
                m_FZ = 1;
            break;
        case 0x03: //addm
            dest = getRegisterValue(op1);
             src = m_memory.mid(static_cast<int>(op2),4).toHex().toULongLong(nullptr,16);
            rez = dest + src;
            setRegisterValue(op1,rez);
            if (rez == 0)
                m_FZ = 1;
            break;
        case 0x04: // sub
            dest = getRegisterValue(op1);
            src = getRegisterValue(op2);
            rez = dest - src;
            setRegisterValue(op1,rez);
            if (rez == 0)
                m_FZ = 1;
            break;
        case 0x05: //subn
            dest = getRegisterValue(op1);
            src = op2;
            rez = dest - src;
            setRegisterValue(op1,rez);
            if (rez == 0)
                m_FZ = 1;
            break;
        case 0x06: //subm
            dest = getRegisterValue(op1);
             src = m_memory.mid(static_cast<int>(op2),4).toHex().toULongLong(nullptr,16);
            rez = dest - src;
            setRegisterValue(op1,rez);
            if (rez == 0)
                m_FZ = 1;
            break;

        case 0x07: //mul
            dest = m_A.Reg32;
            src = getRegisterValue(op2);
            rez = dest * src;
            if (rez > 0x0000FFFF){
                m_FP = 1;
            }
            if (rez == 0)
                m_FZ = 1;
            m_A.Reg32 = static_cast<DWORD>(rez);
            break;

        case 0x08: //div
            dest = m_A.Reg32;
            src = getRegisterValue(op2);
            if(src == 0){
                qDebug() << "Попытка деления на 0\n";
                return false;
            }
            m_A.Reg32 = static_cast<DWORD>(dest / src);
            m_D.Reg32 = static_cast<DWORD>(dest % src);
            break;

        case 0x10: //jmp
            m_IP.Reg32 = op2;
            break;
        case 0x11: //jz
            if (m_FZ == 1)
                m_IP.Reg32 = op2;
            break;
        case 0x12: //jnz
            if (m_FZ == 0)
                m_IP.Reg32 = op2;
            break;
        case 0x13: //jo
            if (m_FO == 1)
                m_IP.Reg32 = op2;
            break;
        case 0x14: //jno
            if (m_FO == 0)
                m_IP.Reg32 = op2;
            break;
        case 0x15: //jp
            if (m_FP == 1)
                m_IP.Reg32 = op2;
            break;
        case 0x16: //jnp
            if (m_FP == 0)
                m_IP.Reg32 = op2;
            break;

        case 0x20: // mov
            dest = getRegisterValue(op1);
            src = getRegisterValue(op2);
            setRegisterValue(op1,src);
            break;
        case 0x21: //movn
            dest = getRegisterValue(op1);
            src = op2;
            setRegisterValue(op1,src);
            break;
        case 0x22:{ //movr
            dest = getRegisterValue(op1);
            uchar bytes[4];
            bytes[0] = (dest >> 24) & 0xFF;
            bytes[1] = (dest >> 16) & 0xFF;
            bytes[2] = (dest >> 8) & 0xFF;
            bytes[3] = dest & 0xFF;
            int addr = static_cast<int>(op2);
            for(int i = 0; i<4; i++){
                m_memory[addr+i] = static_cast<char>(bytes[i]);
            }
            break;
        }

       case 0x23:{ //movm
            int addr = static_cast<int>(op2);
            src = 0;
            uchar t;
            for(int i = 0; i<4; i++){
                t = static_cast<uchar>(m_memory[addr+i]);
                src += t;
                if(i!=3){
                    src <<=8;
                }
            }
            setRegisterValue(op1,src);
            break;

        }
        case 0x31:{ //push
            if(m_SP.Reg32 <= MEM_SIZE - 0xAF){
                qDebug() << "Переполнение стека";
                return false;
            }
            dest = getRegisterValue(op1);
            uchar bytes[4];
            bytes[0] = (dest >> 24) & 0xFF;
            bytes[1] = (dest >> 16) & 0xFF;
            bytes[2] = (dest >> 8) & 0xFF;
            bytes[3] = dest & 0xFF;
            int addr = m_SP.Reg16[0];
            m_SP.Reg32 -= 4;
            for(int i = 0; i<4; i++){
                m_memory[addr+i] = static_cast<char>(bytes[i]);
            }
            break;
        }
        case 0x32:{ //pop
            if(m_SP.Reg32 == MEM_SIZE){
                qDebug() << "Попытка достать из пустого стека";
                return false;
            }
            m_SP.Reg32 += 4;
            int addr = m_SP.Reg16[0];
            src = 0;
            uchar t;
            for(int i = 0; i<4; i++){
                t = static_cast<uchar>(m_memory[addr+i]);
                src += t;
                if(i!=3){
                    src <<=8;
                }
            }
            setRegisterValue(op1,src);
            break;
        }

        case 0x40: // and
            dest = getRegisterValue(op1);
            src = getRegisterValue(op2);
            rez = dest & src;
            setRegisterValue(op1,rez);
            if (rez == 0)
                m_FZ = 0;
            break;
        case 0x41: // or
            dest = getRegisterValue(op1);
            src = getRegisterValue(op2);
            rez = dest | src;
            setRegisterValue(op1,rez);
            if (rez == 0)
                m_FZ = 0;
            break;
        case 0x42: // xor
            dest = getRegisterValue(op1);
            src = getRegisterValue(op2);
            rez = dest ^ src;
            setRegisterValue(op1,rez);
            if (rez == 0)
                m_FZ = 0;
            break;
        case 0x43: // not
            dest = getRegisterValue(op1);
            rez = ~ dest;
            setRegisterValue(op1,rez);
            if (rez == 0)
                m_FZ = 0;
            break;

        default:
            break;
        }
    }

    return true;
}


void CPU::setRegisterValue(uint n, QWORD v){
         if(n == 0x10) m_A.Reg16[0] = static_cast<WORD>(v);
    else if(n == 0x11) m_A.Reg32 = static_cast<DWORD>(v);
    else if(n == 0x12) m_A.Reg8[1] = static_cast<BYTE>(v);
    else if(n == 0x13) m_A.Reg8[0] = static_cast<BYTE>(v);

    else if(n == 0x20) m_B.Reg16[0] = static_cast<WORD>(v);
    else if(n == 0x21) m_B.Reg32 = static_cast<DWORD>(v);
    else if(n == 0x22) m_B.Reg8[1] = static_cast<BYTE>(v);
    else if(n == 0x23) m_B.Reg8[0] = static_cast<BYTE>(v);

    else if(n == 0x30) m_C.Reg16[0] = static_cast<WORD>(v);
    else if(n == 0x31) m_C.Reg32 = static_cast<DWORD>(v);
    else if(n == 0x32) m_C.Reg8[1] = static_cast<BYTE>(v);
    else if(n == 0x33) m_C.Reg8[0] = static_cast<BYTE>(v);

    else if(n == 0x40) m_D.Reg16[0] = static_cast<WORD>(v);
    else if(n == 0x41) m_D.Reg32 = static_cast<DWORD>(v);
    else if(n == 0x42) m_D.Reg8[1] = static_cast<BYTE>(v);
    else if(n == 0x43) m_D.Reg8[0] = static_cast<BYTE>(v);

    else if(n == 0x50) m_SP.Reg16[0] = static_cast<WORD>(v);
    else if(n == 0x51) m_SP.Reg32 = static_cast<DWORD>(v);

    else if(n == 0x90) m_R0.Reg32[0] = static_cast<DWORD>(v);
    else if(n == 0x91) m_R0.Reg64 = static_cast<QWORD>(v);

    else if(n == 0xA0) m_R1.Reg32[0] = static_cast<DWORD>(v);
    else if(n == 0xA1) m_R1.Reg64 = static_cast<QWORD>(v);

    else if(n == 0xB0) m_R2.Reg32[0] = static_cast<DWORD>(v);
    else if(n == 0xB1) m_R2.Reg64 = static_cast<QWORD>(v);

    else if(n == 0xC0) m_R3.Reg32[0] = static_cast<DWORD>(v);
    else if(n == 0xC1) m_R3.Reg64 = static_cast<QWORD>(v);

    else if(n == 0xD0) m_R4.Reg32[0] = static_cast<DWORD>(v);
    else if(n == 0xD1) m_R4.Reg64 = static_cast<QWORD>(v);

    else if(n == 0xE0) m_R5.Reg32[0] = static_cast<DWORD>(v);
    else if(n == 0xE1) m_R5.Reg64 = static_cast<QWORD>(v);

}

QWORD CPU::getRegisterValue(uint n){
         if(n == 0x10) return m_A.Reg16[0];
    else if(n == 0x11) return m_A.Reg32;
    else if(n == 0x12) return m_A.Reg8[1];
    else if(n == 0x13) return m_A.Reg8[0];

    else if(n == 0x20) return m_B.Reg16[0];
    else if(n == 0x21) return m_B.Reg32;
    else if(n == 0x22) return m_B.Reg8[1];
    else if(n == 0x23) return m_B.Reg8[0];

    else if(n == 0x30) return m_C.Reg16[0];
    else if(n == 0x31) return m_C.Reg32;
    else if(n == 0x32) return m_C.Reg8[1];
    else if(n == 0x33) return m_C.Reg8[0];

    else if(n == 0x40) return m_D.Reg16[0];
    else if(n == 0x41) return m_D.Reg32;
    else if(n == 0x42) return m_D.Reg8[1];
    else if(n == 0x43) return m_D.Reg8[0];

    else if(n == 0x50) return m_SP.Reg16[0];
    else if(n == 0x51) return m_SP.Reg32;

    else if(n == 0x90) return m_R0.Reg32[0];
    else if(n == 0x91) return m_R0.Reg64;

    else if(n == 0xA0) return m_R1.Reg32[0];
    else if(n == 0xA1) return m_R1.Reg64;

    else if(n == 0xB0) return m_R2.Reg32[0];
    else if(n == 0xB1) return m_R2.Reg64;

    else if(n == 0xC0) return m_R3.Reg32[0];
    else if(n == 0xC1) return m_R3.Reg64;

    else if(n == 0xD0) return m_R4.Reg32[0];
    else if(n == 0xD1) return m_R4.Reg64;

    else if(n == 0xE0) return m_R5.Reg32[0];
    else if(n == 0xE1) return m_R5.Reg64;
    return  0;

}

LWORD CPU::getRegisterValue(QString n){
    if (n == "AL")     { return m_A.Reg8[0];}
    if (n == "AH")     { return m_A.Reg8[1];}
    if (n == "AX")     { return m_A.Reg16[0];}
    if (n == "EAX")    { return m_A.Reg32;}

    if (n == "BL")     { return m_B.Reg8[0];}
    if (n == "BH")     { return m_B.Reg8[1];}
    if (n == "BX")     { return m_B.Reg16[0];}
    if (n == "EBX")    { return m_B.Reg32;}

    if (n == "CL")     { return m_C.Reg8[0];}
    if (n == "CH")     { return m_C.Reg8[1];}
    if (n == "CX")     { return m_C.Reg16[0];}
    if (n == "ECX")    { return m_C.Reg32;}

    if (n == "DL")     { return m_D.Reg8[0];}
    if (n == "DH")     { return m_D.Reg8[1];}
    if (n == "DX")     { return m_D.Reg16[0];}
    if (n == "EDX")    { return m_D.Reg32;}

    if (n == "SP")     { return m_SP.Reg16[0];}
    if (n == "ESP")    { return m_SP.Reg32;}

    if (n == "R0")     { return m_R0.Reg32[0];}
    if (n == "ER0")    { return m_R0.Reg64;}

    if (n == "R1")     { return m_R1.Reg32[0];}
    if (n == "ER1")    { return m_R1.Reg64;}

    if (n == "R2")     { return m_R2.Reg32[0];}
    if (n == "ER2")    { return m_R2.Reg64;}

    if (n == "R3")     { return m_R3.Reg32[0];}
    if (n == "ER3")    { return m_R3.Reg64;}

    if (n == "R4")     { return m_R4.Reg32[0];}
    if (n == "ER4")    { return m_R4.Reg64;}

    if (n == "R5")     { return m_R5.Reg32[0];}
    if (n == "ER5")    { return m_R5.Reg64;}
    return  0;
}

void CPU::setRegisterValue(QString n, LWORD v){
    if (n == "AL")     { m_A.Reg8[0] = static_cast <BYTE> (v);}
    if (n == "AH")     { m_A.Reg8[1] = static_cast <BYTE> (v);}
    if (n == "AX")     { m_A.Reg16[0] = static_cast <WORD> (v);}
    if (n == "EAX")    { m_A.Reg32 = static_cast <DWORD> (v);}

    if (n == "BL")     { m_B.Reg8[0] = static_cast <BYTE> (v);}
    if (n == "BH")     { m_B.Reg8[1] = static_cast <BYTE> (v);}
    if (n == "BX")     { m_B.Reg16[0] = static_cast <WORD> (v);}
    if (n == "EBX")    { m_B.Reg32 = static_cast <DWORD> (v);}

    if (n == "CL")     { m_C.Reg8[0] = static_cast <BYTE> (v);}
    if (n == "CH")     { m_C.Reg8[1] = static_cast <BYTE> (v);}
    if (n == "CX")     { m_C.Reg16[0] = static_cast <WORD> (v);}
    if (n == "ECX")    { m_C.Reg32 = static_cast <DWORD> (v);}

    if (n == "DL")     { m_D.Reg8[0] = static_cast <BYTE> (v);}
    if (n == "DH")     { m_D.Reg8[1] = static_cast <BYTE> (v);}
    if (n == "DX")     { m_D.Reg16[0] = static_cast <WORD> (v);}
    if (n == "EDX")    { m_D.Reg32 = static_cast <DWORD> (v);}

    if (n == "SP")     { m_SP.Reg16[0] = static_cast <WORD> (v);}
    if (n == "ESP")    { m_SP.Reg32 = static_cast <DWORD> (v);}

    if (n == "R0")     { m_R0.Reg32[0] = static_cast <DWORD> (v);}
    if (n == "ER0")    { m_R0.Reg64 = static_cast <QWORD> (v);}

    if (n == "R1")     { m_R1.Reg32[0] = static_cast <DWORD> (v);}
    if (n == "ER1")    { m_R1.Reg64 = static_cast <QWORD> (v);}

    if (n == "R2")     { m_R2.Reg32[0] = static_cast <DWORD> (v);}
    if (n == "ER2")    { m_R2.Reg64 = static_cast <QWORD> (v);}

    if (n == "R3")     { m_R3.Reg32[0] = static_cast <DWORD> (v);}
    if (n == "ER3")    { m_R3.Reg64 = static_cast <QWORD> (v);}

    if (n == "R4")     { m_R4.Reg32[0] = static_cast <DWORD> (v);}
    if (n == "ER4")    { m_R4.Reg64 = static_cast <QWORD> (v);}

    if (n == "R5")     { m_R5.Reg32[0] = static_cast <DWORD> (v);}
    if (n == "ER5")    { m_R5.Reg64 = static_cast <QWORD> (v);}
}



QString CPU::getA()     {return QString("%1").arg(m_A.Reg32,8,16,QChar('0'));}
QString CPU::getB()     {return QString("%1").arg(m_B.Reg32,8,16,QChar('0'));}
QString CPU::getC()     {return QString("%1").arg(m_C.Reg32,8,16,QChar('0'));}
QString CPU::getD()     {return QString("%1").arg(m_D.Reg32,8,16,QChar('0'));}
QString CPU::getSP()    {return QString("%1").arg(m_SP.Reg32,8,16,QChar('0'));}
QString CPU::getIP()    {return QString("%1").arg(m_IP.Reg32,8,16,QChar('0'));}

QString CPU::getFZ()    {return QString("%1").arg(m_FZ,1,16,QChar('0'));}
QString CPU::getFP()    {return QString("%1").arg(m_FP,1,16,QChar('0'));}
QString CPU::getFO()    {return QString("%1").arg(m_FO,1,16,QChar('0'));}

QString CPU::getR0()    {return QString("%1").arg(m_R0.Reg64,16,16,QChar('0'));}
QString CPU::getR1()    {return QString("%1").arg(m_R1.Reg64,16,16,QChar('0'));}
QString CPU::getR2()    {return QString("%1").arg(m_R2.Reg64,16,16,QChar('0'));}
QString CPU::getR3()    {return QString("%1").arg(m_R3.Reg64,16,16,QChar('0'));}
QString CPU::getR4()    {return QString("%1").arg(m_R4.Reg64,16,16,QChar('0'));}
QString CPU::getR5()    {return QString("%1").arg(m_R5.Reg64,16,16,QChar('0'));}

void CPU::setA(QString A)   {m_A.Reg32 = A.toUInt(nullptr,16);}
void CPU::setB(QString A)   {m_A.Reg32 = A.toUInt(nullptr,16);}
void CPU::setC(QString A)   {m_A.Reg32 = A.toUInt(nullptr,16);}
void CPU::setD(QString A)   {m_A.Reg32 = A.toUInt(nullptr,16);}
void CPU::setSP(QString A)  {m_A.Reg32 = A.toUInt(nullptr,16);}
void CPU::setIP(QString A)  {m_A.Reg32 = A.toUInt(nullptr,16);}

void CPU::setFZ(QString A)   {m_FZ = static_cast<BYTE>(A.at(0).toLatin1());}
void CPU::setFP(QString A)   {m_FP = static_cast<BYTE>(A.at(0).toLatin1());}
void CPU::setFO(QString A)   {m_FO = static_cast<BYTE>(A.at(0).toLatin1());}

void CPU::setR0(QString A)  {m_R0.Reg64 = A.toULong(nullptr,16);}
void CPU::setR1(QString A)  {m_R1.Reg64 = A.toULong(nullptr,16);}
void CPU::setR2(QString A)  {m_R2.Reg64 = A.toULong(nullptr,16);}
void CPU::setR3(QString A)  {m_R3.Reg64 = A.toULong(nullptr,16);}
void CPU::setR4(QString A)  {m_R4.Reg64 = A.toULong(nullptr,16);}
void CPU::setR5(QString A)  {m_R5.Reg64 = A.toULong(nullptr,16);}



QStringList CPU::getRawMemory(){
    QString rez;
    QStringList r;
    for(int i = 0; i < MEM_SIZE; i+=4){
        rez.clear();
        rez += "0000:";
        rez += QString("%1").arg(i,4,16,QChar('0'));
        rez += QString(" %1").arg(static_cast<uchar>(m_memory[i]),2,16,QChar('0'));
        rez += QString(" %1").arg(static_cast<uchar>(m_memory[i+1]),2,16,QChar('0'));
        rez += QString(" %1").arg(static_cast<uchar>(m_memory[i+2]),2,16,QChar('0'));
        rez += QString("%1").arg(static_cast<uchar>(m_memory[i+3]),2,16,QChar('0'));
        r.push_back(rez);
    }
    return r;
}

QString getReg(uchar n){
    if(n == 0x10) return "AX";
    if(n == 0x11) return "EAX";
    if(n == 0x12) return "AH";
    if(n == 0x13) return "AL";

    if(n == 0x20) return "BX";
    if(n == 0x21) return "EBX";
    if(n == 0x22) return "BH";
    if(n == 0x23) return "BL";

    if(n == 0x30) return "CX";
    if(n == 0x31) return "ECX";
    if(n == 0x32) return "CH";
    if(n == 0x33) return "CL";

    if(n == 0x40) return "DX";
    if(n == 0x41) return "EDX";
    if(n == 0x42) return "DH";
    if(n == 0x43) return "DL";

    if(n == 0x50) return "SP";
    if(n == 0x51) return "ESP";


    if(n == 0x90) return "R0";
    if(n == 0x91) return "ER0";

    if(n == 0xA0) return "R1";
    if(n == 0xA1) return "ER1";

    if(n == 0xB0) return "R2";
    if(n == 0xB1) return "ER2";

    if(n == 0xC0) return "R3";
    if(n == 0xC1) return "ER3";

    if(n == 0xD0) return "R4";
    if(n == 0xD1) return "ER4";

    if(n == 0xE0) return "R5";
    if(n == 0xE1) return "ER5";
    return "";
}





QString CPU::getDesLine(int addr){
    uchar com,op1,op2h,op2l;
    ushort op2;
    com = static_cast<uchar>(m_memory[addr]);
    op1 = static_cast<uchar>(m_memory[addr+1]);
    op2h = static_cast<uchar>(m_memory[addr+2]);
    op2l = static_cast<uchar>(m_memory[addr+3]);
    op2 = static_cast<ushort>((op2h << 8) + op2l);
    QString r;
    switch (com) {
    case 0x01:
        r+= "add ";
        r+= getReg(op1);
        r+= ", ";
        r+= getReg(op2l);
        break;
    case 0x02:
        r+= "addn ";
        r+= getReg(op1);
        r+= ", ";
        r+= QString("%1").arg(op2, 4, 16, QChar('0'));
        break;
    case 0x03:
        r+= "addm ";
        r+= getReg(op1);
        r+= ", ";
        r+= QString("&%1").arg(op2, 4, 16, QChar('0'));
        break;
    case 0x04:
        r+= "sub ";
        r+= getReg(op1);
        r+= ", ";
        r+= getReg(op2l);
        break;
    case 0x05:
        r+= "subn ";
        r+= getReg(op1);
        r+= ", ";
        r+= QString("%1").arg(op2, 4, 16, QChar('0'));
        break;
    case 0x06:
        r+= "subm ";
        r+= getReg(op1);
        r+= ", ";
        r+= QString("&%1").arg(op2, 4, 16, QChar('0'));
        break;
    case 0x07:
        r+= "mul ";
        r+= getReg(op2l);
        break;
    case 0x08:
        r+= "div ";
        r+= getReg(op2l);
        break;
    case 0x10:
        r+= "jmp ";
        r+= QString("&%1").arg(op2, 4, 16, QChar('0'));
        break;
    case 0x11:
        r+= "jz ";
        r+= QString("&%1").arg(op2, 4, 16, QChar('0'));
        break;
    case 0x12:
        r+= "jnz ";
        r+= QString("&%1").arg(op2, 4, 16, QChar('0'));
        break;
    case 0x13:
        r+= "jo ";
        r+= QString("&%1").arg(op2, 4, 16, QChar('0'));
        break;
    case 0x14:
        r+= "jno ";
        r+= QString("&%1").arg(op2, 4, 16, QChar('0'));
        break;
    case 0x15:
        r+= "jp ";
        r+= QString("&%1").arg(op2, 4, 16, QChar('0'));
        break;
    case 0x20:
        r+= "mov ";
        r+= getReg(op1);
        r+= ", ";
        r+= getReg(op2l);
        break;
    case 0x21:
        r+= "movn ";
        r+= getReg(op1);
        r+= ", ";
        r+= QString("%1").arg(op2, 4, 16, QChar('0'));
        break;
    case 0x22:
        r+= "movr ";
        r+= getReg(op2l);
        r+= ", ";
        r+= QString("&%1").arg(op1, 4, 16, QChar('0'));
        break;
    case 0x23:
        r+= "movm ";
        r+= getReg(op1);
        r+= ", ";
        r+= QString("&%1").arg(op1, 4, 16, QChar('0'));
        break;
    case 0x31:
        r+= "push ";
        r+= getReg(op1);
        break;
    case 0x32:
        r+= "pop ";
        r+= getReg(op1);
        break;
    case 0x40:
        r+= "and ";
        r+= getReg(op1);
        r+= ", ";
        r+= getReg(op2l);
        break;
    case 0x41:
        r+= "or ";
        r+= getReg(op1);
        r+= ", ";
        r+= getReg(op2l);
        break;
    case 0x42:
        r+= "xor ";
        r+= getReg(op1);
        r+= ", ";
        r+= getReg(op2l);
        break;
    case 0x43:
        r+= "not ";
        r+= getReg(op1);
        r+= ", ";
        r+= getReg(op2l);
        break;
    default:
        break;
    }
    return r;
}

QStringList CPU::getDesasmbl(){
    QString rez;
    QStringList r;
    for(int i = 0; i < MEM_SIZE; i+=4){
        rez.clear();
        rez += " ";
        rez += QString("%1;").arg(getDesLine(i),15);
        r.push_back(rez);
    }
    return r;
}
