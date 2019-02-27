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
  int score;
  vector<int> affected;

public:
  ActionProperty(int priority, AbilityName type, Hero *originHero, Cell targetCell, int score) : priority(priority), type(type), originHero(originHero), targetCell(targetCell), score(score){};
  void addAffected(int heroId);
  bool operator<(const ActionProperty &rhs)
  {
    if (priority == rhs.priority)
    {
      if (score == rhs.score)
      {
        return originHero->getAbility(type).getAPCost() < rhs.originHero->getAbility(type).getAPCost();
      }
      else
      {
        return score > rhs.score;
      }
    }
    else
      return priority < rhs.priority;
  }
};

#endif