#include <stdio.h> 
#include <stdlib.h>
#include <ctime>
#include <chrono>
#include <functional>
#include <queue>
#include <iostream>
#include <fstream>
#include "stdio.h"
#include "windows.h"
#include <vector>

using namespace std::chrono;
using namespace std;

class C {
private:
    static int collatz(int n) {
        int pasos = 0;
        unsigned long num = n;

        while (num != 1) {
            if (num % 2 == 0)
                num = num / 2;
            else
                num = num * 3 + 1;
            pasos++;
        }
        return pasos;
    }

public:
    static int find(int n) {
        int max_i = 0;
        int max_value = 0;
        for (int i = 2; i < n; i++) {
            int res = collatz(i);
            if (res > max_value) {
                max_value = res;
                max_i = i;
            }
        }
        return max_i;
    }
};

class Assembly {
public:
    static int find(int n) {
        __asm {
            jmp find_collatz

            collatz :
            mov eax, 0
                start_col_loop :
                test ebx, 1
                jz es_par
                mov ecx, ebx
                shl ebx, 1
                add ebx, ecx
                inc ebx
                inc eax
                jmp start_col_loop
                es_par :
            shr ebx, 1
                inc eax
                cmp ebx, 1
                jne start_col_loop
                ret

                find_collatz :
            mov edx, 1; greater found
                mov esi, 1; greater found
                mov edi, 1; counter
                start_find_loop :
            mov ebx, edi
                call collatz


                cmp edx, eax
                jge continue_find_loop
                mov edx, eax
                mov esi, edi

                continue_find_loop :
            inc edi
                cmp edi, n
                jl start_find_loop
                mov eax, esi
        }
    }
};

long long clock(int& result, int arg, function<int(int)> func) {
    auto start = high_resolution_clock::now();
    result = func(arg);
    auto end = high_resolution_clock::now();
    return duration_cast<microseconds>(end - start).count();
}

/*int main(void)
{
    ofstream file("collatz.csv");
    file << "n,c,assembly,sse" << endl;

    long long assembly, c;


    printf_s("\n\n");
    for (int i = 10; i <= 20; i++) {
        int r1, r2;
        long n = (long)pow(2, i);

        assembly = clock(r1, n, [](int n) {return Assembly::find(n); });
        c = clock(r2, n, [](int n) {return C::find(n); });

        if (r1 != r2) {
            printf_s("ERROR: las funciones no han dado el mismo resultado. \n Terminando programa");
            return -1;
        }

        file << n << "," << c << "," << assembly;
        if (i < 20)
            file << endl;
    }

    return 0;
}*/