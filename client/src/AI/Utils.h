#ifndef _UTILS_H
#define _UTILS_H

#include <Model/Interface/World.h>
#include "Attack.h"

#define mp make_pair
#define pb push_back

using namespace std;

AbilityConstants getAbilityConstants(AbilityName abilityName, World *world);
void printLine();
string getHero(int hero);
string getDirection(int dir);
int dist(Cell c1, Cell c2);
void preProc(World *world);
Direction moveTowardTheZone(World *world, Hero *hero);
void deadProc(World *world);

#endif // !_UTILS_H
