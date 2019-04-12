#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string> // for string class
#include<time.h>
#include <vector>
#include <fstream>
#include <sstream>

using namespace std;

int main() {
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

    while (getline(ss,token, ',')) {
      cout << token<<endl;
    }

    while (true) {
      file >> output;
      if(file.eof())
        break;
      stringstream ss(output);
      while (getline(ss,token, ',')) {
        cout << token<<endl;
      }
    }
  }
  else
    cout <<"FILE NOT FOUND\n";
}
