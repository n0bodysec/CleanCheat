﻿#pragma once
#include "CleanCheat/RunnerBase.h"

class BasicRunner final : public RunnerBase
{
protected:
    void OnExecute() override;
    
public:
    bool Condition() override;
};
