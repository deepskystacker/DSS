#pragma once

class AvxSimdCheck
{
public:
	static bool checkAvx2CpuSupport();
	static bool checkSimdAvailability();
	static void reportCpuType();
};
