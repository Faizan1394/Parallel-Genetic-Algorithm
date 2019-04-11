package GeneticAlgorithm;

import java.io.File;
import java.io.FileNotFoundException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Random;
import java.util.Scanner;
import java.util.concurrent.locks.ReentrantLock;

public class GA implements Runnable {

    private String[] cities; // Graphics data for testing
    private int[][] distances;

    private int startCity;     // index of city to start and end tour at
    private int popSize; // size of initial population

    private int totalCities; // total number of cities to travel
    private int pathLength;  // totalCities+1 because you must end at the city you started at

    private int amountElite = 5;  //amount of paths in population to carry over as is(MUST BE LESS THEN popSize)
    private int stopGen = 20;

    // holds all paths in current population and there fitness
    private ArrayList<pathData> population = new ArrayList<>();
    private ArrayList<pathData> newPopulation = new ArrayList<>();

    private volatile ArrayList<pathData> finalPopulation;

    //lock
    private ReentrantLock lock;

    GA(String[] cities, int[][] distances, ArrayList<pathData> finalPopulation,int popSize,int startCity, ReentrantLock lock) {
        this.cities = cities;
        this.distances = distances;
        this.totalCities = cities.length;
        this.pathLength = totalCities + 1;
        this.lock = lock;
        this.finalPopulation = finalPopulation;
        this.popSize = popSize;
        this.startCity = startCity;

        //make a random population of size 'popSize'
        initialization();
    }

    public static void main(String[] args) {
        String[] cityNames;
        int[][] distances;


        File file = new File("Map Data.csv");
        Scanner input;
        try {
            input = new Scanner(file);
            String cities = input.nextLine();
            cityNames = cities.split(",");
            distances = new int[cityNames.length][cityNames.length];

            String line;
            String[] tempSplit;

            int currentRow = 0;
            while (input.hasNextLine()) {
                line = input.nextLine();
                tempSplit = line.split(",");
                for (int j = 0; j < cityNames.length; j++) {
                    distances[currentRow][j] = Integer.valueOf(tempSplit[j]);
                }
                currentRow++;
            }


            //Print out the distances between each city
            {

                System.out.print(String.format("%20s", cityNames[0]));
                for (int i = 1; i < cityNames.length; i++) {
                    System.out.print(String.format("%15s", cityNames[i]));
                }
                System.out.println();
                for (int i = 0; i < cityNames.length; i++) {
                    System.out.print(String.format("%-10s|", cityNames[i]));
                    System.out.print(String.format("%8s", distances[i][0]));
                    for (int j = 1; j < cityNames.length; j++) {
                        System.out.print(String.format("%15s", distances[i][j]));
                    }
                    System.out.println();
                }
            }

            //number of threads
            int nThreads;

            //get the amount of cores available on the cpu
            // Genetic Algorithm is CPU intensive so no point in having more threads then the amount of cores
            nThreads = Runtime.getRuntime().availableProcessors();

            System.out.println("\nNumber of Threads: " + nThreads);

            //create threads
            ReentrantLock lock = new ReentrantLock();
            ArrayList<pathData> finalPopulation = new ArrayList<>();

            //the size of each population
            int popSize = 30;

            //the city to start and end the path at
            int startCity = 0;

            Thread threads[] = new Thread[nThreads];
            for (int i = 0; i < nThreads; i++) {
                threads[i] = new Thread(new GA(cityNames, distances,finalPopulation,popSize,startCity,lock));
                threads[i].start();
            }

            for (int i = 0; i < nThreads; i++) {
                threads[i].join();
            }

            System.out.println("\n\nFinal Output:\n");

            System.out.println("Size of each Population: " + popSize);
            System.out.println("Path length: " + cityNames.length);
            System.out.println("Start City: " + startCity);

            System.out.println("Best path found by each thread: ");
            for (pathData i : finalPopulation) {
                i.printPath();
                System.out.println(" fitness = " + i.getFitness());
            }


        } catch (FileNotFoundException e) {
            e.printStackTrace();
            System.out.println("unable to read file");
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

    }

    @Override
    public void run() {
        int gen = 1;

        //termination condition
        //stop when either you have reached the maximum time limit
        //or the max generation
        //while(elapsedTime < stopTime){  // another possible termination condition
        while (gen < stopGen) {

            evaluation();
            eliteSelection();
            crossover();  // selection and mutation called from within crossover

            //uncomment the lines bellow to see the population after each generation
            // commented out so easier to see terminal output
//            lock.lock();
//            printPopulation(gen);
//            lock.unlock();

            Collections.copy(population, newPopulation); //
            newPopulation.clear();
            gen++;
        }

        lock.lock();
        try {
            //add the best path from each population to the final population array
            // which is shared by all threads
            finalPopulation.add(population.get(0));
        } finally {
            lock.unlock();
        }
    }

    private void printPopulation(int gen) {
        System.out.println("\n\nThread Name: " + Thread.currentThread().getName());
        System.out.println("Generation " + gen);
        for (int i = 0; i < popSize; i++) {
            System.out.print(i + ": ");
            population.get(i).printPath();
            System.out.println(" fitness = " + population.get(i).getFitness());
        }
    }

    /**
     * Generate initial population off size 'popSize' for generation 0
     * Each population is a path representation
     * Path representation:
     * the order in which to traverse each city based on there index in the arrays
     * one city is selected to be the start city and will also be the ending city before hand
     * each GeneticAlgorithm.pathData
     */
    public void initialization() {
        int[] tempPath;

        for (int i = 0; i < popSize; i++) {
            tempPath = ranPath();
            pathData p = new pathData(tempPath);
            population.add(p);
        }

        //uncomment lines bellow to see population after a initial population is generated for a thread
//        lock.lock();
//        try {
//            System.out.println("\nThread Name: " + Thread.currentThread().getName());
//            System.out.println("initialization");
//            for (int i = 0; i < popSize; i++) {
//                System.out.print(i + ": ");
//                population.get(i).printPath();
//                System.out.println();
//            }
//        } finally {
//            lock.unlock();
//        }
    }

    public int getPopulationSize(){
        return popSize;
    }

    /**
     * calculate fitness of each path in the population
     * shorter paths have higher fitness
     * we calculate fitness of a path by subtracting the length for the path
     * from the maximum length of the current population
     */
    private void evaluation() {
        int maxLength = 0;                     // max length of path
        for (int i = 0; i < popSize; i++) {        // go through all paths in population
            int curPathL = 0;                    // the length of the current path

            int[] tempPath;                      // used to temporarily hold the path
            tempPath = population.get(i).getPath();  // get the path

            for (int j = 0; j < pathLength - 1; j++) {   // go through the entire path
                int cityX = tempPath[j];             // city at j
                int cityY = tempPath[j + 1];           // city the previous city connects to
                curPathL += distances[cityX][cityY]; // distance between the 2 cities
            }

            if (curPathL > maxLength) {   // find out which path is longest
                maxLength = curPathL;
            }
            population.get(i).setFitness(curPathL);
        }
        // go through all the lengths in fitness array
        int n = 0;
        for (int i = 0; i < popSize; i++) {
            n = population.get(i).getFitness();
            int m = maxLength - n;  // figure out the fitness of the specific path
            population.get(i).setFitness(m);
        }


        //sort the paths based on there fitness
        Collections.sort(population, Collections.reverseOrder());
    }

    /**
     * Elitism method to copy the best paths from previous population into the
     * new population as is based on there fitness
     * adds a few of the best paths from original population to the new population as is
     */
    private void eliteSelection() {
        for (int i = 0; i < amountElite; i++) {
            newPopulation.add(population.get(i));
        }
    }

    /**
     * Use tournament selection to pick a candidate for next population
     *
     * @return the two selected candidates in an int array containing there index
     */
    private pathData[] selection() {
        //select top best 50 percent of the population as potential candidates
        pathData[] candidates = new pathData[popSize / 2];
        int candidatesFitness = 0;

        //get the top 50% of the candidates for selection based on there fitness levels
        for (int i = 0; i < popSize / 2; i++) {
            candidates[i] = population.get(i);
            candidatesFitness += population.get(i).getFitness();
        }

        double[] probability = new double[popSize / 2];


        for (int i = 0; i < popSize / 2; i++) {
            probability[i] = (candidates[i].getFitness() / (double) candidatesFitness);   //get probability of each one being selected out of 100%
        }


        if (candidates.length > 2) {
            pathData[] selectedCandidates = new pathData[2];


            //indexes of the two selected parents from the candidates
            int selectFirst = 0;
            int selectSecond = 0;

            double randNum;
            Random rand = new Random();
            randNum = rand.nextDouble();   // generate a random floating point number between 0.0 and 1.0
            //pick the first candidates based on probability(randNum)
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
            randNum = rand.nextFloat();    //generate a new random number
            startProb = 0;
            for (int i = 0; i < popSize / 2; i++) {
                if (randNum >= startProb && randNum < (probability[i] + startProb)) {
                    selectSecond = i;
                    // if the candidate selected is the same as the previous candidate selected
                    // generate a new random floating point number and check again
                    if (selectSecond == selectFirst) {
                        i = -1;
                        randNum = rand.nextFloat();
                        startProb = 0;
                    } else {
                        break;
                    }
                } else {
                    startProb += probability[i];
                }
            }

            // add the two selected candidates to the array to be returned
            selectedCandidates[0] = candidates[selectFirst];
            selectedCandidates[1] = candidates[selectSecond];


            return selectedCandidates;
        } else {
            return candidates;
        }
    }

    /**
     * get the two parents from selection and perform Ordered Crossover
     * to get a child for the next generation
     */
    private void crossover() {
        for (int repeat = amountElite; repeat < popSize; repeat++) {
            pathData[] parents = selection(); // get the two parents for selection

            int firstCut;
            int secondCut;

            //first and last city are picked before hand so skip those indexes
            firstCut = (int) (Math.random() * (pathLength) + 1);
            secondCut = (int) (Math.random() * (pathLength - 1));


            //keep looking for a second cut till second is more than firstCut
            while (secondCut <= firstCut) {
                firstCut = (int) (Math.random() * (pathLength) + 1);
                secondCut = (int) (Math.random() * (pathLength - 1));
            }

            int[] firstParent = parents[0].getPath();


            //save the cities in between the two cuts to remove
            int index = firstCut;
            int[] inbetweenFirst = new int[secondCut - firstCut];
            // cities in between the two cuts on the first parent
            for (int i = 0; i < secondCut - firstCut; i++) {
                inbetweenFirst[i] = firstParent[index];
                if (index < secondCut) {
                    index++;
                }
            }


            int[] child = new int[pathLength];

            //the first and last index dont change because start and end city always the same
            child[0] = firstParent[0];
            child[pathLength - 1] = firstParent[pathLength - 1];

            //copy the cities in between the two cuts into child
            for (int i = firstCut; i < secondCut; i++) {
                child[i] = firstParent[i];
            }


            int[] secondParent = parents[1].getPath();
            index = secondCut;  // second parent array that you are copying from
            boolean found = false;
            int i = secondCut;

            while (i != firstCut) {
                found = false;

                //check if its one of the cities copied from first parent
                for (int x = 0; x < inbetweenFirst.length; x++) {
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


            child = mutation(child);


            pathData p = new pathData(child);
            newPopulation.add(p);
        }

    }

    /**
     * Swap two random cities
     *
     * @param child the path to perform the swap in
     */
    private int[] mutation(int[] child) {
        int firstNum = (int) (Math.random() * (pathLength) + 1);
        int secondNum = (int) (Math.random() * (pathLength - 1));


        while (secondNum <= firstNum) {
            firstNum = (int) (Math.random() * (pathLength) + 1);
            secondNum = (int) (Math.random() * (pathLength - 1));
        }

        int temp = child[firstNum];
        child[firstNum] = child[secondNum];
        child[secondNum] = temp;

        return child;
    }

    /**
     * use the Fisher-Yates shuffel method to generate
     * unique random numbers for each GeneticAlgorithm.pathData in the initial population(generation 0)
     */
    public int[] ranPath() {
        //holds the index of all cities that can make up the GeneticAlgorithm.pathData except for the city that we start and end at
        int[] cityIndex = new int[pathLength - 2];

        int index = 0;
        //save indexes of all cities into the array cityIndex except for the city that we start and end at
        for (int i = 0; i < cityIndex.length; i++) {
            if (index == startCity) {
                index++;
            }
            cityIndex[i] = index;

            index++;
        }

        for (int i = cityIndex.length - 1; i > 0; i--) {
            int ranNum = (int) (Math.random() * ((i) + 1)); // random num between 0 and i
            int temp = cityIndex[i];
            cityIndex[i] = cityIndex[ranNum];
            cityIndex[ranNum] = temp;
        }

        //add the start and end city to the GeneticAlgorithm.pathData
        int[] fullPath = new int[pathLength];
        fullPath[0] = startCity;            // add the first to the start of GeneticAlgorithm.pathData
        fullPath[pathLength - 1] = startCity; // add the last city to the end of GeneticAlgorithm.pathData
        for (int j = 1; j < pathLength - 1; j++) {      // add the rest of the randomly picked cities in the middle
            fullPath[j] = cityIndex[j - 1];
        }

        return fullPath;
    }
}
