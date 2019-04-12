#include<iostream>
#include<vector>
#define totalPoints = 100
using namespace std;

class GA{
private:
  int startX;
  int startY;

  int popSize;

  int points[totalPoints][totalPoints];

  int pathMaxLength;   // total length of path

  int amountEliete;
  int stopGen;

  vector<pathData> population;
  vector<pathData> newPopulation;

public:
  GA(){
    popSize = 10;
    pathMaxLength = 100;

    startX = 0;
    startY = 0;
  }


}
