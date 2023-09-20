package Mitrichev;

public class Main {
    public static void main(String[] args) throws InterruptedException {
        LabOneThreads.initMatrices();
        long[] time = new long[4];
        int threads = 1;

        //Выполняем для корректного засечения времени
        threadTest(1);

        for (int i = 0; i < 4; i++) {
            long start = System.nanoTime();
            threadTest(threads);
            long end = System.nanoTime();
            time[i] = (end - start) / 1000;
            threads *= 2;
        }
        System.out.println();

        threads = 1;
        for (int i = 0; i < 4; i++) {
            System.out.println("Время выполнения микросекунд для " + threads + " потоков: " + time[i]);
            threads *= 2;
        }
    }

    public static void threadTest(int threads) throws InterruptedException {
        int from = 0;
        int inc = 1000 / threads;
        int to = inc;
        Thread[] ts = new Thread[threads];
        int id = 1;

        for (int i = 0; i < ts.length; i++) {
            ts[i] = new Thread(new LabOneThreads(i + 1, from, to));
            ts[i].start();
            from = to;
            to += inc;
        }

        for (Thread t : ts) {
            t.join();
        }
    }
}
