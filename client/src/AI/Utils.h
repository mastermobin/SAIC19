#include <Model/Interface/World.h>

#define mp make_pair
#define pb push_back

using namespace std;

void printLine();
string getHero(int hero);
string getDirection(int dir);
int dist(Cell c1, Cell c2);
void preProc(World *world);
void moveTowardTheZone(World *world);
void deadProc(World *world);
