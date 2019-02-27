#include <bits/stdc++.h>
#include "Intersection.h"

using namespace std;

void Intersection::addCell(int row, int col, World *world)
{
    if (world->getMap().isInMap(row, col))
        cells.insert(make_pair(row, col));
}

void Intersection::addMember(int id)
{
    members.insert(id);
}

bool Intersection::isEmpty()
{
    return cells.empty();
}