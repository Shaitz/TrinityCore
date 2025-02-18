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
#include <Containers.h>
#include "CellImpl.h"
#include "GridNotifiersImpl.h"

enum Spells
{
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

    SPELL_WATER_BUBBLE                  = 106062,
    SPELL_WATER_BUBBLE2                 = 106079,
    SPELL_WATER_BUBBLE_BURST            = 396874,
    SPELL_DNT_WATER_BUBBLE_REDUCTION    = 396880,

    SPELL_CALL_WATER                    = 106526,
    SPELL_SERVERSIDE_NONDB2_CALL_WATER  = 106461,
    SPELL_CALL_WATER_TRIGGERED          = 106462,

    SPELL_WASH_AWAY_DUMMY               = 115575,
    SPELL_WASH_AWAY_AURA                = 106331,
    SPELL_WASH_AWAY_TRIGGERED           = 106334,

    SPELL_DRY                           = 128437,

    SPELL_QUIET_SUICIDE                 = 115372
};

enum Events
{
    EVENT_WATER_BUBBLE      = 1,
    EVENT_CORRUPTING_WATERS = 2,
    EVENT_CALL_WATER        = 3,
    EVENT_WASH_AWAY         = 4,
    EVENT_WASH_AWAY_FACE    = 5
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
    NPC_CORRUPT_LIVING_WATER                    = 56511,
    SAY_CORRUPT_LIVING_WATER_SPAWN              = 0,

    EVENT_TAINTED_RIPPLE                        = 1,
    EVENT_SURGING_DELUGE                        = 2,
    EVENT_SUMMON                                = 3,

    SPELL_CORRUPT_FOUNTAIN                      = 106519,
    SPELL_TAINTED_RIPPLE                        = 397878,
    SPELL_SURGING_DELUGE_DUMMY                  = 397880,
    SPELL_SURGING_DELUGE                        = 397881,
    SPELL_SUMMON_CORRUPT_DROPLET_TRIGGER        = 123983,
    SPELL_SUMMON_CORRUPT_DROPLET                = 121517,

    SPELL_SHA_RESIDUE_AT                        = 395828,
    SPELL_SHA_RESIDUE_AURA                      = 395829,

    NPC_EAST_TEMPLE_CORRUPTED_WATERS_STALKER_MW = 65393,
    NPC_SPLASH_STALKER                          = 56542,
    NPC_FOUNTAIN_STALKER                        = 56586,
    NPC_FIREHOSE_TARGET                         = 56574
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
        //events.ScheduleEvent(EVENT_CORRUPTING_WATERS, 2s);
        events.ScheduleEvent(EVENT_CALL_WATER, 6s);
    }

    void SummonedCreatureDies(Creature* summon, Unit* /*killer*/) override
    {
        if (summon->GetEntry() == NPC_CORRUPT_LIVING_WATER)
        {
            if (Aura* aura = me->GetAura(SPELL_WATER_BUBBLE))
            {
                me->CastSpell(me, SPELL_DNT_WATER_BUBBLE_REDUCTION);
                me->CastSpell(me, SPELL_WATER_BUBBLE_BURST);
            }

            if (me->HasAura(SPELL_WATER_BUBBLE))
                events.ScheduleEvent(EVENT_CALL_WATER, 5s);
            else
                events.ScheduleEvent(EVENT_WASH_AWAY, 4s);
        }
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
                DoCastSelf(SPELL_WATER_BUBBLE);
                break;

            case EVENT_CORRUPTING_WATERS:
                DoCastSelf(SPELL_HYDROLANCE_PRECAST_BOTTOM);
                events.ScheduleEvent(EVENT_CORRUPTING_WATERS, 5s);
                break;

            case EVENT_CALL_WATER:
            {
                Talk(SAY_SUMMON_CORRUPT_LIVING_WATER);

                std::list<Creature*> creatures;
                me->GetCreatureListWithEntryInGrid(creatures, NPC_FOUNTAIN_STALKER);
                if (!creatures.empty())
                {
                    Creature* creature = Trinity::Containers::SelectRandomContainerElement(creatures);
                    me->SetFacingToObject(creature);
                    me->UpdatePosition(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation());
                    DoCast(creature, SPELL_CALL_WATER);
                }
                break;
            }

            case EVENT_WASH_AWAY:
            {
                Talk(SAY_ANNOUNCE_TORRENTIAL_WATERS);
                DoCastSelf(SPELL_WASH_AWAY_AURA);
                events.ScheduleEvent(EVENT_WASH_AWAY_FACE, 0s);
                break;
            }

            case EVENT_WASH_AWAY_FACE:
            {
                Creature* creature = me->FindNearestCreature(NPC_FIREHOSE_TARGET, 100.0f);
                me->SetFacingToObject(creature);
                me->UpdatePosition(me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation());
                events.ScheduleEvent(EVENT_WASH_AWAY_FACE, 0s);
                break;
            }

            default:
                break;
        }
    }
};

// 106062
class spell_water_bubble : public AuraScript
{
    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WATER_BUBBLE2 });
    }

    void OnApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Aura* aura = GetCaster()->GetAura(SPELL_WATER_BUBBLE))
            aura->SetStackAmount(4);
    }

    void HandlePeriodic(AuraEffect const* /*aurEff*/)
    {
        PreventDefaultAction();

        if (Unit* caster = GetCaster())
            if (!GetTarget()->IsFalling())
                caster->CastSpell(GetTarget(), SPELL_WATER_BUBBLE2, true);
    }

    void Register() override
    {
        AfterEffectApply += AuraEffectApplyFn(spell_water_bubble::OnApply, EFFECT_1, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN, AURA_EFFECT_HANDLE_REAL);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_water_bubble::HandlePeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
    }
};

// 106331
class spell_wash_away_aura : public AuraScript
{
    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_WASH_AWAY_TRIGGERED });
    }

    void HandlePeriodicTick(AuraEffect const* /*aurEff*/)
    {
        PreventDefaultAction();

        if (Unit* caster = GetCaster())
        {
            Map::PlayerList const& players = caster->GetMap()->GetPlayers();
            for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
            {
                if (Player* player = itr->GetSource())
                    if (caster->isInFront(player, M_PI / 4) && !player->IsFalling())
                        caster->CastSpell(player, SPELL_WASH_AWAY_TRIGGERED, true);
            }
        }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_wash_away_aura::HandlePeriodicTick, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
    }
};

// 115229
class spell_hydrolance_precast_bottom : public SpellScript
{
    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([](WorldObject* obj) { return obj->GetEntry() != NPC_SPLASH_STALKER; });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_hydrolance_precast_bottom::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENTRY);
    }
};

// 56511
struct npc_corrupt_living_water : public ScriptedAI
{
    npc_corrupt_living_water(Creature* creature) : ScriptedAI(creature) { }

    void IsSummonedBy(WorldObject* summoner) override
    {
        DoZoneInCombat();
        Talk(SAY_CORRUPT_LIVING_WATER_SPAWN);

        _events.ScheduleEvent(EVENT_TAINTED_RIPPLE, 3s);
        _events.ScheduleEvent(EVENT_SURGING_DELUGE, 6s);
    }

    void JustDied(Unit* /*killer*/) override
    {
        DoCastSelf(SPELL_SUMMON_CORRUPT_DROPLET);
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        _events.Update(diff);

        switch (_events.ExecuteEvent())
        {
            case EVENT_TAINTED_RIPPLE:
                DoCastSelf(SPELL_TAINTED_RIPPLE);
                _events.ScheduleEvent(EVENT_TAINTED_RIPPLE, 14s);
                break;

            case EVENT_SURGING_DELUGE:
                DoCastSelf(SPELL_SURGING_DELUGE_DUMMY);
                _events.ScheduleEvent(EVENT_SURGING_DELUGE, 16s);
                break;

            default:
                break;
        }
    }

private:
    ObjectGuid _summonerGUID;
    EventMap _events;
};

// 397880
class spell_surging_deluge : public SpellScript
{
    bool Validate(SpellInfo const* /*spellinfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SURGING_DELUGE });
    } 

    void HandleHit(SpellEffIndex /*effIndex*/)
    {
        GetCaster()->CastSpell(GetHitUnit(), SPELL_SURGING_DELUGE);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_surging_deluge::HandleHit, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

class spell_summon_corrupt_droplet : public SpellScript
{
    bool Validate(SpellInfo const* /*spellinfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SUMMON_CORRUPT_DROPLET });
    }

    void HandleTriggerMissile(SpellEffIndex /*effIndex*/)
    {
        GetCaster()->CastSpell(GetCaster(), SPELL_SUMMON_CORRUPT_DROPLET);
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_summon_corrupt_droplet::HandleTriggerMissile, EFFECT_0, SPELL_EFFECT_TRIGGER_MISSILE);
    }
};

struct npc_corrupt_droplet : public ScriptedAI
{
    npc_corrupt_droplet(Creature* creature) : ScriptedAI(creature) { }

    void IsSummonedBy(WorldObject* /*summoner*/)
    {
        DoZoneInCombat();
    }

    void JustDied(Unit* /*killer*/)
    {

    }
};

void AddSC_boss_wise_mari()
{
    RegisterTempleOfTheJadeSerpentCreatureAI(boss_wise_mari);
    RegisterTempleOfTheJadeSerpentCreatureAI(npc_corrupt_living_water);
    //RegisterTempleOfTheJadeSerpentCreatureAI(npc_corrupt_droplet);
    //RegisterTempleOfTheJadeSerpentCreatureAI(npc_east_temple_corrupted_waters_stalker_mw);

    RegisterSpellScript(spell_water_bubble);
    //RegisterSpellScript(spell_hydrolance_precast_bottom);
    RegisterSpellScript(spell_summon_corrupt_droplet);
    RegisterSpellScript(spell_surging_deluge);
    RegisterSpellScript(spell_wash_away_aura);
}
