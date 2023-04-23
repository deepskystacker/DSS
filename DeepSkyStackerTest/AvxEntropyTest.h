#pragma once
#include "EntropyInfo.h"

class TestEntropyInfo : public CEntropyInfo
{
private:
	virtual void InitSquareEntropies() override;
};
