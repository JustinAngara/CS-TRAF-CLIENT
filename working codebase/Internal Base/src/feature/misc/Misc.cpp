#include "Misc.h"
#include "bhop/Bhop.h"
#include <iostream>

// this is main entry point
void Misc::Render()
{
	// need to run from here
	Misc::Run();
}

void Misc::Run() {
	// run bunny hop feature
	Bhop::Run();
}
