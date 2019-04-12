#include<iostream>
#include<vector>
#include<algorithm>
#include "pathData.cpp"
#include "GUI.cpp"
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <omp.h>
#include <utility>

#define numCities 8
#define numElite 5
#define numGen 10
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
    fileReader();
    initialization();

    int gen = 0;

    // while(gen < stopGen){
      evaluation();
      eliteSelection();
      crossover();
      printPopulation(gen);

    //   population.clear();
    //   population = newPopulation;
    //   newPopulation.clear();
    //   gen++;
    // }


    // cout << "testing sorting by fitness" << endl;
    // for(auto i : population){
    //   cout << i.getFitness() << endl;
    // }

    // cout << "aobut to draw" << endl;
    // gui.drawLine(population[0].getPath(),pathLength);


  }

  void printPopulation(int gen) {
    cout << "\n\nGeneration" << gen << endl;
      for (int i = 0; i < popSize; i++) {
        cout << i << ": ";
        population[i].printPath();
        cout << " fitness = " << population[i].getFitness() << endl;
      }
  }

  void initialization(){
  // cout << "aobut to initialize" << endl;
    vector<int> tempPath;

    for(int i =0; i< popSize;i++){
      tempPath = ranPath();
      pathData p(tempPath);
      population.push_back(p);
    }
  }

  void evaluation(){
    int maxLength = 0;                     // max length of path
    for (int i = 0; i < popSize; i++) {        // go through all paths in population
        int curPathL = 0;                    // the length of the current path

        vector<int> tempPath;                      // used to temporarily hold the path
        tempPath = population[i].getPath();  // get the path

        for (int j = 0; j < pathLength - 1; j++) {   // go through the entire path
            int cityX = tempPath[j];             // city at j
            int cityY = tempPath[j + 1];           // city the previous city connects to
            curPathL += distances[cityX][cityY]; // distance between the 2 cities
        }

        if (curPathL > maxLength) {   // find out which path is longest
            maxLength = curPathL;
        }
        population[i].setFitness(curPathL);
    }

    // go through all the lengths in fitness array
    int n = 0;
    for (int i = 0; i < popSize; i++) {
        n = population[i].getFitness();
        int m = maxLength - n;  // figure out the fitness of the specific path
        population[i].setFitness(m);
    }

    sort(population.begin(),population.end());

  }

  /**
   * Elitism method to copy the best paths from previous population into the
   * new population as is based on there fitness
   * adds a few of the best paths from original population to the new population as is
   */
  void eliteSelection() {
      for (int i = 0; i < amountElite; i++) {
          newPopulation.push_back(population[i]);
      }
  }

  /**
   * Use tournament selection to pick a candidate for next population
   *
   * @return the two selected candidates in an int array containing there index
   */
  vector<pathData> selection() {
      //select top best 50 percent of the population as potential candidates
      vector<pathData> candidates;
      int candidatesFitness = 0;

      //get the top 50% of the candidates for selection based on there fitness levels
      for (int i = 0; i < (int)(popSize / 2); i++) {
          candidates.push_back(population[i]);
          candidatesFitness += population[i].getFitness();
      }


      double probability[popSize/2] ;


      for (int i = 0; i < popSize / 2; i++) {
          probability[i] = (candidates[i].getFitness() / (double) candidatesFitness);   //get probability of each one being selected out of 100%
      }


      if (candidates.size() > 2) {
          vector<pathData> selectedCandidates;

          //indexes of the two selected parents from the candidates
          int selectFirst = 0;
          int selectSecond = 0;

          double randNum;
          randNum = (rand()%100)/100.0;
          double startProb = 0.0;   // keep track of what numbers have already been check for probability

          //go through all candidates
          for (int i = 0; i < popSize / 2; i++) {
              // if the num generated is between the total of all previous probability
              // and the probability of the number being selected + total of all previous probabilities
              if (randNum >= startProb && randNum < (probability[i] + startProb)) {
                  selectFirst = i;
                  break;
              } else {
                  startProb += probability[i];  // increase startProbability by the probability of the current candidate
              }
          }

          //pick the second candidates based on probability(randNum)
          randNum = (rand()%100)/100.0;    //generate a new random number
          startProb = 0;
          for (int i = 0; i < popSize / 2; i++) {
              if (randNum >= startProb && randNum < (probability[i] + startProb)) {
                  selectSecond = i;
                  // if the candidate selected is the same as the previous candidate selected
                  // generate a new random floating point number and check again
                  if (selectSecond == selectFirst) {
                      i = -1;
                      randNum = (rand()%100)/100;
                      startProb = 0;
                  } else {
                      break;
                  }
              } else {
                  startProb += probability[i];
              }
          }

          // add the two selected candidates to the array to be returned
          selectedCandidates.push_back(candidates[selectFirst]);
          selectedCandidates.push_back(candidates[selectSecond]);
          return selectedCandidates;
      } else {
          return candidates;
      }
  }

  /**
   * Swap two random cities
   *
   * @param child the path to perform the swap in
   */
  vector<int> mutation(vector<int> child) {
      int firstNum = (int) (rand()% (pathLength) + 1);
      int secondNum = (int) (rand()% (pathLength - 1));


      while (secondNum <= firstNum) {
          firstNum = (int) (rand()% (pathLength) + 1);
          secondNum = (int) (rand()% (pathLength - 1));
      }

      int temp = child[firstNum];
      child[firstNum] = child[secondNum];
      child[secondNum] = temp;

      return child;
  }

  /**
   * get the two parents from selection and perform Ordered Crossover
   * to get a child for the next generation
   */
  void crossover() {
      for (int repeat = amountElite; repeat < popSize; repeat++) {
          vector<pathData> parents = selection(); // get the two parents for selection

          int firstCut;
          int secondCut;

          //first and last city are picked before hand so skip those indexes
          firstCut = (int) (rand()%(pathLength) + 1);
          secondCut = (int) (rand()%(pathLength - 1));


          //keep looking for a second cut till second is more than firstCut
          while (secondCut <= firstCut) {
              firstCut = (int) (rand()%(pathLength) + 1);
              secondCut = (int) (rand()%(pathLength - 1));
          }



          vector<int> firstParent = parents[0].getPath();



          //save the cities in between the two cuts to remove
          int index = firstCut;
          int inbetweenFirst[secondCut - firstCut];
          // cities in between the two cuts on the first parent
          for (int i = 0; i < secondCut - firstCut; i++) {
              inbetweenFirst[i] = firstParent[index];
              if (index < secondCut) {
                  index++;
              }
          }




          int child[pathLength];

          //the first and last index dont change because start and end city always the same
          child[0] = firstParent[0];
          child[pathLength - 1] = firstParent[pathLength - 1];

          //copy the cities in between the two cuts into child
          for (int i = firstCut; i < secondCut; i++) {
              child[i] = firstParent[i];
          }


          vector<int> secondParent = parents[1].getPath();

          index = secondCut;  // second parent array that you are copying from
          bool found = false;
          int i = secondCut;

          while (i != firstCut) {
              found = false;

              //check if its one of the cities copied from first parent
              for (int x = 0; x < (secondCut - firstCut); x++) {
                  if (secondParent[index] == inbetweenFirst[x]) {
                      found = true;
                      break;
                  }
              }

              if (!found) {  // if the city wasnt copied from parent 1
                  child[i] = secondParent[index];
                  if (i < pathLength - 2) {  // if you havent reached end of array
                      i++; //go to next index in child
                  } else {
                      i = 1; // go to the first index child array
                  }
                  if (index < pathLength - 2) {
                      index++; // go to next index in secondParent;
                  } else {
                      index = 1; // go to the first index in secondParent array
                  }
              } else {
                  if (index < pathLength - 2) {
                      index++; // go to next index in secondParent;
                  } else {
                      index = 1; // go to the first index in secondParent array
                  }
              }

          }

          vector<int> childVector;
          for(int i =0; i < pathLength;i++){
            childVector.push_back(child[i]);
          }
          cout << endl;


          childVector = mutation(childVector);

          pathData p (childVector);
          newPopulation.push_back(p);
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

  void fileReader(){
    ifstream file;
    file.open("Map data.csv");
    string output;

    stringstream ss(output);

    // getline(ss,output, ',');

    // cout << output<<endl;

    if (file.is_open()) {
      file >> output;

      stringstream ss(output);
      string token;

      int x=0;
      while (getline(ss,token, ',')) {
        // cout << token<<endl;
        cities[x] = token;
        x++;
      }

      x = 0;
      int y = 0;
      while (true) {
        file >> output;
        if(file.eof())
          break;
        stringstream ss(output);
        while (getline(ss,token, ',')) {
          // cout << token << endl;
          int temp = stoi(token);
          distances[x][y] = temp;
          x++;
        }
        x = 0;
        y++;
      }
    }
    else
      cout <<"FILE NOT FOUND\n";
  }

};

int main(int argv, char** args) {
  // GUI gui;
  // gui.running();
  GA ga;
  return 0;
}
