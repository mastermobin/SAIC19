#include <stdlib.h>
#include <bits/stdc++.h>
#include "AI.h"
#include "Utils.h"
#include "Attack.h"

#define mp make_pair
#define pb push_back

using namespace std;

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
        world->pickHero(HeroName::GUARDIAN);
        break;
    case 2:
        world->pickHero(HeroName::HEALER);
        break;
    case 3:
        world->pickHero(HeroName::SENTRY);
        break;
    default:
        world->pickHero(HeroName::BLASTER);
    }
    cnt++;
}

void AI::move(World *world)
{
    deadProc(world);
    moveTowardTheZone(world);
}

void AI::action(World *world)
{
    cerr << "----- Action Step; " << world->getCurrentTurn() << " : " << world->getCurrentPhase() << endl;
    deadProc(world);
    for (Hero *hero : world->getMyHeroes())
    {
        if (hero->getName() == BLASTER)
        {
            vector<ActionProperty> movements = BlasterFitness(world, hero);
            if (!movements.empty())
            {
                ActionProperty ap = movements[0];
                cerr << ap.type << "; " << ap.targetCell.getRow() << " : " << ap.targetCell.getColumn() << endl;
                world->castAbility(*hero, ap.type, ap.targetCell);
            }
        }
    }
}