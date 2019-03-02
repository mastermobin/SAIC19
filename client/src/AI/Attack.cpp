#include <bits/stdc++.h>
#include "Attack.h"

using namespace std;

int damageMap[33][33] = {0};
static int lastHeal = -1;
static int delay[9][10] = {0};
// static bool enoughHealth[9] = {true, true, true, true, true, true, true, true, true};

void processCasts(World *world)
{
    vector<CastAbility *> casts = world->getOppCastAbilities();
    for (CastAbility *c : casts)
    {
        if (c->getCasterId() != -1)
            delay[c->getCasterId()][getAbilityConstants(c->getAbilityName(), world).getCooldown()] = getAbilityConstants(c->getAbilityName(), world).getCooldown();
    }
}

void decreaseCooldown()
{
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            if (delay[i][j] > 0)
                delay[i][j]--;
        }
    }
}

void addCell(int bitVal, int sr, int sc, int row, int col, int power, bool isLobbing, World *world, AbilityName ability)
{
    if (world->getMap().isInMap(row, col))
    {
        Cell impactCell = world->getImpactCell(ability, sr, sc, row, col);
        if (!isLobbing && (impactCell.getRow() != row || impactCell.getColumn() != col))
            return;
        if ((bitVal & damageMap[row][col]) != bitVal)
        {
            damageMap[row][col] |= bitVal;
            damageMap[row][col] += power;
        }
    }
}

void createDamageMap(World *world)
{
    memset(damageMap, 0, sizeof(damageMap));
    vector<ActionProperty> ans;
    vector<Hero *> enemies = world->getOppHeroes();

    for (auto &enemy : enemies)
    {
        if (enemy->getCurrentCell().getRow() == -1)
            continue;
        vector<pair<int, AbilityName>> offensiveAbilities;
        for (AbilityName abilityName : enemy->getAbilityNames())
        {
            AbilityConstants abilityCons = getAbilityConstants(abilityName, world);
            if (abilityCons.getType() != AbilityType::OFFENSIVE)
                continue;
            if (delay[enemy->getId()][abilityCons.getCooldown()] > 0)
                continue;
            offensiveAbilities.push_back(make_pair(abilityCons.getPower(), abilityName));
        }
        sort(offensiveAbilities.begin(), offensiveAbilities.end());
        reverse(offensiveAbilities.begin(), offensiveAbilities.end());

        int bitAdd = 10 + enemy->getId();
        int bitVal = (1 << bitAdd);

        for (pair<int, AbilityName> p : offensiveAbilities)
        {
            AbilityConstants abilityCons = getAbilityConstants(p.second, world);
            int row = enemy->getCurrentCell().getRow();
            int col = enemy->getCurrentCell().getColumn();
            int radius = abilityCons.getAreaOfEffect() + abilityCons.getRange();

            if (p.second == SENTRY_RAY)
                radius = 34;

            for (int i = 0; i <= radius; i++)
                for (int j = 0; j <= radius - i; j++)
                {
                    addCell(bitVal, row, col, row + i, col + j, abilityCons.getPower(), abilityCons.isLobbing(), world, abilityCons.getName());
                    addCell(bitVal, row, col, row + i, col - j, abilityCons.getPower(), abilityCons.isLobbing(), world, abilityCons.getName());
                    addCell(bitVal, row, col, row - i, col + j, abilityCons.getPower(), abilityCons.isLobbing(), world, abilityCons.getName());
                    addCell(bitVal, row, col, row - i, col - j, abilityCons.getPower(), abilityCons.isLobbing(), world, abilityCons.getName());
                }
        }

        for (int i = 0; i < 33; i++)
        {
            for (int j = 0; j < 33; j++)
            {
                damageMap[i][j] &= (-1 ^ bitVal);
            }
        }
    }

    // for (int i = 0; i < 33; i++)
    // {
    //     for (int j = 0; j < 33; j++)
    //     {
    //         cerr << setw(2) << damageMap[i][j] << " ";
    //     }
    //     cerr << endl;
    // }
}

void getDamageMap(int cp[][33])
{
    for (int i = 0; i < 33; i++)
    {
        for (int j = 0; j < 33; j++)
        {
            cp[i][j] = damageMap[i][j];
        }
    }
}

pair<int, int> findBestToDodge(Intersection in)
{
    pair<int, int> ans;
    int score = 1000000;
    for (pair<int, int> p : in.cells)
    {
        if (damageMap[p.first][p.second] < score)
        {
            score = damageMap[p.first][p.second];
            ans = p;
        }
    }
    return ans;
}

bool isInDanger(Hero *hero)
{
    return (((0.85 * damageMap[hero->getCurrentCell().getRow()][hero->getCurrentCell().getColumn()]) >= hero->getCurrentHP()) && hero->getCurrentHP() < 85);
}

bool isInDanger(Hero *hero, int row, int col)
{
    return (((0.85 * damageMap[hero->getCurrentCell().getRow()][hero->getCurrentCell().getColumn()]) >= hero->getCurrentHP()) && hero->getCurrentHP() < 85);
}

//Blaster ------------------------------------------------------------------------------------------------

vector<ActionProperty> BlasterAttack(World *world, Hero *hero, AbilityName ability)
{
    vector<ActionProperty> ans;
    vector<Hero *> enemies = world->getOppHeroes();
    Ability attackAbility = hero->getAbility(ability);
    int aoe = attackAbility.getAreaOfEffect();
    int range = attackAbility.getRange();
    int attackRange = aoe + range;
    int po = attackAbility.getPower();
    bool isLobbing = attackAbility.isLobbing();

    Intersection throwRange;
    if (isLobbing)
        throwRange = createArea(hero->getCurrentCell().getRow(), hero->getCurrentCell().getColumn(), range, world);
    else
        throwRange = createLinearArea(hero->getCurrentCell().getRow(), hero->getCurrentCell().getColumn(), range, world, ability);
    vector<ActionProperty> temp;
    vector<Hero *> availableEnemies;

    for (auto &enemy : enemies)
    {
        if (enemy->getCurrentCell().getRow() == -1)
            continue;

        int dist = world->manhattanDistance(hero->getCurrentCell(), enemy->getCurrentCell());
        if (dist > attackRange)
            continue;
        availableEnemies.push_back(enemy);
    }

    cerr << "I Am In (" << hero->getCurrentCell().getRow() << "," << hero->getCurrentCell().getColumn() << ") And See " << availableEnemies.size()
         << " Enemy(es) Close For " << ((ability == BLASTER_ATTACK) ? "Normal Attack" : "Bomb")
         << "!!" << endl;

    vector<Intersection> intersection[4];
    for (unsigned int i = 0; i < availableEnemies.size(); i++)
    {
        Intersection temp = createArea(availableEnemies[i]->getCurrentCell().getRow(), availableEnemies[i]->getCurrentCell().getColumn(), aoe, world);
        temp.addMember(availableEnemies[i]->getId());
        intersection[0].push_back(temp);
    }

    unsigned int k;
    for (k = 1; k < 4; k++)
    {
        for (unsigned int i = 0; i < intersection[k - 1].size(); i++)
        {
            for (unsigned int j = 0; j < intersection[0].size(); j++)
            {
                if (intersection[k - 1][i].members.find((*(intersection[0][j].members.begin()))) == intersection[k - 1][i].members.end())
                {
                    Intersection t = getIntersection(intersection[k - 1][i], intersection[0][j]);
                    if (!t.isEmpty() && isIntersected(t, throwRange))
                    {
                        intersection[k].push_back(t);
                    }
                }
            }
        }
        if (intersection[k].empty())
            break;
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
        ActionProperty temp = ActionProperty(1, ability, hero, tarcell, score);
        for (auto mem : intersection[k][i].members)
            temp.addAffected(mem);
        ans.push_back(temp);
    }

    return ans;
}

vector<ActionProperty> BlasterDodge(World *world, Hero *hero)
{
    vector<ActionProperty> ans;
    vector<Hero *> enemies = world->getOppHeroes();

    Intersection badPoints;
    bool inDanger = false;

    int row = hero->getCurrentCell().getRow();
    int col = hero->getCurrentCell().getColumn();
    int health = hero->getCurrentHP();

    inDanger = (damageMap[row][col] >= health);

    if (!inDanger)
        return ans;

    Intersection safeInZone;
    Intersection dodgeZone = createArea(hero->getCurrentCell().getRow(), hero->getCurrentCell().getColumn(), hero->getAbility(BLASTER_DODGE).getRange(), world);
    for (Cell *cell : world->getMap().getObjectiveZone())
    {
        int r = cell->getRow();
        int c = cell->getColumn();
        if (damageMap[r][c] < health)
            safeInZone.addCell(r, c);
    }
    safeInZone = getIntersection(safeInZone, dodgeZone);
    Intersection justSafe;
    for (pair<int, int> cell : dodgeZone.cells)
    {
        int r = cell.first;
        int c = cell.second;
        if (damageMap[r][c] < health)
            justSafe.addCell(r, c);
    }

    Intersection healerZone;
    for (Hero *h : world->getMyHeroes())
    {
        if (h->getName() == HEALER && h->getCurrentCell().getRow() != -1 && h->getAbility(HEALER_HEAL).isReady())
        {
            Intersection temp = createArea(h->getCurrentCell().getRow(), h->getCurrentCell().getColumn(), h->getAbility(HEALER_HEAL).getRange() + h->getAbility(HEALER_HEAL).getAreaOfEffect(), world);
            temp.addMember(h->getId());
            healerZone = addIntersection(healerZone, temp);
        }
    }

    pair<int, int> ad;
    if (!safeInZone.isEmpty())
    {
        Intersection withHealer = getIntersection(healerZone, safeInZone);
        if (!withHealer.isEmpty())
        {
            ad = findBestToDodge(withHealer);
        }
        else
        {
            if (!justSafe.isEmpty())
            {
                Intersection nwithHealer = getIntersection(healerZone, justSafe);
                if (!nwithHealer.isEmpty())
                {
                    ad = findBestToDodge(nwithHealer);
                }
                else
                {
                    ad = findBestToDodge(safeInZone);
                }
            }
            else
            {
                return ans;
            }
        }
    }
    else if (!justSafe.isEmpty())
    {
        Intersection withHealer = getIntersection(healerZone, justSafe);
        if (!withHealer.isEmpty())
        {
            ad = findBestToDodge(withHealer);
        }
        else
        {
            ad = findBestToDodge(justSafe);
        }
    }
    else
    {
        ad = make_pair(world->getMap().getMyRespawnZone()[0]->getRow(), world->getMap().getMyRespawnZone()[0]->getColumn());
    }

    cerr << "Dodge To (" << ad.first << "," << ad.second << ")" << endl;

    Cell tarcell = world->getMap().getCell(ad.first, ad.second);
    ans.push_back(ActionProperty(-1, BLASTER_DODGE, hero, tarcell, 0));

    return ans;
}

void drawIntersection(Intersection in, World *world)
{
    char map[world->getMap().getRowNum()][world->getMap().getColumnNum()];
    for (int i = 0; i < world->getMap().getRowNum(); i++)
    {
        for (int j = 0; j < world->getMap().getColumnNum(); j++)
        {
            map[i][j] = '.';
            if (world->getMap().getCell(i, j).isWall())
                map[i][j] = '#';
        }
    }

    for (pair<int, int> p : in.cells)
    {
        int r = p.first;
        int c = p.second;
        if (map[r][c] == '.')
            map[r][c] = '*';
    }

    for (int i = 0; i < world->getMap().getRowNum(); i++)
    {
        for (int j = 0; j < world->getMap().getColumnNum(); j++)
        {
            cerr << map[i][j] << " ";
        }
        cerr << endl;
    }

    cerr << "-------------------------------------------------" << endl;
}

Direction BlasterMove(World *world, Hero *hero)
{
    if (!hero->getCurrentCell().isInObjectiveZone())
        return moveTowardTheZone(world, hero);

    vector<ActionProperty> ans;
    vector<Hero *> enemies = world->getOppHeroes();
    Ability attackAbility = hero->getAbility(BLASTER_BOMB);
    int aoe = attackAbility.getAreaOfEffect();
    int range = attackAbility.getRange();
    int po = attackAbility.getPower();

    Intersection throwRange;
    throwRange = createArea(hero->getCurrentCell().getRow(), hero->getCurrentCell().getColumn(), range, world);
    vector<ActionProperty> temp;
    vector<Hero *> availableEnemies;

    Intersection riskiPoints;

    for (Hero *h : world->getMyHeroes())
    {
        if (h->getId() != hero->getId())
            riskiPoints = addIntersection(riskiPoints, createArea(h->getCurrentCell().getRow(), h->getCurrentCell().getColumn(), 3, world));
    }

    for (auto &enemy : enemies)
    {
        if (enemy->getCurrentCell().getRow() == -1)
            continue;

        availableEnemies.push_back(enemy);
    }

    Intersection zone;
    for (Cell *cell : world->getMap().getObjectiveZone())
    {
        int r = cell->getRow();
        int c = cell->getColumn();
        zone.addCell(r, c);
    }

    vector<Intersection> intersection[4];
    for (unsigned int i = 0; i < availableEnemies.size(); i++)
    {
        Intersection temp = createArea(availableEnemies[i]->getCurrentCell().getRow(), availableEnemies[i]->getCurrentCell().getColumn(), aoe, world);
        temp.addMember(availableEnemies[i]->getId());
        Intersection withZone = getIntersection(zone, temp);
        if (!withZone.isEmpty() && !subIntersection(withZone, riskiPoints).isEmpty())
        {
            intersection[0].push_back(withZone);
        }
    }

    unsigned int k;
    for (k = 1; k < 4; k++)
    {
        for (unsigned int i = 0; i < intersection[k - 1].size(); i++)
        {
            for (unsigned int j = 0; j < intersection[0].size(); j++)
            {
                if (intersection[k - 1][i].members.find((*(intersection[0][j].members.begin()))) == intersection[k - 1][i].members.end())
                {
                    Intersection t = getIntersection(intersection[k - 1][i], intersection[0][j]);
                    Intersection safe = subIntersection(t, riskiPoints);
                    if (!safe.isEmpty())
                    {
                        intersection[k].push_back(t);
                    }
                }
            }
        }
        if (intersection[k].empty())
            break;
    }

    k--;

    vector<pair<int, Intersection>> options;

    for (unsigned int i = 0; i < intersection[k].size(); i++)
    {
        // drawIntersection(intersection[k][i], world);
        int score = 0;
        for (auto mem : intersection[k][i].members)
        {
            if (po > world->getHero(mem).getCurrentHP())
                score += 100000 + world->getHero(mem).getCurrentHP();
            else
                score += po;
        }

        Intersection finalIn = subIntersection(intersection[k][i], riskiPoints);

        if (finalIn.isEmpty())
            continue;
        if (isIntersected(throwRange, finalIn))
        {
            cerr << "I Am In (" << hero->getCurrentCell().getRow() << "," << hero->getCurrentCell().getColumn() << ") And" << endl;
            cerr << " Middle Case" << endl;
            return NULL_DIRECTION;
        }
        pair<int, Intersection> temp = make_pair(score, finalIn);
        options.push_back(temp);
    }

    sort(options.begin(), options.end());
    reverse(options.begin(), options.end());

    vector<Cell *> avoid;
    for (unsigned int j = 0; j < 4; j++)
    {
        Cell *tc = new Cell(world->getMyHeroes()[j]->getCurrentCell());
        if (world->getMyHeroes()[j]->getId() != hero->getId())
            avoid.push_back(tc);
    }

    for (auto element : options)
    {
        for (pair<int, int> dest : element.second.cells)
        {
            vector<Direction> path = world->getPathMoveDirections(hero->getCurrentCell().getRow(), hero->getCurrentCell().getColumn(), dest.first, dest.second, avoid);

            if (path.size() > 0)
            {
                cerr << "I Wanna Go (" << dest.first << "," << dest.second << ") So I'll Go " << getDirection(path[0]) << ", There Are " << k << " People" << endl;
                for (int co = 0; co < 3; co++)
                {
                    delete avoid[co];
                }
                return path[0];
            }
        }
    }

    Intersection remain;
    for (Cell *cell : world->getMap().getObjectiveZone())
    {
        remain.addCell(cell->getRow(), cell->getColumn());
    }

    remain = subIntersection(remain, riskiPoints);

    int minDist = 10000;
    pair<int, int> minDest;

    for (pair<int, int> p : remain.cells)
    {
        if (world->manhattanDistance(hero->getCurrentCell().getRow(), hero->getCurrentCell().getColumn(), p.first, p.second) < minDist)
        {
            minDist = world->manhattanDistance(hero->getCurrentCell().getRow(), hero->getCurrentCell().getColumn(), p.first, p.second);
            minDest = p;
        }
    }

    if (minDist != 10000)
    {
        vector<Direction> path = world->getPathMoveDirections(hero->getCurrentCell().getRow(), hero->getCurrentCell().getColumn(), minDest.first, minDest.second, avoid);
        if (!path.empty())
        {
            for (int co = 0; co < 3; co++)
            {
                delete avoid[co];
            }
            return path[0];
        }
    }

    for (int co = 0; co < 3; co++)
    {
        delete avoid[co];
    }

    cerr << "I Am In (" << hero->getCurrentCell().getRow() << "," << hero->getCurrentCell().getColumn() << ") And" << endl;
    cerr << " End Case" << endl;
    return NULL_DIRECTION;
}

vector<ActionProperty> BlasterFitness(World *world, Hero *hero)
{
    vector<ActionProperty> ans;
    if (hero->getAbility(BLASTER_BOMB).isReady())
    {
        vector<ActionProperty> bombVector = BlasterAttack(world, hero, BLASTER_BOMB);
        ans.insert(ans.end(), bombVector.begin(), bombVector.end());
    }

    if (hero->getAbility(BLASTER_ATTACK).isReady())
    {
        vector<ActionProperty> attackVec = BlasterAttack(world, hero, BLASTER_ATTACK);
        ans.insert(ans.end(), attackVec.begin(), attackVec.end());
    }

    if (hero->getAbility(BLASTER_DODGE).isReady() && lastHeal != hero->getId())
    {
        vector<ActionProperty> dodgeVec = BlasterDodge(world, hero);
        ans.insert(ans.end(), dodgeVec.begin(), dodgeVec.end());
    }

    if (lastHeal == hero->getId())
        lastHeal = -1;

    sort(ans.begin(), ans.end());

    return ans;
}

//Healer -------------------------------------------------------------------------------------------------

ActionProperty *HealerHeal(World *world, Hero *hero)
{
    Ability healAbility = hero->getAbility(HEALER_HEAL);
    int range = healAbility.getRange();

    vector<pair<int, int>> options;
    for (Hero *h : world->getMyHeroes())
    {
        if (world->manhattanDistance(hero->getCurrentCell(), h->getCurrentCell()) > range)
            continue;

        int p;
        if (isInDanger(h))
            p = 0;
        else if (h->getCurrentHP() < 85)
            p = 10000;
        else
            continue;

        // cerr << "Candidate " << h->getCurrentCell().getRow() << " " << h->getCurrentCell().getColumn() << endl;

        p += h->getCurrentHP();

        options.push_back(make_pair(p, h->getId()));
    }

    sort(options.begin(), options.end());

    if (options.empty())
        return NULL;
    pair<int, int> p = options[0];
    // cerr << "Choosen " << world->getHero(p.second).getCurrentCell().getRow() << " " << world->getHero(p.second).getCurrentCell().getColumn() << endl;
    ActionProperty *ap = new ActionProperty((p.first % 10000) - 3, HEALER_HEAL, hero, world->getHero(p.second).getCurrentCell(), p.first);
    ap->addAffected(p.second);
    lastHeal = p.second;

    return ap;
}

ActionProperty *HealerHealFree(World *world, Hero *hero)
{
    Ability healAbility = hero->getAbility(HEALER_HEAL);
    int range = healAbility.getRange();

    vector<pair<int, int>> options;
    for (Hero *h : world->getMyHeroes())
    {
        if (world->manhattanDistance(hero->getCurrentCell(), h->getCurrentCell()) > range)
            continue;

        // cerr << "Candidate " << h->getCurrentCell().getRow() << " " << h->getCurrentCell().getColumn() << endl;
        int p;
        if (h->getCurrentHP() == h->getMaxHP())
            continue;
        p += h->getCurrentHP();

        options.push_back(make_pair(p, h->getId()));
    }

    sort(options.begin(), options.end());

    if (options.empty())
        return NULL;
    pair<int, int> p = options[0];
    // cerr << "Choosen " << world->getHero(p.second).getCurrentCell().getRow() << " " << world->getHero(p.second).getCurrentCell().getColumn() << endl;
    ActionProperty *ap = new ActionProperty((p.first % 10000) - 3, HEALER_HEAL, hero, world->getHero(p.second).getCurrentCell(), p.first);
    ap->addAffected(p.second);
    lastHeal = p.second;

    return ap;
}

Direction HealerMove(World *world, Hero *hero)
{
    if (!hero->getCurrentCell().isInObjectiveZone())
        return moveTowardTheZone(world, hero);

    return NULL_DIRECTION;
}

vector<ActionProperty> HealerFitness(World *world, Hero *hero)
{
    vector<ActionProperty> ans;

    if (hero->getAbility(HEALER_HEAL).isReady())
    {
        ActionProperty *temp = HealerHeal(world, hero);
        if (temp != NULL)
        {
            ActionProperty heal = *temp;
            delete temp;
            ans.push_back(heal);
            return ans;
        }
    }

    if (hero->getAbility(HEALER_ATTACK).isReady())
    {
        vector<ActionProperty> attackVec = BlasterAttack(world, hero, HEALER_ATTACK);
        ans.insert(ans.end(), attackVec.begin(), attackVec.end());
    }

    if (hero->getAbility(HEALER_DODGE).isReady() && lastHeal != hero->getId())
    {
        // vector<ActionProperty> dodgeVec = BlasterDodge(world, hero);
        // ans.insert(ans.end(), dodgeVec.begin(), dodgeVec.end());
    }

    if (lastHeal == hero->getId())
        lastHeal = -1;

    sort(ans.begin(), ans.end());

    return ans;
}
