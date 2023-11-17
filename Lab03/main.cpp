#include <iostream>
#include <mpi.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
#include <algorithm>

#define TAG 0

/*
* Семейное-2. Муж и жена едят торт, забирая с тарелки по одному кусочку и
сообщая номер съеденного кусочка супруге/у. Организовать MPI-программу,
моделирующую поедание торта из 12 кусочков. Супруги съедают торт не обязательно
поровну.
*/

int main(int argc, char** argv) {
    // Вывод на экран на английском, чтобы не возиться
    // с консольными кодировками

    // Общее число кусочков торта
    const int total_pieces = 12;

    // Вектор строк, хранящий названия кусочков
    std::vector<std::string> cake_pieces = {
        "Piece 1",
        "Piece 2",
        "Piece 3",
        "Piece 4",
        "Piece 5",
        "Piece 6",
        "Piece 7",
        "Piece 8",
        "Piece 9",
        "Piece 10",
        "Piece 11",
        "Piece 12"
    };

    // Число кусков, которые каждый из супругов съест (генерируется случайно)
    srand(time(NULL));
    int spouse1_pieces = rand() % (total_pieces / 2 + 1);
    int spouse2_pieces = total_pieces - spouse1_pieces;

    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Барьер для того, чтобы к этому времени синхронизировать потоки
    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0) {
        // Супруг 1
        for (int i = 0; i < spouse1_pieces; ++i) {
            // Генерация случайного кусочка
            int chosen_piece = std::rand() % cake_pieces.size();
            // Отправка (сообщение) второму супругу о выбранном кусочке
            MPI_Send(cake_pieces[chosen_piece].c_str(), cake_pieces[chosen_piece].size() + 1, MPI_CHAR, 1, TAG, MPI_COMM_WORLD);
            std::cout << "Spouse 1 ate: " << cake_pieces[chosen_piece] << std::endl;
            cake_pieces.erase(std::remove(cake_pieces.begin(), cake_pieces.end(), cake_pieces[chosen_piece]), cake_pieces.end());

            // Получение информации о том, какой кусочек съел второй супруг
            char buffer[255];
            MPI_Recv(buffer, sizeof(buffer), MPI_CHAR, 1, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            std::string piece(buffer);
            cake_pieces.erase(std::remove(cake_pieces.begin(), cake_pieces.end(), piece), cake_pieces.end());
        }

        /*
        * Далее следует цикл, в котором мы получем информацию о том, какой кусочек съел второй
        * супруг. Происходит это только в том случае, когда второй супруг съедае больше кусочков,
        * чем первый. Цикл необходим для того, чтобы число отправленных сообщений было таким же, 
        * как и число полученных. Так не будет возникать deadlock.
        */
        if (spouse2_pieces - spouse1_pieces > 0) {
            for (int i = 0; i < spouse2_pieces - spouse1_pieces; i++) {
                // Получение информации о том, какой кусочек съел второй супруг
                char buffer[255];
                MPI_Recv(buffer, sizeof(buffer), MPI_CHAR, 1, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                std::string piece(buffer);
                cake_pieces.erase(std::remove(cake_pieces.begin(), cake_pieces.end(), piece), cake_pieces.end());
            }
        }
    }
    else if (rank == 1) {
        // Супруг 2
        for (int i = 0; i < spouse2_pieces; ++i) {
            char buffer[255];

            /*
            * Получение информации о том, какой кусочек съел первый супруг
            * Здесь так же идет условие, служащее для защиты от deadlock. Тоесть,
            * когда первый супруг перестал есть, а, соответственно, слать сообщения второму,
            * то пора прекращать пытаться эти сообщения получать.
            */
            if (spouse2_pieces - spouse1_pieces == 0 || (spouse2_pieces - spouse1_pieces > 0 && i < spouse1_pieces)) {
                MPI_Recv(buffer, sizeof(buffer), MPI_CHAR, 0, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                std::string piece(buffer);
                cake_pieces.erase(std::remove(cake_pieces.begin(), cake_pieces.end(), piece), cake_pieces.end());
            }

            int chosen_piece = std::rand() % cake_pieces.size();
            std::cout << "Spouse 2 ate: " << cake_pieces[chosen_piece] << std::endl;
            // Отправка (сообщение) первому супругу о выбранном кусочке
            MPI_Send(cake_pieces[chosen_piece].c_str(), cake_pieces[chosen_piece].size() + 1, MPI_CHAR, 0, TAG, MPI_COMM_WORLD);
            cake_pieces.erase(std::remove(cake_pieces.begin(), cake_pieces.end(), cake_pieces[chosen_piece]), cake_pieces.end());
        }
    }

    // Барьер для того, чтобы к этому времени синхронизировать потоки
    MPI_Barrier(MPI_COMM_WORLD);

    // Вывод на экран распределение кусочков торта между супругами
    if (rank == 0) {
        std::cout << "-------------------------------------------------" << std::endl;
        std::cout << "Spouse 1 got " << spouse1_pieces << " pieces." << std::endl;
        std::cout << "Spouse 2 got " << spouse2_pieces << " pieces." << std::endl;
    }

    MPI_Finalize();
    return 0;
}
