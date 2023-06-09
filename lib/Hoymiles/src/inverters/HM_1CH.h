// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include "HM_Abstract.h"
#include <list>

class HM_1CH : public HM_Abstract {
public:
    explicit HM_1CH(HoymilesRadio* radio, uint64_t serial);
    static bool isValidSerial(uint64_t serial);
    String typeName();
    const byteAssign_t* getByteAssignment();
    uint8_t getByteAssignmentSize();
};