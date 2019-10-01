#include <fstream>
#include <string>
#include <map>
#include <iostream>
#include <stdio.h>

using namespace std;

union double_int {
    unsigned i[2];
    double d;
};

enum type {
    RM, RR, RI, JUMP, RET, CALL, END, WORD
};

enum flag {
    NOT_GIVEN, EQUAL, SMALLER, GREATER
};

struct operation {
    string name;
    unsigned code;
    unsigned type;
};

struct register_ {
    string name;
    unsigned code;
};

string s;
ifstream fin("input.fasm.txt");
ofstream fout("output.txt");
map<string, unsigned> marks;
unsigned mark_code = 0;
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

unsigned r[16] = {0};
unsigned *memory = new unsigned[1048576];
int memory_position = 0;
int start_line = 0;
int flags = NOT_GIVEN;

int emulate(int MachComm) {
    unsigned command = (MachComm / 16777216) % 256;
    unsigned R1 = 0, R2 = 0, CODE = 0;
    int IMM = 0;
    unsigned i;
    for (i = 0; i < 54; i++) {
        if (operations[i].code == command) break;
    }
    CODE = operations[i].type;
    if (CODE == RM) {
        R1 = (MachComm >> 20) & 15;
        IMM = MachComm & 1048575;
    } else if (CODE == RR) {
        R1 = (MachComm >> 20) & 15;
        R2 = (MachComm >> 16) & 15;
        IMM = MachComm & ((1 << 16) - 1);
        if (IMM >> 15 == 1) {
            IMM = IMM - (1 << 16);
        }
    } else if (CODE == RI) {
        R1 = (MachComm >> 20) & 15;
        IMM = MachComm & 1048575;
        if (IMM >> 19 == 1) {
            IMM = IMM - (1 << 20);
        }
    } else if (CODE == JUMP) {
        IMM = MachComm & 1048575;
    } else if (CODE == RET) {
        IMM = MachComm & 1048575;
    } else if (CODE == CALL) {
        IMM = MachComm & 1048575;
    }
    //получили функцию и ее аргументы

    if (i == 0) {         //halt
        cout << "HALT CALLED" << endl;
        return -1;
    } else if (i == 1) {  //syscall
        if (IMM == 0) {
            return -1;
        } else if (IMM == 102) {
            printf("%d", r[R1]);
        } else if (IMM == 103) {
            double_int buf;
            buf.i[0] = r[R1];
            buf.i[1] = r[R1 + 1];
            cout << buf.d;
        } else if (IMM == 105) {
            putchar(r[R1]);
        }
        r[15]++;
    } else if (i == 2) { //add
        r[R1] += (r[R2] + IMM);
        r[15]++;
    } else if (i == 3) { //addi
        r[R1] += IMM;
        r[15]++;
    } else if (i == 4) { //sub
        r[R1] -= (r[R2] + IMM);
        r[15]++;
    } else if (i == 5) { //subi
        r[R1] -= IMM;
        r[15]++;
    } else if (i == 6) { //mul
        long long buf = (long long)r[R1] * (long long)r[R2];
        r[R1] = (buf & (-1));
        r[R1 + 1] = (buf >> 32);
        r[15]++;
    } else if (i == 7) { //muli
        long long buf = (long long)r[R1] * (long long)IMM;
        r[R1] = (buf & (-1));
        r[R1 + 1] = (buf >> 32);
        r[15]++;
    } else if (i == 8) { //div
        long long buf = (long long)r[R1] | ((long long)r[R1 + 1] << 32);
        r[R1] = buf / r[R2];
        r[R1 + 1] = buf % r[R2];
        r[15]++;
    } else if (i == 9) { //divi
        long long buf = (long long)r[R1] | ((long long)r[R1 + 1] << 32);
        r[R1] = buf / IMM;
        r[R1 + 1] = buf % IMM;
        r[15]++;
    } else if (i == 10) { //lc
        r[R1] = IMM;
        r[15]++;
    } else if (i == 11) { //shl
        r[R1] = (r[R1] << r[R2]);
        r[15]++;
    } else if (i == 12) { //shli
        r[R1] = (r[R1] << IMM);
        r[15]++;
    } else if (i == 13) { //shr
        r[R1] = (r[R1] >> r[R2]);
        r[15]++;
    } else if (i == 14) { //shri
        r[R1] = (r[R1] >> IMM);
        r[15]++;
    } else if (i == 15) { //and
        r[R1] &= r[R2];
        r[15]++;
    } else if (i == 16) { //andi
        r[R1] &= IMM;
        r[15]++;
    } else if (i == 17) { //or
        r[R1] |= r[R2];
        r[15]++;
    } else if (i == 18) { //ori
        r[R1] |= IMM;
        r[15]++;
    } else if (i == 19) { //xor
        r[R1] ^= r[R2];
        r[15]++;
    } else if (i == 20) { //xori
        r[R1] ^= IMM;
        r[15]++;
    } else if (i == 21) { //not
        r[R1] = ~r[R1];
        r[15]++;
    } else if (i == 22) { //mov
        r[R1] = (r[R2] + IMM);
        r[15]++;
    } else if (i == 23) { //addd
        double_int left, right;
        left.i[0] = r[R1];
        left.i[1] = r[R1 + 1];
        right.i[0] = r[R2];
        right.i[1] = r[R2 + 1];
        left.d += right.d;
        r[R1] = left.i[0];
        r[R1 + 1] = left.i[1];
        r[15]++;
    } else if (i == 24) { //subd
        double_int left, right;
        left.i[0] = r[R1];
        left.i[1] = r[R1 + 1];
        right.i[0] = r[R2];
        right.i[1] = r[R2 + 1];
        left.d -= right.d;
        r[R1] = left.i[0];
        r[R1 + 1] = left.i[1];
        r[15]++;
    } else if (i == 25) { //muld
        double_int left, right;
        left.i[0] = r[R1];
        left.i[1] = r[R1 + 1];
        right.i[0] = r[R2];
        right.i[1] = r[R2 + 1];
        left.d *= right.d;
        r[R1] = left.i[0];
        r[R1 + 1] = left.i[1];
        r[15]++;
    } else if (i == 26) { //divd
        double_int left, right;
        left.i[0] = r[R1];
        left.i[1] = r[R1 + 1];
        right.i[0] = r[R2];
        right.i[1] = r[R2 + 1];
        left.d /= right.d;
        r[R1] = left.i[0];
        r[R1 + 1] = left.i[1];
        r[15]++;
    } else if (i == 27) { //itod
        double_int left;
        left.d = (double)r[R2];
        r[R1] = left.i[0];
        r[R1 + 1] = left.i[1];
        r[15]++;
    } else if (i == 28) { //dtoi
        double_int right;
        right.i[0] = r[R2];
        right.i[1] = r[R2 + 1];
        r[R1] = (int)right.d;
        r[15]++;
    } else if (i == 29) { //push
        r[14]--;
        memory[r[14]] = (r[R1] + IMM);
        r[15]++;
    } else if (i == 30) { //pop
        r[R1] = memory[r[14]] + IMM;
        r[14]++;
        r[15]++;
    } else if (i == 31) { //call
        r[14]--;
        memory[r[14]] = r[15] + 1;
        r[15] = r[R2] + IMM;
        r[R1] = r[15] + 1;
    } else if (i == 32) { //calli
        r[14]--;
        memory[r[14]] = r[15] + 1;
        r[15] = IMM;
    } else if (i == 33) { //ret
        r[15] = memory[r[14]];
        r[14]++;
        r[14] += IMM;
    } else if (i == 34) { //cmp
        if (r[R1] == r[R2]) {
            flags = EQUAL;
        } else if (r[R1] > r[R2]) {
            flags = GREATER;
        } else if (r[R1] < r[R2]) {
            flags = SMALLER;
        }
        r[15]++;
    } else if (i == 35) { //cmpi
        if ((int)r[R1] == IMM) {
            flags = EQUAL;
        } else if ((int)r[R1] > IMM) {
            flags = GREATER;
        } else if ((int)r[R1] < IMM) {
            flags = SMALLER;
        }
        r[15]++;
    } else if (i == 36) { //cmpd
        double_int left, right;
        left.i[0] = r[R1];
        left.i[1] = r[R1 + 1];
        right.i[0] = r[R2];
        right.i[1] = r[R2 + 1];
        if (left.d == right.d) {
            flags = EQUAL;
        } else if (left.d > right.d) {
            flags = GREATER;
        } else if (left.d < right.d) {
            flags = SMALLER;
        }
        r[15]++;
    } else if (i == 37) { //jmp
        r[15] = IMM;
    } else if (i == 38) { //jne
        if (flags == NOT_GIVEN) {
            r[15]++;
        } else if (flags == EQUAL) {
            r[15]++;
        } else if (flags == GREATER) {
            r[15] = IMM;
        } else if (flags == SMALLER) {
            r[15] = IMM;
        }
    } else if (i == 39) { //jeq
        if (flags == NOT_GIVEN) {
            r[15]++;
        } else if (flags == EQUAL) {
            r[15] = IMM;
        } else if (flags == GREATER) {
            r[15]++;
        } else if (flags == SMALLER) {
            r[15]++;
        }
    } else if (i == 40) { //jle
        if (flags == NOT_GIVEN) {
            r[15]++;
        } else if (flags == EQUAL) {
            r[15] = IMM;
        } else if (flags == GREATER) {
            r[15]++;
        } else if (flags == SMALLER) {
            r[15] = IMM;
        }
    } else if (i == 41) { //jl
        if (flags == NOT_GIVEN) {
            r[15]++;
        } else if (flags == EQUAL) {
            r[15]++;
        } else if (flags == GREATER) {
            r[15]++;
        } else if (flags == SMALLER) {
            r[15] = IMM;
        }
    } else if (i == 42) { //jge
        if (flags == NOT_GIVEN) {
            r[15]++;
        } else if (flags == EQUAL) {
            r[15] = IMM;
        } else if (flags == GREATER) {
            r[15] = IMM;
        } else if (flags == SMALLER) {
            r[15]++;
        }
    } else if (i == 43) { //jg
        if (flags == NOT_GIVEN) {
            r[15]++;
        } else if (flags == EQUAL) {
            r[15]++;
        } else if (flags == GREATER) {
            r[15] = IMM;
        } else if (flags == SMALLER) {
            r[15]++;
        }
    } else if (i == 44) { //load
        r[R1] = memory[IMM];
        r[15]++;
    } else if (i == 45) { //store
        memory[IMM] = r[R1];
        r[15]++;
    } else if (i == 46) { //load2
        r[R1] = memory[IMM];
        r[R1 + 1] = memory[IMM + 1];
        r[15]++;
    } else if (i == 47) { //store2
        memory[IMM] = r[R1];
        memory[IMM + 1] = r[R1 + 1];
        r[15]++;
    } else if (i == 48) { //loadr
        r[R1] = memory[r[R2] + IMM];
        r[15]++;
    } else if (i == 49) { //storer
        memory[r[R2] + IMM] = r[R1];
        r[15]++;
    } else if (i == 50) { //loadr2
        r[R1] = memory[r[R2] + IMM];
        r[R1 + 1] = memory[r[R2] + IMM + 1];
        r[15]++;
    } else if (i == 51) { //storer2
        memory[r[R2] + IMM] = r[R1];
        memory[r[R2] + IMM + 1] = r[R1 + 1];
        r[15]++;
    } else if (i == 52) { //end
        r[15]++;
    } else if (i == 53) { //word
        r[15]++;
    }


    return 0;
}
//функция, выполняющая машинную команду по ее слову

int main()
{
    while (getline(fin, s)) {
        unsigned long long p = s.find(';');
        if (p != string::npos) {
            s = s.substr(0, p);
        }
        // удаление комментария

        p = s.find(':');
        if (p != string::npos) {
            string s_mark = s.substr(0, p);
            s = s.substr(p + 1, s.length() - p - 1);
            p = marks[s_mark];
            if (p == 0) {
                marks[s_mark] = mark_code;
            }
            p = s.find("  ");
            while (p != string::npos) {
                s.erase(p, 1);
                p = s.find("  ");
            }
            if (s[0] == ' ') s.erase(0, 1);
            if (s.length() > 0 && s[s.length() - 1] == ' ') s = s.substr(0, s.length() - 1);
            p = s.find((char)9);
            while (p != string::npos) {
                s.erase(p, 1);
                p = s.find((char)9);
            }
            if (s[0] == (char)9) s.erase(0, 1);
            if (s.length() > 0 && s[s.length() - 1] == (char)9) s = s.substr(0, s.length() - 1);
            //удаление лишних пробелов
             if (s.length() > 0) {
                mark_code++;
             }
        } else {
            mark_code++;
        }
    }
    fin.clear();
    fin.seekg(0, ios_base::beg);
    // поиск и сохранение меток


/*  fout << "ThisIsFUPM2Exec\n";  //название
    fout <<  mark_code - 1 << endl; //количество команд
    fout << 0 << endl; //нолик
    fout << 0 << endl; //нолик


    while (getline(fin, s)) {

        unsigned p = s.find("end");
        if (p != s.npos) {
            s = s.substr(p + 4, s.length() - p - 4);
            p = s.find("  ");
            while (p != s.npos) {
                s.erase(p, 1);
                p = s.find("  ");
            }
            if (s[0] == ' ') s.erase(0, 1);
            if (s[s.length() - 1] == ' ') s.erase(s.length() - 1, 1);
            r[15] = marks[s]; // начало программы
            fout << r[15] << endl;
            break;
        }

    }
    //поиск начала программы
    */

    fin.clear();
    fin.seekg(0, ios_base::beg);
    r[14] = 1048575; //адрес стека
    //заголовок исполнимого файла


    while (getline(fin, s)) {

        unsigned long long p = s.find(':');
        if (p != string::npos) {
            s = s.substr(p + 1, s.length() - p - 1);
        }
        // удаление метки

        p = s.find(';');
        if (p != string::npos) {
            s = s.substr(0, p);
        }
        // удаление комментария

        p = s.find("  ");
        while (p != string::npos) {
            s.erase(p, 1);
            p = s.find("  ");
        }
        if (s[0] == ' ') s.erase(0, 1);
        if (s.length() > 0 && s[s.length() - 1] == ' ') s = s.substr(0, s.length() - 1);
        p = s.find((char)9);
        while (p != string::npos) {
            s.erase(p, 1);
            p = s.find((char)9);
        }
        if (s[0] == (char)9) s.erase(0, 1);
        if (s.length() > 0 && s[s.length() - 1] == (char)9) s = s.substr(0, s.length() - 1);
        //удаление лишних пробелов

        if (s.length() == 0) continue;
        //пропускаем строчку, если она состоит только из пробелов

        string command;
        int commandNumber;
        p = s.find(" ");
        command = s.substr(0, p);
        s = s.substr(p + 1, s.length() - p - 1);
        for (commandNumber = 0; commandNumber < 54; commandNumber++) {
            if (command == operations[commandNumber].name) break;
        }
        //ищем команду

        if (commandNumber == 54) {
            cout << "error in line " << memory_position << endl;
            fin.close();
            fout.close();
            delete [] memory;
            return 0;
        }
        //если команда не нашлась, получаем ошибку компиляции, завершаем программу
  //      fout << operations[commandNumber].code << " ";
        unsigned machineCommand = (operations[commandNumber].code << 24);
        //выводим команду

        string argument1, argument2, argument3;
        int register1 = 0, register2 = 0, Imm = 0;
        if (operations[commandNumber].type == RM) {
            p = s.find(" ");
            argument1 = s.substr(0, p);
            argument2 = s.substr(p + 1, s.length() - p - 1);

            for (register1 = 0; register1 < 16; register1++) {
                if (argument1 == registers[register1].name) break;
            }
            //ищем регистр

            if (register1 == 16) {
                cout << "error in line " << memory_position << endl;
                fin.close();
                fout.close();
                delete [] memory;
                return 0;
            }
            //если регистр не нашелся, ошибка компиляции
    //        fout << register1 << " ";
            machineCommand |= (register1 << 20);
            //выводим регистр

            if ((Imm = marks[argument2]) == 0) {
                sscanf(argument2.c_str(), "%d", &Imm);
            }
            //непосредственный операнд либо равен числу, либо метке
     //       fout << Imm << " ";
            machineCommand |= Imm;
      //      fout << endl;
        }
        ////////////////////////////////////////////////////////////////
        else if (operations[commandNumber].type == RR) {
            p = s.find(" ");
            argument1 = s.substr(0, p);
            s = s.substr(p + 1, s.length() - 1 - p);
            p = s.find(" ");
            argument2 = s.substr(0, p);
            argument3 = s.substr(p + 1, s.length() - 1 - p);

            for (register1 = 0; register1 < 16; register1++) {
                if (argument1 == registers[register1].name) break;
            }
            //ищем регистр1

            if (register1 == 16) {
                cout << "error in line " << memory_position << endl;
                fin.close();
                fout.close();
                delete [] memory;
                return 0;
            }
            //если регистр1 не нашелся, ошибка компиляции
     //       fout << register1 << " ";
            machineCommand |= (register1 << 20);
            //выводим регистр1

            for (register2 = 0; register1 < 16; register2++) {
                if (argument2 == registers[register2].name) break;
            }
            //ищем регистр2

            if (register2 == 16) {
                cout << "error in line " << memory_position << endl;
                fin.close();
                fout.close();
                delete [] memory;
                return 0;
            }
            //если регистр2 не нашелся, ошибка компиляции
   //         fout << register2 << " ";
            machineCommand |= (register2 << 16);
            //выводим регистр2

            if ((Imm = marks[argument3]) == 0) {
                sscanf(argument3.c_str(), "%d", &Imm);
            }
            //непосредственный операнд либо равен числу, либо метке
        //    fout << Imm << " ";
            machineCommand |= Imm;
        //    fout << endl;
        }
        ////////////////////////////////////////////////////////////////
        else if (operations[commandNumber].type == RI) {
            p = s.find(" ");
            argument1 = s.substr(0, p);
            argument2 = s.substr(p + 1, s.length() - p - 1);

            for (register1 = 0; register1 < 16; register1++) {
                if (argument1 == registers[register1].name) break;
            }
            //ищем регистр

            if (register1 == 16) {
                cout << "error in line " << memory_position << endl;
                fin.close();
                fout.close();
                delete [] memory;
                return 0;
            }
            //если регистр не нашелся, ошибка компиляции
         //   fout << register1 << " ";
            machineCommand |= (register1 << 20);
            //выводим регистр

            if ((Imm = marks[argument2]) == 0) {
                sscanf(argument2.c_str(), "%d", &Imm);
            }
            //непосредственный операнд либо равен числу, либо метке
        //    fout << Imm << " ";
            machineCommand |= Imm;
        //    fout << endl;
        }
        ////////////////////////////////////////////////////////////////
        else if (operations[commandNumber].type == JUMP) {
            if ((Imm = marks[s]) == 0) {
                sscanf(s.c_str(), "%d", &Imm);
            }
            //непосредственный операнд либо равен числу, либо метке
         //   fout << Imm << " ";
            machineCommand |= Imm;
        //    fout << endl;
        }
        ////////////////////////////////////////////////////////////////
        else if (operations[commandNumber].type == RET) {
            if ((Imm = marks[s]) == 0) {
                sscanf(s.c_str(), "%d", &Imm);
            }
            //непосредственный операнд либо равен числу, либо метке
      //      fout << Imm << " ";
            machineCommand |= Imm;
     //       fout << endl;
        }
        ////////////////////////////////////////////////////////////////
        else if (operations[commandNumber].type == CALL) {
            if ((Imm = marks[s]) == 0) {
                sscanf(s.c_str(), "%d", &Imm);
            }
            //непосредственный операнд либо равен числу, либо метке
      //      fout << Imm << " ";
            machineCommand |= Imm;
       //     fout << endl;
        }
        ////////////////////////////////////////////////////////////////
        else if (operations[commandNumber].type == END) {
            if ((Imm = marks[s]) == 0) {
                sscanf(s.c_str(), "%d", &Imm);
            }
            //непосредственный операнд либо равен числу, либо метке
       //     fout << Imm << " ";
            start_line = Imm;
            r[15] = start_line;
            machineCommand = -1;
      //      fout << endl;

        }
        ////////////////////////////////////////////////////////////////
        else if (operations[commandNumber].type == WORD) {
            machineCommand = 0;
     //       fout << endl;
        }
        ////////////////////////////////////////////////////////////////
        if (machineCommand != (unsigned)-1) {
            memory[memory_position] = machineCommand;
            memory_position++;
        }
        //загрузка команды в память
        if (operations[commandNumber].type != END) fout << machineCommand << endl;
    }
    //компиляция

    int machineCommand, code;
    while (1) {
 //       cout << "Now doing line " << r[15] << endl;
        machineCommand = memory[r[15]];
        code = emulate(machineCommand);
        if (code == -1) {
            break;
        }
    }
    //эмуляция работы машины

    fin.close();
    fout.close();
    delete [] memory;
    return 0;
}
