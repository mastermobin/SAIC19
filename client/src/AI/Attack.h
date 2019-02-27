#include <Model/Interface/World.h>
#include "Intersection.h"
#include "ActionProperty.h"

int BlasterAttack(World *world);
vector<ActionProperty> BlasterFitness(World *world, Hero *hero);

int SentryAttack(World *world);
int SentryFitness(World *world);

int HealerAttack(World *world);
int HealerFitness(World *world);

int GuardianAttack(World *world);
int GuardianFitness(World *world);