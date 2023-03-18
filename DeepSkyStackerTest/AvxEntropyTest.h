#pragma once
#include "../DeepSkyStacker/EntropyInfo.h"

class TestEntropyInfo : public CEntropyInfo
{
private:
	virtual void InitSquareEntropies() override;
};
