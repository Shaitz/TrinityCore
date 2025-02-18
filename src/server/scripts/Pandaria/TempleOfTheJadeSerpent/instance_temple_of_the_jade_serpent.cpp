/*
 * This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ScriptMgr.h"
#include "InstanceScript.h"
#include "temple_of_the_jade_serpent.h"

static BossBoundaryData const boundaries =
{

};
// wowtools
ObjectData const creatureData[] =
{
	{ BOSS_WISE_MARI,                   DATA_WISE_MARI },
	{ BOSS_STRIFE,                      DATA_STRIFE },
    { BOSS_PERIL,                       DATA_PERIL },
    { BOSS_LIU_FLAMEHEART,              DATA_LIU_FLAMEHEART },
    { BOSS_YU_LON_THE_JADE_SERPENT,     DATA_YU_LON_THE_JADE_SERPENT },
    { BOSS_SHA_OF_DOUBT,                DATA_SHA_OF_DOUBT },
    { 0,                                0 }  // END

};
// sniff sql
DoorData const doorData[] =
{
    { GO_WATERY_DOOR,                   DATA_WISE_MARI,             EncounterDoorBehavior::OpenWhenNotInProgress },
    { GO_FOUNTAIN_OF_EVERSEEING_EXIT,   DATA_WISE_MARI,             EncounterDoorBehavior::OpenWhenDone },
    { GO_SCROLLKEEPERS_SANCTUM_EXIT,    DATA_LOREWALKER_STONESTEP,  EncounterDoorBehavior::OpenWhenDone },
    { GO_DOOR_TO_COURTYARD,             DATA_SHA_OF_DOUBT,          EncounterDoorBehavior::OpenWhenDone },
    { GO_DOOR_TO_SHA_OF_DOUBT,          DATA_LIU_FLAMEHEART,        EncounterDoorBehavior::OpenWhenDone }
};
// wowtools encounters
DungeonEncounterData const encounters[] =
{
    { DATA_WISE_MARI,               {{ 1418 }} },
    { DATA_LOREWALKER_STONESTEP,    {{ 1417 }} },
    { DATA_LIU_FLAMEHEART,          {{ 1416 }} },
    { DATA_SHA_OF_DOUBT,            {{ 1439 }} },
};

class instance_temple_of_the_jade_serpent : public InstanceMapScript
{
public:
	instance_temple_of_the_jade_serpent() : InstanceMapScript("instance_temple_of_the_jade_serpent", 960) { }

    struct instance_temple_of_the_jade_serpent_InstanceMapScript : public InstanceScript
    {
        instance_temple_of_the_jade_serpent_InstanceMapScript(InstanceMap* map) : InstanceScript(map)
        {
            SetHeaders(DataHeader);
            SetBossNumber(EncounterCount);
            LoadObjectData(creatureData, nullptr);
            LoadDoorData(doorData);
            LoadBossBoundaries(boundaries);
            LoadDungeonEncounterData(encounters);
        }
    };

    InstanceScript* GetInstanceScript(InstanceMap* map) const override
    {
        return new instance_temple_of_the_jade_serpent_InstanceMapScript(map);
    }
};

void AddSC_instance_temple_of_the_jade_serpent()
{
	new instance_temple_of_the_jade_serpent();
}
