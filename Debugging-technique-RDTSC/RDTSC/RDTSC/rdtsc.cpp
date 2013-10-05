#include "stdio.h"
#include "Windows.h"	// Required for Sleep

#define CPUID __asm __emit 0fh __asm __emit 0a2h
#define RDTSC __asm __emit 0fh __asm __emit 031h //Not needed in latest Microsoft Visual Studio

float ProcSpeedCalc();
unsigned __int64 getCycles(void);
unsigned __int64 TNEW, TOLD;

// The following avoids the overhead of saving and restoring registers,
//adjusting the stack, and a call to some monitoring function
__declspec(naked)
	unsigned __int64 __cdecl rdtsc(void)	//The __cdecl is probably redundant in most cases
{
	__asm
	{
		rdtsc;
		ret;  // return value in EDX:EAX
	}
}

int main(int Argc, char *Argv[], char *Arge[])
{
	unsigned __int64 nFreq = 0;
	QueryPerformanceFrequency((LARGE_INTEGER *) &nFreq);
	printf("1.QueryPerformanceFrequency frequency:%d MHz\n", nFreq/1000);
	printf("2.ProcSpeedCalc:%d MHz\n", (int)ProcSpeedCalc());
	//printf("Hi %d %s  \n", Argc, *Argv);
	//printf("%I64u  %I64u  \n", getCycles(), rdtsc());
	TOLD = rdtsc();
	Sleep(1000);
	TNEW = rdtsc();
	printf("3.rdtsc %I64u MHz\n", (TNEW - TOLD)/1000000 );
	

	return 0;
}

// Don't use this form... too much overhead. See rdtsc above
unsigned __int64 getCycles()
{
	__asm {

		// useful sometimes to force completion of previous instructions
		// CPUID
		RDTSC;
	}
}

float ProcSpeedCalc()
{
/*
RdTSC:
It's the Pentium instruction "ReaD Time Stamp Counter". It measures the
number of clock cycles that have passed since the processor was reset, as a
64-bit number. That's what the <CODE>_emit</CODE> lines do.*/
//#define RdTSC __asm _emit 0x0f __asm _emit 0x31

// variables for the clock-cycles:
__int64 cyclesStart = 0, cyclesStop = 0;
// variables for the High-Res Preformance Counter:
unsigned __int64 nCtr = 0, nFreq = 0, nCtrStop = 0;


    // retrieve performance-counter frequency per second:
    if(!QueryPerformanceFrequency((LARGE_INTEGER *) &nFreq)) return 0;

    // retrieve the current value of the performance counter:
    QueryPerformanceCounter((LARGE_INTEGER *) &nCtrStop);

    // add the frequency to the counter-value:
    nCtrStop += nFreq;


    _asm
        {// retrieve the clock-cycles for the start value:
            RDTSC
            mov DWORD PTR cyclesStart, eax
            mov DWORD PTR [cyclesStart + 4], edx
        }

        do{
        // retrieve the value of the performance counter
        // until 1 sec has gone by:
             QueryPerformanceCounter((LARGE_INTEGER *) &nCtr);
          }while (nCtr < nCtrStop);

    _asm
        {// retrieve again the clock-cycles after 1 sec. has gone by:
            RDTSC
            mov DWORD PTR cyclesStop, eax
            mov DWORD PTR [cyclesStop + 4], edx
        }

// stop-start is speed in Hz divided by 1,000,000 is speed in MHz
return    ((float)cyclesStop-(float)cyclesStart) / 1000000;
}