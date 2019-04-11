package GeneticAlgorithm;

/**
 * holds the info about a specific GeneticAlgorithm.pathData
 * contains the cities that make up the paths
 * and the paths fitness
 */
public class pathData implements Comparable<pathData> {
    private int[] path;
    private int fitness;

    pathData(int[] path) {
        this.path = path;
        fitness = 0;
    }
    public pathData(){

    }

    int getFitness() {
        return fitness;
    }

    void setFitness(int fitness) {
        this.fitness = fitness;
    }

    int[] getPath() {
        return path;
    }

    public void printPath() {
        for (int i : path) {
            System.out.print(i + " ");
        }
    }

    @Override
    public int compareTo(pathData o) {
        if (this.fitness < o.fitness) {
            return -1;
        } else if(this.fitness == o.getFitness()) {
            return 0;
        } else {
            return 1;
        }
    }
}
