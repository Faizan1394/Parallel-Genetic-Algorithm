#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string> // for string class
#include <time.h>
#include <vector>
#include <utility>
#include <algorithm>

using namespace std;

class pathData {
  private:
    vector<int> path;
    int fitness;

  public:
    pathData(vector<int> path) {
      this->path = path;
      fitness = 0;
    }

    int getFitness() {return fitness;}

    void setFitness(int fitness) {this->fitness = fitness;}

    vector<int> getPath() {return path;}

    void printPath() {
      for (int i = 0; i < path.size(); i++) {
        printf("%i ", path[i]);
      }
    }

    int compareTo (pathData o) {
      if(this->fitness < o.getFitness())
        return -1;
      else if (this->fitness == o.getFitness())
        return 0;
      else
        return 1;
    }

    bool operator<(pathData &o)const{
      if(this->fitness > o.getFitness()){
        return this->fitness > o.getFitness();
      }
    }
};
