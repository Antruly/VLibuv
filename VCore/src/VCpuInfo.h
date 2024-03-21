#pragma once
#include "VCoreDefine.h"
#include "VLibuvInclude.h"
#include "VObject.h"
class VCpuInfo :
    public VObject
{
public:
    VCpuInfo();
    ~VCpuInfo();
    DEFINE_COPY_FUNC(VCpuInfo);

    int init();

private:
    uv_cpu_info_t* cpu_info = nullptr;
};

