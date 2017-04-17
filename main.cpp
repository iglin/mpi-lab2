#include <mpi.h>
#include <stdio.h>
#include <string.h>

#define BUFSIZE 128
#define PRODUCER_ID 0
#define N 100

double** generateArray();
void printArray(double ** arr);
bool consume(int processId) ;

void produce(int processesNumber) ;

int main(int argc, char *argv[])
{
    int processesNumber;
    int myId;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &processesNumber);
    MPI_Comm_rank(MPI_COMM_WORLD, &myId);

    if(myId == PRODUCER_ID) {
        produce(processesNumber);
    }
    else {
        while (consume(myId));
    }

    MPI_Finalize();
    return 0;
}

double** generateArray() {
    double** table = new double*[N];
    for (int i = 0; i < N; i++) {
        table[i] = new double[N];
        for (int j = 0; j < N; j++) {
            table[i][j] = rand() % 100;
        }
    }
    return table;
}


void printArray(double ** arr) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%f ", arr[i][j]);
           }
        printf("\n");
    }
}

void produce(int processesNumber) {
    MPI_Status status;

    srand((unsigned int) time(0));
    double **matrixA = generateArray();
    double **matrixB = generateArray();
    printf("Matrix A: \n");
    printArray(matrixA);
    printf("Matrix B: \n");
    printArray(matrixB);

    int tag = 0;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            char *buff = new char[BUFSIZE];
            sprintf(buff, "%f+%f", matrixA[i][j], matrixB[i][j]);
            int targetProcess = i % (processesNumber - 1) + 1;
            MPI_Send(buff, BUFSIZE, MPI_CHAR, targetProcess, tag, MPI_COMM_WORLD);
            printf("Producer sent message '%s' to process %d with tag %d\n", buff, targetProcess, tag);
            tag++;
        }
    }

    double** result = new double*[N];
    tag = 0;
    for (int i = 0; i < N; i++) {
        result[i] = new double[N];
        for (int j = 0; j < N; j++) {
            char *buff = new char[BUFSIZE];
            int targetProcess = i % (processesNumber - 1) + 1;
            MPI_Recv(buff, BUFSIZE, MPI_CHAR, targetProcess, tag, MPI_COMM_WORLD, &status);
            printf("Received result M[%d,%d]=%s\n", i, j, buff);
            result[i][j] = atof(buff);
            tag++;
        }
    }

    char *buff = new char[BUFSIZE];
    sprintf(buff, "exit");
    printf("Resulting matrix: \n");
    printArray(result);

    MPI_Abort(MPI_COMM_WORLD, 0);
}

bool consume(int processId) {
    MPI_Status status;

    char buff[BUFSIZE];
    MPI_Recv(buff, BUFSIZE, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    printf("Process %d received message '%s'\n", processId, buff);

    std::string str = buff;
    unsigned long index = str.find("+");
    double a = atof(str.substr(0, index).c_str());
    double b = atof(str.substr(index + 1, str.length() - index - 1).c_str());

    char *formattedString = new char[BUFSIZE];
    sprintf(formattedString, "%f", a + b);

    printf("Process %d is sending result '%s' with tag %d\n", processId, formattedString, status.MPI_TAG);
    MPI_Send(formattedString, BUFSIZE, MPI_CHAR, PRODUCER_ID, status.MPI_TAG, MPI_COMM_WORLD);
    return true;
}