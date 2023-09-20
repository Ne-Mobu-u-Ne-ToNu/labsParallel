package Mitrichev;

import java.util.ArrayList;
import java.util.List;

public class LabOneThreads implements Runnable {

    private int id;
    private static final int SIZE = 50;
    private static final List<Integer[][]> matrices = new ArrayList<>();
    private static final int matrixCount = 1000;
    private static final int MIN = -1000;
    private static final int MAX = 1000;
    private int from;
    private int to;

    public LabOneThreads(int id, int from, int to) {
        this.id = id;
        this.from = from;
        this.to = to;
    }

    private static int rnd(int min, int max) {
        max -= min;
        return (int) (Math.random() * ++max) + min;
    }

    public static void initMatrices() {
        for (int k = 0; k < matrixCount; k++) {
            Integer[][] matrix = new Integer[SIZE][SIZE];
            for (int i = 0; i < SIZE; i++) {
                for (int j = 0; j < SIZE; j++) {
                    matrix[i][j] = rnd(MIN, MAX);
                }
            }
            matrices.add(matrix);
        }
    }

    public synchronized int getTrace(Integer[][] matrix) {
        int result = 0;
        for (int i = 0; i < matrix.length; i++) {
            for (int j = 0; j < matrix[i].length; j++) {
                if (i == j) result += matrix[i][j];
            }
        }
        return result;
    }

    @Override
    public String toString() {
        return "Поток #" + id;
    }

    @Override
    public void run() {
        for (int i = from; i < to; i++) {
            int trace = getTrace(matrices.get(i));
            System.out.println(this + " матрица #" + i + " след: " + trace);
            Thread.yield();
        }
    }
}
