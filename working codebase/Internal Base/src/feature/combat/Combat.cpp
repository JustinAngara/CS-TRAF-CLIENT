#include "Combat.h"
#include "legitbot/Aimbot.h"
#include <iostream>
void Combat::Render() {
	Aimbot::Run();
	std::cout << "now in combat;\n";
}