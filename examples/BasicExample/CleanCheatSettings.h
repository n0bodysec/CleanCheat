#pragma once

#include "CleanCheat/RunnersCollectionBase.h"
#include "MySharedData.h"
#include "Runners/BasicRunner.h"

#define SHARED_DATA_TYPE        MySharedData
#define CLEANCHEAT_LOG

class RunnersCollection final : public RunnersCollectionBase
{
public:
    BasicRunner* Basic = new BasicRunner();
};
