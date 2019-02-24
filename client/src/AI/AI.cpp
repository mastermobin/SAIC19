#include <stdlib.h>
#include <bits/stdc++.h>
#include "AI.h"

#define mp make_pair
#define pb push_back

using namespace std;

void printLine()
{
    cerr << "----------------------------------" << endl;
}

string getHero(int hero)
{
    switch (hero)
    {
    case SENTRY:
        return "Sentry";
    case BLASTER:
        return "Blaster";
    case HEALER:
        return "Healer";
    case GUARDIAN:
        return "Guardian";
    }
    return "NONE";
}

string getDirection(int dir)
{
    switch (dir)
    {
    case UP:
        return "UP";
    case DOWN:
        return "DOWN";
    case LEFT:
        return "LEFT";
    case RIGHT:
        return "RIGHT";
    }
    return "NONE";
}

int dist(Cell c1, Cell c2)
{
    int ans = abs(c1.getColumn() - c2.getColumn()) + abs(c1.getRow() - c2.getRow());
    return ans;
}

int **targeted;
pair<int, int> targets[9];

void AI::preProcess(World *world)
{
    targeted = new int *[world->getMap().getRowNum()];
    for (int i = 0; i < world->getMap().getRowNum(); i++)
    {
        targeted[i] = new int[world->getMap().getColumnNum()];
    }

    for (int i = 0; i < world->getMap().getRowNum(); i++)
    {
        for (int j = 0; j < world->getMap().getColumnNum(); j++)
        {
            targeted[i][j] = -1;
        }
    }

    for (int i = 0; i < 9; i++)
    {
        targets[i] = mp(-1, -1);
    }
}

void AI::pick(World *world)
{
    cerr << "-pick" << endl;
    static int cnt = 0;

    switch (cnt)
    {
    case 0:
        world->pickHero(HeroName::BLASTER);
        break;
    case 1:
        world->pickHero(HeroName::GUARDIAN);
        break;
    case 2:
        world->pickHero(HeroName::HEALER);
        break;
    case 3:
        world->pickHero(HeroName::SENTRY);
        break;
    default:
        world->pickHero(HeroName::BLASTER);
    }
    cnt++;
}

void moveTowardTheZone(World *world)
{
    cerr << "Move :" << endl;

    vector<Cell *> objectiveCells = world->getMap().getObjectiveZone();
    vector<Hero *> heroes = world->getMyHeroes();

    for (auto &hero : heroes)
    {
        printLine();
        cerr << getHero(hero->getName()) << endl;
        int minDist = INT_MAX;
        Cell *minCell = NULL;

        for (auto &objectiveCell : objectiveCells)
        {
            if (dist(*objectiveCell, hero->getCurrentCell()) < minDist &&
                world->isAccessible(*objectiveCell) &&
                (targeted[objectiveCell->getRow()][objectiveCell->getColumn()] == -1 || targeted[objectiveCell->getRow()][objectiveCell->getColumn()] == hero->getId()))
            {
                minCell = objectiveCell;
                minDist = dist(*objectiveCell, hero->getCurrentCell());
            }
        }

        if (minCell != NULL)
        {
            targets[hero->getId()] = mp(minCell->getRow(), minCell->getColumn());
            targeted[minCell->getRow()][minCell->getColumn()] = hero->getId();
            Cell heroCell = hero->getCurrentCell();
            cerr << "Hero Place : " << heroCell.getRow() << " " << heroCell.getColumn() << endl;
            cerr << "Objective Cell : " << minCell->getRow() << " " << minCell->getColumn() << endl;
            vector<Cell *> avoid;
            for (int j = 0; j < 4; j++)
            {
                Cell *tc = new Cell(heroes[j]->getCurrentCell());
                if (heroes[j]->getId() != hero->getId())
                    avoid.push_back(tc);
                cerr << "Test" << tc->getRow() << " " << tc->getColumn();
            }

            vector<Direction> path = world->getPathMoveDirections(heroCell, *minCell, avoid);
            cerr << "Path Size : " << path.size() << endl;

            if (path.size() > 0)
            {
                Direction dir = path[0];
                world->moveHero(*hero, dir);
            }
        }
    }
}

void deadProc(World *world)
{
    vector<Hero *> deads = world->getMyDeadHeroes();
    for (auto &dead : deads)
    {
        pair<int, int> targetCell = targets[dead->getId()];
        if (targetCell.first != -1)
            targeted[targetCell.first][targetCell.second] = -1;
        targets[dead->getId()] = mp(-1, -1);
    }
}

void AI::move(World *world)
{
    deadProc(world);
    moveTowardTheZone(world);
}

void AI::action(World *world)
{
    deadProc(world);
    cerr << "-action" << endl;
}