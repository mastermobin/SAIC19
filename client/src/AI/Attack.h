#ifndef _ATTACK_H
#define _ATTACK_H

#include <Model/Interface/World.h>
#include "Intersection.h"
#include "ActionProperty.h"
#include "Utils.h"

bool isInDanger(Hero *hero, int row, int col);

void setPlace(World *world, Hero *hero);
void processCasts(World *world);
void decreaseCooldown();
void createDamageMap(World *world);
void getDamageMap(int cp[][33]);

Direction BlasterMove(World *world, Hero *hero);
vector<ActionProperty> BlasterFitness(World *world, Hero *hero);

int SentryFitness(World *world);

Direction HealerMove(World *world, Hero *hero);
ActionProperty *HealerHealFree(World *world, Hero *hero);
vector<ActionProperty> HealerFitness(World *world, Hero *hero);

int GuardianFitness(World *world);

#endif // !_ATTACK_H