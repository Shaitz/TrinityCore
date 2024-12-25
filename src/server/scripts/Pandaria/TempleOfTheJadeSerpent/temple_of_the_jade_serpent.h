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

#ifndef DEF_TEMPLE_OF_THE_JADE_SERPENT_H
#define DEF_TEMPLE_OF_THE_JADE_SERPENT_H

#include "CreatureAIImpl.h"

constexpr char const* DataHeader = "TOTJS";
constexpr char const* TempleOfTheJadeSerpentScriptName = "instance_temple_of_the_jade_serpent";

constexpr uint32 EncounterCount = 4;

enum TempleOfTheJadeSerpentDataTypes
{
    DATA_WISE_MARI              = 0,
    DATA_LOREWALKER_STONESTEP   = 1,
    DATA_LIU_FLAMEHEART         = 2,
    DATA_SHA_OF_DOUBT           = 3,

    DATA_STRIFE,
    DATA_PERIL,
    DATA_YU_LON_THE_JADE_SERPENT,
};

enum TempleOfTheJadeSerpentCreatureIds
{
    BOSS_WISE_MARI                  = 56448,
    BOSS_STRIFE                     = 59051,
    BOSS_PERIL                      = 59726,
    BOSS_LIU_FLAMEHEART             = 56732,
    BOSS_YU_LON_THE_JADE_SERPENT    = 56762,
    BOSS_SHA_OF_DOUBT               = 56439,
};

enum TempleOfTheJadeSerpentGameObjectIds
{
    GO_WATERY_DOOR                      = 211280,
    GO_DOOR_TO_COURTYARD                = 213544,
    GO_DOOR_TO_SCROLLKEEPERS_SANCTUM    = 213545,
    GO_SCROLLKEEPERS_SANCTUM_EXIT       = 213549,
    GO_DOOR_TO_FOUNTAIN_OF_EVERSEEING   = 213547,
    GO_FOUNTAIN_OF_EVERSEEING_EXIT      = 213550,
    GO_DOOR_TO_SHA_OF_DOUBT             = 213548,
    GO_FRONT_DOOR_2                     = 213903,
    GO_LIN_FAMILY_SCROLL                = 221413,
};

template <class AI, class T>
inline AI* GetTempleOfTheJadeSerpentAI(T* obj)
{
    return GetInstanceAI<AI>(obj, TempleOfTheJadeSerpentScriptName);
}

#define RegisterTempleOfTheJadeSerpentCreatureAI(ai_name) RegisterCreatureAIWithFactory(ai_name, GetTempleOfTheJadeSerpentAI);

#endif
