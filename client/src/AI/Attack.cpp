#include <bits/stdc++.h>
#include "Attack.h"

using namespace std;
World *ourWorld;

class Intersection
{
  public:
    set<int> members;
    set<pair<int, int>> cells;

  public:
    void addCell(int row, int col)
    {
        if (ourWorld->getMap().isInMap(row, col))
            cells.insert(make_pair(row, col));
    }

    void addMember(int id)
    {
        members.insert(id);
    }

    bool isEmpty()
    {
        return cells.empty();
    }
};

Intersection createArea(int row, int col, int radius)
{
    Intersection ans;
    for (int i = 0; i <= radius; i++)
        for (int j = 0; j <= radius - i; j++)
        {
            ans.addCell(row + i, col + j);
            ans.addCell(row + i, col - j);
            ans.addCell(row - i, col + j);
            ans.addCell(row - i, col - j);
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

vector<ActionProperty> BlasterAttack(World *world, Hero *hero, AbilityName ability)
{
    vector<ActionProperty> ans;
    vector<Hero *> enemies = world->getOppHeroes();
    Ability attackAbility = hero->getAbility(ability);
    int aoe = attackAbility.getAreaOfEffect();
    int range = attackAbility.getRange();
    int attackRange = aoe + range;
    int po = attackAbility.getPower();

    Intersection throwRange = createArea(hero->getCurrentCell().getRow(), hero->getCurrentCell().getColumn(), range);
    vector<ActionProperty> temp;
    vector<Hero *> availableEnemies;

    for (auto &enemy : enemies)
    {
        if (enemy->getCurrentCell().getRow() == -1)
            continue;

        cerr << "Offensive Abilities Count : " << enemy->getOffensiveAbilities().size() << endl;
        int dist = world->manhattanDistance(hero->getCurrentCell(), enemy->getCurrentCell());
        if (dist > attackRange)
            continue;
        availableEnemies.push_back(enemy);
    }

    cerr << "I Am In (" << hero->getCurrentCell().getRow() << "," << hero->getCurrentCell().getColumn() << ") And See " << availableEnemies.size() << " Enemy(es) Close !!" << endl;

    vector<Intersection> intersection[4];
    for (unsigned int i = 0; i < availableEnemies.size(); i++)
    {
        Intersection temp = createArea(availableEnemies[i]->getCurrentCell().getRow(), availableEnemies[i]->getCurrentCell().getColumn(), aoe);
        temp.addMember(availableEnemies[i]->getId());
        intersection[0].push_back(temp);
    }

    unsigned int k;
    bool fully = false;
    for (k = 1; k < 4; k++)
    {
        for (unsigned int i = 0; i < intersection[k - 1].size() && !fully; i++)
        {
            for (unsigned int j = i + 1; j < intersection[k - 1].size() && !fully; j++)
            {
                Intersection t = getIntersection(intersection[k - 1][i], intersection[k - 1][j]);
                if (!t.isEmpty() && isIntersected(t, throwRange))
                {
                    intersection[k].push_back(t);
                    if (k == 3)
                    {
                        fully = true;
                    }
                }
            }
        }
        if (intersection[k].empty())
            break;
        cerr << "Level " << k << " Intersection Found !!" << endl;
    }

    k--;

    for (unsigned int i = 0; i < intersection[k].size(); i++)
    {
        int score = 0;
        for (auto mem : intersection[k][i].members)
        {
            if (po > world->getHero(mem).getCurrentHP())
                score += 100000 + world->getHero(mem).getCurrentHP();
            else
                score += po;
        }

        Intersection finalIntersect = getIntersection(throwRange, intersection[k][i]);
        pair<int, int> ad = *(finalIntersect.cells.begin());
        Cell tarcell = world->getMap().getCell(ad.first, ad.second);
        ans.push_back(ActionProperty(score / 100000, ability, hero, tarcell, score % 100000));
    }

    sort(ans.begin(), ans.end());

    return ans;
}

vector<ActionProperty> BlasterFitness(World *world, Hero *hero)
{
    ourWorld = world;
    vector<ActionProperty> ans;
    if (hero->getAbility(BLASTER_BOMB).isReady())
    {
        vector<ActionProperty> bombVector = BlasterAttack(world, hero, BLASTER_BOMB);
        ans.insert(ans.end(), bombVector.begin(), bombVector.end());
    }

    return ans;
}