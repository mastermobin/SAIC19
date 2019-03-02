#ifndef _INTERSECTION_H
#define _INTERSECTION_H

#include <Model/Interface/World.h>
#include <bits/stdc++.h>

using namespace std;

class Intersection
{
public:
  set<int> members;
  set<pair<int, int>> cells;

public:
  void addLinearCell(int sr, int sc, int row, int col, World *world, AbilityName ability);
  void addCell(int row, int col, World *world);
  void addCell(int row, int col);
  void addMember(int id);
  bool isEmpty();
  bool operator<(const Intersection &rhs) const
  {
    return cells.size() < rhs.cells.size();
  }
};

Intersection createArea(int row, int col, int radius, World *world);
Intersection createLinearArea(int row, int col, int radius, World *world, AbilityName ability);
Intersection getIntersection(Intersection in1, Intersection in2);
bool isIntersected(Intersection in1, Intersection in2);
Intersection addIntersection(Intersection in1, Intersection in2);
Intersection subIntersection(Intersection in1, Intersection in2);

#endif