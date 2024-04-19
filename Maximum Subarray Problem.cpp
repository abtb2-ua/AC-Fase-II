#pragma warning(disable : 6011)

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
#include <random>

using namespace std::chrono;
using namespace std;

class C {
public:
    static int maximumSubarray(int* n, int SIZE) {
        int *ps, *sm, *m;
        int maxSubarray = n[0];

        ps = new int[SIZE];
        ps[0] = n[0];
        for (int i = 1; i < SIZE; i++)
            ps[i] = ps[i - 1] + n[i];

        sm = new int[SIZE];
        sm[SIZE - 1] = ps[SIZE - 1];
        for (int i = SIZE - 2; i >= 0; i--)
            sm[i] = max(ps[i], sm[i + 1]);

        m = new int [SIZE];
        for (int i = 0; i < SIZE; i++) {
            m[i] = sm[i] - ps[i] + n[i];
            maxSubarray = max(maxSubarray, m[i]);
        }

        return maxSubarray;
    }

    static long long kadane(int* n, int SIZE) {
        int chain = INT16_MIN;
        int max = INT16_MIN;

        for (int i = 0; i < SIZE; i++) {
            chain = max(n[i], chain + n[i]);
            max = max(chain, max);
        }

        return max;
    }
};

class Assembly {
public: 
    static int maximumSubarray(int* n, int SIZE) {
        int* ps = new int[SIZE];
        int* sm = new int[SIZE];
        int* m = new int[SIZE];

        int s = SIZE;
        int result;

        _asm {
            ;;;;;;;; ps
            mov eax, n
            mov ebx, ps
            mov ecx, s
            shl ecx, 2
            mov esi, 4
            mov edx, [eax]
            mov [ebx], edx

        ps_loop:
            mov edx, [eax+esi]
            add edx, [ebx+esi-4]
            mov [ebx+esi], edx

            add esi, 4
            cmp esi, ecx
            jl ps_loop
                
            ;;;;;;;; sm
            mov eax, ps
            mov ebx, sm
            mov esi, s
            sub esi, 1
            shl esi, 2
            mov ecx, [eax+esi]
            mov [ebx+esi], ecx
            sub esi, 4

        sm_loop:
            mov ecx, [eax+esi]
            mov edx, [ebx+esi+4]
            cmp ecx, edx
            jle sm_loop_else
            mov [ebx+esi], ecx
            jmp sm_loop_endif
        sm_loop_else:
            mov [ebx+esi], edx
        sm_loop_endif:

            sub esi, 4
            cmp esi, 0
            jge sm_loop

            ;;;;;;;; m
            mov eax, n
            mov ebx, m
            mov esi, s
            sub esi, 1
            shl esi, 2
            mov edx, [eax+esi]
            sub esi, 4

        m_loop:
            mov eax, n
            mov ecx, [eax+esi]
            
            mov eax, sm
            add ecx, [eax+esi]

            mov eax, ps
            sub ecx, [eax+esi]

            cmp ecx, edx
            jle m_loop_endif
            mov edx, ecx
        m_loop_endif:

            sub esi, 4
            cmp esi, 0
            jge m_loop

            mov result, edx
        }

        delete[] ps;
        delete[] sm;
        delete[] m;

        return result;
    }
};

class SSE {
public:
    static int maximumSubarray(int* n, int SIZE)
    {
        int* ps = new int[SIZE];
        int* sm = new int[SIZE];
        int* m = new int[SIZE];

        int s = SIZE;
        int result;

        _asm
        {
            pusha
           
            ;;;;;;;; Desde i = 2 hasta SIZE : ps[i] = ps[i] + ps[i - 1]
            mov eax, ps
            mov ebx, 4
            mov ecx, s
            shl ecx, 2
            mov esi, n
            mov edx, [esi]
            mov [eax], edx

        ps_loop:
            cmp ebx, ecx
            jge end_ps_loop

            add edx, [esi + ebx]
            mov [eax + ebx], edx
            add ebx, 4
            jmp ps_loop

        end_ps_loop:
            

            ;;;;;;;; Desde i = SIZE hasta 1 : sm[i] = máximo hasta el momento
            mov eax, ps
            mov ebx, sm
        
            mov ecx, s
            sub ecx, 4
            shl ecx, 2

            movups xmm0, [eax + ecx]

        sm_loop:
            cmp ecx, 0
            jge continue_sm_loop
            mov ecx, 0

        continue_sm_loop:
            shufps xmm0, xmm0, 0x00

            movups xmm1, [eax + ecx]
            shufps xmm0, xmm0, 0xff
            pmaxsd xmm0, xmm1

            movups xmm1, xmm0
            shufps xmm0, xmm0, 0xf9
            pmaxsd xmm0, xmm1

            movups xmm1, xmm0
            shufps xmm0, xmm0, 0xfe
            pmaxsd xmm0, xmm1

            movups [ebx + ecx], xmm0

            cmp ecx, 0
            je end_sm_loop
            sub ecx, 16
            jmp sm_loop
        end_sm_loop:



            ;;;;;;;; Desde i = 1 hasta SIZE : m[i] = sm[i] - ps[i] + n[i]
            mov eax, n
            mov ebx, ps
            mov ecx, sm
            mov edx, m
            mov esi, s
            sub esi, 4
            shl esi, 2
            movups xmm3, [eax] ; max

        m_loop:
            movups xmm0, [eax + esi]
            movups xmm1, [ebx + esi]
            movups xmm2, [ecx + esi]

            psubd xmm2, xmm1
            paddd xmm2, xmm0

            movups [edx + esi], xmm2
        
            pmaxsd xmm3, xmm2

            cmp esi, 0
            je end_m_loop
            sub esi, 16
            jmp m_loop
        end_m_loop:

            movups xmm0, xmm3
            shufps xmm3, xmm3, 0xb1
            pmaxsd xmm0, xmm3
            movups xmm3, xmm0
            shufps xmm3, xmm3, 0x4e
            pmaxsd xmm3, xmm0

            movss result, xmm3

            popa
        }

        delete[] ps;
        delete[] sm;
        delete[] m;

        return result;
    }
};

long long clock(int& result, int* n, int SIZE, function<int(int*, int)> func) {
    auto start = high_resolution_clock::now();
    result = func(n, SIZE);
    auto end = high_resolution_clock::now();
    return duration_cast<microseconds>(end - start).count();
}

int main(void) {
    srand(time(nullptr));

    const pair<int, int> range = { 10, 20 };
    const pair<int, int> randomDist = { -pow(2, 10), pow(2, 10)};
    const int repeticiones = 20;
    vector<vector<long long>> times(range.second - range.first + 1, vector<long long>(3, 0));

    ofstream file("subarray_problem.csv");
    file << "n,c,assembly,sse" << endl;
    
    for (int j = 0; j < repeticiones; j++) {
        int* n = new int[pow(2, range.second)];

        for (int i = 0; i < pow(2, range.second); ++i) {
            n[i] = rand() % (-randomDist.first + randomDist.second) + randomDist.first;
        }

        for (int i = range.first; i <= range.second; i++) {
            const int SIZE = pow(2, i);

            int r1, r2, r3;
            int ref = C::kadane(n, SIZE);
            times[i - range.first][0] += clock(r1, n, SIZE, [](int* n, int SIZE) { return C::maximumSubarray(n, SIZE); });
            times[i - range.first][1] += clock(r2, n, SIZE, [](int* n, int SIZE) { return Assembly::maximumSubarray(n, SIZE); });
            times[i - range.first][2] += clock(r3, n, SIZE, [](int* n, int SIZE) { return SSE::maximumSubarray(n, SIZE); });

            if (r1 != ref || r2 != ref || r3 != ref)
                cerr << "ERROR: " << ref << " " << r1 << " " << r2 << " " << r3 << endl;
        }

        delete[] n;
    }


    for (int i = range.first; i <= range.second; i++) {
        const int SIZE = pow(2, i);
        file << SIZE << "," << times[i - range.first][0] / repeticiones << "," << times[i - range.first][1] / repeticiones << "," << times[i - range.first][2] / repeticiones;

        if (i != range.second)
            file << endl;
    }



    cout  << endl << "Done.";

    return 0;
}