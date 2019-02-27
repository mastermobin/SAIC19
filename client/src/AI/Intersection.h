#include <Model/Interface/World.h>

using namespace std;

class Intersection
{
public:
  set<int> members;
  set<pair<int, int>> cells;

public:
  void addCell(int row, int col, World *world);
  void addMember(int id);
  bool isEmpty();
};