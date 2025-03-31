///////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) 2024 SaxonSoc contributors
//  SPDX license identifier: MIT
//  Full license header bsp/efinix/EfxSapphireSoc/include/LICENSE.MD
///////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "type.h"
#include "io.h"

#define PRESCALER_VALUE     0x00


    writeReg_u32(prescaler_setValue              , PRESCALER_VALUE)
