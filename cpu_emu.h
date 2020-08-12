#pragma once

#include "HEADER.h"

using namespace std;

class CPU: public QObject{
    Q_OBJECT

private:
    QByteArray m_memory; // Память

    Register32 m_A;     // AX (primary accumulator)
    Register32 m_B;     // BX (base, accumulator)
    Register32 m_C;     // CX (counter, accumulator)
    Register32 m_D;     // DX (accumulator, other functions)

    Register32 m_SP;    // Stack Pointer
    Register32 m_IP;    // Instruction Pointer

    //FLAGS
    BYTE m_FZ;          // Zero Flag
    BYTE m_FO;          // Overflow Flag
    BYTE m_FP;          // Carry Flag

    //Common use registers
    Register64 m_R0;
    Register64 m_R1;
    Register64 m_R2;
    Register64 m_R3;
    Register64 m_R4;
    Register64 m_R5;

    QProcess *editor;

public:
    explicit CPU(QObject *p = nullptr); // Конструктор p - родительский объект
    int readFromFile(QString);// Прочитать программу из файла
    void reset();  // Сбросить показания регистров
    LWORD getRegisterValue(QString); //Получить значения регистра по его имени
    QWORD getRegisterValue(uint); //Получить значение регистра по его коду
    void setRegisterValue(QString,LWORD); // устаносить значение регистра по его имени
    void setRegisterValue(uint,DWORD); //установить значение регистра по его коду
    void setRegisterValue(uint,QWORD); //установить значение регистра по его коду
    QString getDesLine(int); // получить строку дизасемблированных данных



public slots:
    QString getOpenEditor(QString, QString); // открыть редактор
    QString getOpenEditor(QString); // открыть редактор по умолчанию
    void getDataFromFile(QString); //получить данные из файла
    QStringList getRawMemory(); // получить массив строк состояния памяти
    QStringList getDesasmbl(); //получить массив строк дизассемблированных данных

    bool tick(); // тик процессора

    // БЛОК GET SET
    QString getA() ;
    void setA(QString A);
    QString getB() ;
    void setB(QString A);
    QString getC() ;
    void setC(QString A);
    QString getD() ;
    void setD(QString A);

    QString getSP() ;
    void setSP(QString A);  
    QString getIP() ;
    void setIP(QString A);


    QString getFZ() ;
    void setFZ(QString A);
    QString getFO() ;
    void setFO(QString A);
    QString getFP() ;
    void setFP(QString A);

    QString getR0() ;
    void setR0(QString A);
    QString getR1() ;
    void setR1(QString A);
    QString getR2() ;
    void setR2(QString A);
    QString getR3() ;
    void setR3(QString A);
    QString getR4() ;
    void setR4(QString A);
    QString getR5() ;
    void setR5(QString A);


signals:

};

