

#include "GenericTypeDefs.h"
#include "Compiler.h"


#if defined(__C30__)
	void _ISR __attribute__((__no_auto_psv__)) _AddressError(void)
	{
	   
	}

	
#elif defined(__C32__)
	void _general_exception_handler(unsigned cause, unsigned status)
	{
		Nop();
		Nop();
	}
#endif



