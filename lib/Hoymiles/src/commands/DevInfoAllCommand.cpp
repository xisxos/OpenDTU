// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (C) 2022-2024 Thomas Basler and others
 */

/*
This command is used to fetch firmware information from the inverter.

Derives from MultiDataCommand

Command structure:
* DT: this specific command uses 0x01

00   01 02 03 04   05 06 07 08   09   10   11   12 13 14 15   16 17   18 19   20 21 22 23   24 25   26   27 28 29 30 31
-----------------------------------------------------------------------------------------------------------------------
                                      |<------------------- CRC16 --------------------->|
15   71 60 35 46   80 12 23 04   80   01   00   65 72 06 B8   00 00   00 00   00 00 00 00   00 00   00   -- -- -- -- --
^^   ^^^^^^^^^^^   ^^^^^^^^^^^   ^^   ^^   ^^   ^^^^^^^^^^^   ^^^^^           ^^^^^^^^^^^   ^^^^^   ^^
ID   Target Addr   Source Addr   Idx  DT   ?    Time          Gap             Password      CRC16   CRC8
*/
#include "DevInfoAllCommand.h"
#include "inverters/InverterAbstract.h"

DevInfoAllCommand::DevInfoAllCommand(InverterAbstract* inv, const uint64_t router_address, const time_t time)
    : MultiDataCommand(inv, router_address)
{
    setTime(time);
    setDataType(0x01);
    setTimeout(200);
}

String DevInfoAllCommand::getCommandName() const
{
    return "DevInfoAll";
}

bool DevInfoAllCommand::handleResponse(const fragment_t fragment[], const uint8_t max_fragment_id)
{
    // Check CRC of whole payload
    if (!MultiDataCommand::handleResponse(fragment, max_fragment_id)) {
        return false;
    }

    // Move all fragments into target buffer
    uint8_t offs = 0;
    _inv->DevInfo()->beginAppendFragment();
    _inv->DevInfo()->clearBufferAll();
    for (uint8_t i = 0; i < max_fragment_id; i++) {
        _inv->DevInfo()->appendFragmentAll(offs, fragment[i].fragment, fragment[i].len);
        offs += (fragment[i].len);
    }
    _inv->DevInfo()->endAppendFragment();
    _inv->DevInfo()->setLastUpdateAll(millis());
    return true;
}
