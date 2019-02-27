#ifndef _ACTION_PROPERTY_H
#define _ACTION_PROPERTY_H

#include <Model/Interface/World.h>
#include <bits/stdc++.h>

using namespace std;

class ActionProperty
{
public:
  int priority;
  AbilityName type;
  Hero *originHero;
  Cell targetCell;
  int effect;
  vector<int> affected;

public:
  ActionProperty(int priority, AbilityName type, Hero *originHero, Cell targetCell, int effect) : priority(priority), type(type), originHero(originHero), targetCell(targetCell), effect(effect){};
  void addAffected(int heroId);
  bool operator<(const ActionProperty &rhs)
  {
    if (priority == rhs.priority)
    {
      return originHero->getAbility(type).getAPCost() < rhs.originHero->getAbility(type).getAPCost();
    }
    else
      return priority < rhs.priority;
  }
};

#endif