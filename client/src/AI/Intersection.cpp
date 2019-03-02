#include <bits/stdc++.h>
#include "Intersection.h"

using namespace std;

void Intersection::addCell(int row, int col, World *world)
{
    if (world->getMap().isInMap(row, col))
        cells.insert(make_pair(row, col));
}

void Intersection::addLinearCell(int sr, int sc, int row, int col, World *world, AbilityName ability)
{
    Cell impactCell = world->getImpactCell(ability, sr, sc, row, col);
    if (world->getMap().isInMap(row, col) && (impactCell == world->getMap().getCell(row, col)))
        cells.insert(make_pair(row, col));
}

void Intersection::addCell(int row, int col)
{
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

Intersection createArea(int row, int col, int radius, World *world)
{
    Intersection ans;
    for (int i = 0; i <= radius; i++)
        for (int j = 0; j <= radius - i; j++)
        {
            ans.addCell(row + i, col + j, world);
            ans.addCell(row + i, col - j, world);
            ans.addCell(row - i, col + j, world);
            ans.addCell(row - i, col - j, world);
        }
    return ans;
}

Intersection createLinearArea(int row, int col, int radius, World *world, AbilityName ability)
{
    Intersection ans;
    for (int i = 0; i <= radius; i++)
        for (int j = 0; j <= radius - i; j++)
        {
            ans.addLinearCell(row, col, row + i, col + j, world, ability);
            ans.addLinearCell(row, col, row + i, col - j, world, ability);
            ans.addLinearCell(row, col, row - i, col + j, world, ability);
            ans.addLinearCell(row, col, row - i, col - j, world, ability);
        }
    return ans;
}

Intersection getIntersection(Intersection in1, Intersection in2)
{
    vector<pair<int, int>> v_intersection;

    set_intersection(in1.cells.begin(), in1.cells.end(),
                     in2.cells.begin(), in2.cells.end(),
                     back_inserter(v_intersection));

    Intersection ans;
    for (auto p : v_intersection)
    {
        ans.addCell(p.first, p.second);
    }

    for (auto mem : in1.members)
        ans.addMember(mem);

    for (auto mem : in2.members)
        ans.addMember(mem);

    return ans;
}

bool isIntersected(Intersection in1, Intersection in2)
{
    vector<pair<int, int>> v_intersection;

    set_intersection(in1.cells.begin(), in1.cells.end(),
                     in2.cells.begin(), in2.cells.end(),
                     back_inserter(v_intersection));

    return !v_intersection.empty();
}

Intersection addIntersection(Intersection in1, Intersection in2)
{

    Intersection ans = in1;
    for (auto p : in2.cells)
    {
        ans.addCell(p.first, p.second);
    }

    for (auto mem : in2.members)
        ans.addMember(mem);

    return ans;
}

Intersection subIntersection(Intersection in1, Intersection in2)
{

    vector<pair<int, int>> v_diff;

    set_difference(in1.cells.begin(), in1.cells.end(),
                   in2.cells.begin(), in2.cells.end(),
                   back_inserter(v_diff));

    Intersection ans;
    for (auto p : v_diff)
    {
        ans.addCell(p.first, p.second);
    }

    for (auto mem : in1.members)
        ans.addMember(mem);

    return ans;
}