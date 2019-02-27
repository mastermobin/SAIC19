#include <Model/Interface/World.h>

using namespace std;

class Intersection
{
public:
  set<int> members;
  set<pair<int, int>> cells;

public:
  void addCell(int row, int col, World *world);
  void addCell(int row, int col);
  void addMember(int id);
  bool isEmpty();
};

Intersection createArea(int row, int col, int radius, World* world);
Intersection getIntersection(Intersection in1, Intersection in2);
bool isIntersected(Intersection in1, Intersection in2);