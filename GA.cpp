#include<iostream>
#include<vector>
#include<algorithm>
#include "pathData.cpp"
#include <cstdlib>

#define numCities 8
#define numElite 5
#define numGen 20
#define pop 10
#define start 0

using namespace std;

class GA{
private:

  string cities[numCities];
  int distances[numCities][numCities];

  int startCity;
  int popSize;

  int pathLength;

  int amountElite;
  int stopGen;



  vector<pathData> population;
  vector<pathData> newPopulation;

public:
  GA(){
    srand(time(NULL));
    amountElite = numElite;
    stopGen = numGen;
    pathLength = numCities+1;
    popSize = pop;
    startCity = start;

    initialization();

    for(int i =0; i< popSize;i++){
      population[i].printPath();
      cout << endl;
    }
  }

<<<<<<< HEAD
  
=======
  void initialization(){
    vector<int> tempPath;

    for(int i =0; i< popSize;i++){
      tempPath = ranPath();
      pathData p(tempPath);
      population.push_back(p);
    }
  }

  /**
   * generate unique random numbers for each GeneticAlgorithm.pathData in the initial population(generation 0)
   */
  vector<int> ranPath(){
    vector<int> cityIndex;

    cityIndex.push_back(startCity); // starting city

    for (int i = 0; i < pathLength-2; i++) {
        int ranNum = rand()%(numCities-1) + 1;
        while(true){
          if(find(cityIndex.begin(),cityIndex.end(),ranNum) !=cityIndex.end()){
            ranNum = rand()%(numCities-1) + 1;
          }else{
            cityIndex.push_back(ranNum);
            break;
          }
        }
    }
    cityIndex.push_back(startCity); // end city

    return cityIndex;

  }
};

int main(int argc, char const *argv[]) {
  GA ga;
  return 0;
>>>>>>> f6b84f796a18dfc0b8b4eaf7167943eee0e1101d
}
