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
#include "temple_of_the_jade_serpent.h"

enum Spells
{
    SPELL_CORRUPTED_WATER_NOT_READY     = 106778,
    SPELL_CORRUPTED_WATERS              = 115165,
    SPELL_CORRUPTING_WATERS_DUMMY       = 106055,
    SPELL_CORRUPTING_WATER_BURST        = 106104,

    SPELL_HYDROLANCE_DUMMY              = 106097,
    SPELL_HYDROLANCE_DUMMY2             = 106098,
    SPELL_HYDROLANCE_DUMMY3             = 106241,
    SPELL_HYDROLANCE_SPLASH_STALKER     = 106250,
    SPELL_HYDROLANCE_PULSE              = 106267,
    SPELL_HYDROLANCE_PULSE2             = 106319,
    SPELL_HYDROLANCE_PRECAST            = 115220,
    SPELL_HYDROLANCE_PRECAST_TOP        = 115227,
    SPELL_HYDROLANCE_PRECAST_BOTTOM     = 115229,
    SPELL_HYDROLANCE                    = 396904,
    SPELL_TRIGGERED_HYDROLANCE          = 396906,
    SPELL_HYDROLANCE_IDK                = 397888,
    SPELL_HYDROLANCE_SPLASH             = 106216,

    SPELL_BUBBLE_BURST                  = 106612, // remove aura, knockback, dmg when bubble burst
    SPELL_WATER_BUBBLE                  = 106062, // aura, in sniff [0] Applications: 4
    SPELL_WATER_BUBBLE2                 = 106079, // triggered by 106062, knockback when getting close?, always cast on loc (Cast) (Target) (SrcLocation) Location: X: 1047.0104 Y: -2560.2588 Z: 174.24026
    SPELL_WATER_BUBBLE_BURST            = 396874, // dmg, snare when bubble burst
    SPELL_DNT_WATER_BUBBLE_REDUCTION    = 396880,

    SPELL_CALL_WATER                    = 106526,
    SPELL_SERVERSIDE_NONDB2_CALL_WATER  = 106461,
    SPELL_CALL_WATER_TRIGGERED          = 106462,
    SPELL_CORRUPT_FOUNTAIN              = 106519,

    SPELL_WASH_AWAY_DUMMY               = 115575, // HITS FIREHOSE TARGET
    SPELL_WASH_AWAY                     = 106331,
    SPELL_WASH_AWAY2                    = 106334,
    SPELL_WASH_AWAY_START               = 106329,
    SPELL_WASH_AWAY_MAYBE               = 397785,

    SPELL_DRY                           = 128437,

    SPELL_QUIET_SUICIDE                 = 115372
};

enum Events
{
    EVENT_WATER_BUBBLE      = 1,
    EVENT_CORRUPTING_WATERS = 2,
    EVENT_CALL_WATER        = 3,
    //EVENT_HYDROLANCE        = 4,
    EVENT_WASH_AWAY         = 5
};

enum Texts
{
    SAY_AGGRO                           = 0,
    SAY_SUMMON_CORRUPT_LIVING_WATER     = 1,
    SAY_START_TORRENTIAL_WATERS         = 2,
    SAY_ANNOUNCE_TORRENTIAL_WATERS      = 3,
    SAY_TAUNT                           = 4,
    SAY_DEATH1                          = 5,
    SAY_DEATH2                          = 6,
    SAY_DEATH3                          = 7,
    SAY_ANNOUNCE_CORRUPT_LIVING_WATER   = 8
};

enum Misc
{
    NPC_CORRUPT_LIVING_WATER = 56511,
    NPC_EAST_TEMPLE_CORRUPTED_WATERS_STALKER_MW = 65393,
    NPC_
};

struct boss_wise_mari : public BossAI
{
    boss_wise_mari(Creature* creature) : BossAI(creature, DATA_WISE_MARI) { }

    void JustDied(Unit* /*killer*/) override
    {
        _JustDied();
    }

    void JustEngagedWith(Unit* who) override
    {
        BossAI::JustEngagedWith(who);
        SetCombatMovement(false);
        me->SetCanMelee(false);
        Talk(SAY_AGGRO);

        events.ScheduleEvent(EVENT_WATER_BUBBLE, 0s);
        events.ScheduleEvent(EVENT_CORRUPTING_WATERS, 2s);
        events.ScheduleEvent(EVENT_CALL_WATER, 3s);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        switch (events.ExecuteEvent())
        {
            case EVENT_WATER_BUBBLE:
                DoCastSelf(SPELL_WATER_BUBBLE); // hits multiple times in the air when getting close. Only has 1 charge
                break;

            case EVENT_CORRUPTING_WATERS:
                break;

            case EVENT_CALL_WATER:
                break;

            case EVENT_WASH_AWAY:
                break;
        }
    }
};

// add to db spell_script_names
class spell_water_bubble : public AuraScript
{
    // make OnApply for 4 stacks
    void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Aura* aura = GetCaster()->GetAura(SPELL_WATER_BUBBLE))
            aura->SetStackAmount(4);
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_water_bubble::OnApply, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL); // spell_aura_periodic_trigger_spell id 23 in spellwork
    }
    //void OnRemove();
};

struct npc_east_temple_corrupted_waters_stalker_mw : public ScriptedAI
{
    npc_east_temple_corrupted_waters_stalker_mw(Creature* creature) : ScriptedAI(creature) { }
};

struct npc_corrupt_living_water : public ScriptedAI
{
    npc_corrupt_living_water(Creature* creature) : ScriptedAI(creature) { }
};

void AddSC_boss_wise_mari()
{
    RegisterTempleOfTheJadeSerpentCreatureAI(boss_wise_mari);
    //RegisterTempleOfTheJadeSerpentCreatureAI(npc_corrupt_living_water);
    //RegisterTempleOfTheJadeSerpentCreatureAI(npc_east_temple_corrupted_waters_stalker_mw);

    RegisterSpellScript(spell_water_bubble);
}
