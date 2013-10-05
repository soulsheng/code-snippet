#include "stdio.h"
#include "Windows.h"	// Required for Sleep

#define CPUID __asm __emit 0fh __asm __emit 0a2h
#define RDTSC __asm __emit 0fh __asm __emit 031h //Not needed in latest Microsoft Visual Studio

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
	printf("Hi %d %s  ", Argc, *Argv);
	printf("%I64u  %I64u  ", getCycles(), rdtsc());
	Sleep(10000);
	TNEW = rdtsc();
	printf("%I64u\n", TNEW - TOLD);
	TOLD = TNEW;
	if (Argc) main(--Argc, ++Argv, Arge);
	else
		while (*Arge) printf("%s ", *Arge++);
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