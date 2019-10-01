#include <fstream>
#include <string>
#include <map>
#include <iostream>
#include <stdio.h>

using namespace std;

enum type {
    RM, RR, RI, JUMP, RET, CALL, END, WORD
};

struct operation {
    string name;
    int code;
    int type;
};

struct register_ {
    string name;
    int code;
};

operation operations[] = {{"halt", 0, RI},  //0
                        {"syscall", 1, RI}, //1
                        {"add", 2, RR},     //2
                        {"addi", 3, RI},    //3
                        {"sub", 4, RR},     //4
                        {"subi", 5, RI},    //5
                        {"mul", 6, RR},     //6
                        {"muli", 7, RI},    //7
                        {"div", 8, RR},     //8
                        {"divi", 9, RI},    //9
                        {"lc", 12, RI},     //10
                        {"shl", 13, RR},    //11
                        {"shli", 14, RI},   //12
                        {"shr", 15, RR},    //13
                        {"shri", 16, RI},   //14
                        {"and", 17, RR},    //15
                        {"andi", 18, RI},   //16
                        {"or", 19, RR},     //17
                        {"ori", 20, RI},    //18
                        {"xor", 21, RR},    //19
                        {"xori", 22, RI},   //20
                        {"not", 23, RI},    //21
                        {"mov", 24, RR},    //22
                        {"addd", 32, RR},   //23
                        {"subd", 33, RR},   //24
                        {"muld", 34, RR},   //25
                        {"divd", 35, RR},   //26
                        {"itod", 36, RR},   //27
                        {"dtoi", 37, RR},   //28
                        {"push", 38, RI},   //29
                        {"pop", 39, RI},    //30
                        {"call", 40, RR},   //31
                        {"calli", 41, CALL},//32
                        {"ret", 42, RET},   //33
                        {"cmp", 43, RR},    //34
                        {"cmpi", 44, RI},   //35
                        {"cmpd", 45, RR},   //36
                        {"jmp", 46, JUMP},  //37
                        {"jne", 47, JUMP},  //38
                        {"jeq", 48, JUMP},  //39
                        {"jle", 49, JUMP},  //40
                        {"jl", 50, JUMP},   //41
                        {"jge", 51, JUMP},  //42
                        {"jg", 52, JUMP},   //43
                        {"load", 64, RM},   //44
                        {"store", 65, RM},  //45
                        {"load2", 66, RM},  //46
                        {"store2", 67, RM}, //47
                        {"loadr", 68, RR},  //48
                        {"storer", 69, RR}, //49
                        {"loadr2", 70, RR}, //50
                        {"storer2", 71, RR},//51
                        {"end", 72, END},   //52
                        {"word", 73, WORD}  //53
};

register_ registers[] = {{"r0", 0},
                     {"r1", 1},
                     {"r2", 2},
                     {"r3", 3},
                     {"r4", 4},
                     {"r5", 5},
                     {"r6", 6},
                     {"r7", 7},
                     {"r8", 8},
                     {"r9", 9},
                     {"r10", 10},
                     {"r11", 11},
                     {"r12", 12},
                     {"r13", 13},
                     {"r14", 14},
                     {"r15", 15}
};

string s;
ifstream fin("input.fasm.txt");
ofstream fout("output.txt");

int main()
{
    while (getline(fin,s)) {
        int MachComm, commandNumber;
        sscanf(s.c_str(), "%d", &MachComm);
        int Command, R1, R2, IMM, type;
        Command = (MachComm >> 24);
        for (commandNumber = 0; commandNumber < 54; commandNumber++) {
            if (Command == operations[commandNumber].code) break;
        }
        if (commandNumber == 0) {
            fout << "word" << endl;
            continue;
        }
        if (commandNumber == 54) {
            fout << "ERROR" << endl;
            continue;
        }
        fout << operations[commandNumber].name << " ";
        type = operations[commandNumber].type;
        if (type == RM) {
            R1 = (MachComm >> 20) & 15;
            IMM = MachComm & 1048575;
            fout << "r" << R1 << " " << IMM;
        } else if (type == RR) {
            R1 = (MachComm >> 20) & 15;
            R2 = (MachComm >> 16) & 15;
            IMM = MachComm & ((1 << 16) - 1);
            if (IMM >> 15 == 1) {
                IMM = IMM - (1 << 16);
            }
            fout << "r" << R1 << " " << "r" << R2 << " " << IMM;
        } else if (type == RI) {
            R1 = (MachComm >> 20) & 15;
            IMM = MachComm & 1048575;
            if (IMM >> 19 == 1) {
                IMM = IMM - (1 << 20);
            }
            fout << "r" << R1 << " " << IMM;
        } else if (type == JUMP) {
            IMM = MachComm & 1048575;
            fout << IMM;
        } else if (type == RET) {
            IMM = MachComm & 1048575;
            fout << IMM;
        } else if (type == CALL) {
            IMM = MachComm & 1048575;
            fout << IMM;
        }
        fout << endl;
    }
    return 0;
}
