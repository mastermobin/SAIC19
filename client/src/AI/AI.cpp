#include <stdlib.h>
#include <bits/stdc++.h>
#include <chrono>
#include "AI.h"
#include "Utils.h"
#include "Attack.h"

#define mp make_pair
#define pb push_back

using namespace std;
using namespace std::chrono;

void AI::preProcess(World *world)
{
    preProc(world);
}

void AI::pick(World *world)
{
    cerr << "-- Pick" << endl;
    static int cnt = 0;

    switch (cnt)
    {
    case 0:
        world->pickHero(HeroName::BLASTER);
        break;
    case 1:
        world->pickHero(HeroName::BLASTER);
        break;
    case 2:
        world->pickHero(HeroName::BLASTER);
        break;
    case 3:
        world->pickHero(HeroName::BLASTER);
        break;
    default:
        world->pickHero(HeroName::BLASTER);
    }
    cnt++;
}

void AI::move(World *world)
{
    cerr << "----- Move Step; " << world->getCurrentTurn() - 4 << " : " << world->getMovePhaseNum() << endl;

    milliseconds ms1 = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    deadProc(world);
    processCasts(world);
    createDamageMap(world);

    for (Hero *hero : world->getMyHeroes())
    {
        Direction movement = BlasterMove(world, hero);
        if (movement != NULL_DIRECTION)
        {
            world->moveHero(*hero, movement);
        }
    }
    milliseconds ms2 = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    cerr << "Time Elapsed : " << (ms2 - ms1).count() << endl;
}

void AI::action(World *world)
{
    cerr << "----- Action Step; " << world->getCurrentTurn() - 4 << " : " << world->getMovePhaseNum() << endl;
    milliseconds ms1 = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    deadProc(world);
    decreaseCooldown();
    createDamageMap(world);

    for (Hero *hero : world->getMyHeroes())
    {
        if (hero->getName() == HEALER)
        {
            vector<ActionProperty> movements = HealerFitness(world, hero);
            if (!movements.empty())
            {
                ActionProperty ap = movements[0];
                addDamageToAffected(ap.affected, ap.originHero->getAbility(ap.type).getPower());
                cerr << "I'm Using " << ((ap.type == HEALER_ATTACK) ? "Normal Attack" : ((ap.type == HEALER_HEAL) ? "Heal" : "Dodge")) << " At ("
                     << ap.targetCell.getRow() << "," << ap.targetCell.getColumn() << ") And ";
                for (int e : ap.affected)
                {
                    cerr << getHero(world->getHero(e).getName()) << ", ";
                }
                cerr << "Affected!!" << endl;

                world->castAbility(*hero, ap.type, ap.targetCell);
            }
        }
    }

    for (Hero *hero : world->getMyHeroes())
    {
        if (hero->getName() == BLASTER)
        {
            vector<ActionProperty> movements = BlasterFitness(world, hero);
            if (!movements.empty())
            {
                ActionProperty ap = movements[0];
                cerr << "I'm Using " << ((ap.type == BLASTER_ATTACK) ? "Normal Attack" : ((ap.type == BLASTER_BOMB) ? "Bomb" : "Dodge")) << " At ("
                     << ap.targetCell.getRow() << "," << ap.targetCell.getColumn() << ") And ";
                for (int e : ap.affected)
                {
                    cerr << getHero(world->getHero(e).getName()) << ", ";
                }
                cerr << "Affected!!" << endl;

                world->castAbility(*hero, ap.type, ap.targetCell);
            }
        }
    }

    for (Hero *hero : world->getMyHeroes())
    {
        if (hero->getName() == HEALER)
        {
            ActionProperty *movement = HealerHealFree(world, hero);
            if (movement != NULL)
            {
                ActionProperty ap = *movement;
                cerr << "I'm Using " << ((ap.type == HEALER_ATTACK) ? "Normal Attack" : ((ap.type == HEALER_HEAL) ? "Heal" : "Dodge")) << " At ("
                     << ap.targetCell.getRow() << "," << ap.targetCell.getColumn() << ") And ";
                for (int e : ap.affected)
                {
                    cerr << getHero(world->getHero(e).getName()) << ", ";
                }
                cerr << "Affected!!" << endl;

                world->castAbility(*hero, ap.type, ap.targetCell);
                delete movement;
            }
        }
    }

    milliseconds ms2 = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    cerr << "Time Elapsed : " << (ms2 - ms1).count() << endl;
}