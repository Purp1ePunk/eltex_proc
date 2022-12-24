#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <signal.h>
#include <semaphore.h>

#define SHM_ID  27
#define SHM_SIZE 128
int counter = 0;
int gl_shm_id;
sem_t semaphore;


int amount_of_rand_nums(void)
{
    int result = rand() % 10000 + 1;
    return result;
}

void insert_sort(int array[], int size_of_array)
{
    for(int i = 1; i < size_of_array; ++i)
    {
        int pos = i;
        while(pos > 0 && array[pos - 1] > array[pos])
        {
            int tmp = array[pos - 1];
            array[pos - 1] = array[pos];
            array[pos] = tmp;
            pos -= 1;
        }
    }
}

void print_array(int array[], int size_of_array)
{
    /*Used for code testing in the intermediate developmental stages  */
    for(int i = 0; i < size_of_array; ++i)
        printf("%d ", array[i]);
}

void sig_handler(int sig)
{
    /*Works for all processes (parent, child 1, child 2,
    but child of a child 1 needs kill)*/
    printf("Counter: %d\n", counter);
    shmctl(gl_shm_id, IPC_RMID, 0);
    sem_destroy(&semaphore);
}

int summ(int array[], int sz_of_arr)
{
    int sum = 0;
    for(int i = 0; i < sz_of_arr; ++i)
        sum += array[i];
    return sum;
}


int main(int argc, char *argv[])
{
    srand(time(NULL));
    signal(SIGINT, sig_handler);

    key_t key_shm;
    int shmid;
    int *shm_pointer, *shm_chld_pointer;

    sem_init(&semaphore, 0, 1);

    int size = amount_of_rand_nums();
    if((key_shm = ftok("./task7.c", SHM_ID)) == -1)
    {
        perror("key_shm ftok error");
        exit(EXIT_FAILURE);
    }
    if((shmid = shmget(key_shm, SHM_SIZE, 0644 | IPC_CREAT)) == -1)
    {
        perror("shmget error");
        exit(EXIT_FAILURE);
    }

    gl_shm_id = shmid;
    int id = fork();
    int id1 = fork();

    while(1)
    {
        if(id == 0)
        {
            if(id1 == 0)
            {
                int sum = 0;
                sleep(1);
                if (sem_wait(&semaphore) == -1) {perror("sem_wait");}
                shm_chld_pointer = (int *)shmat(shmid, NULL, 0);
                sum = summ(shm_chld_pointer, size);
                if(shmdt(shm_chld_pointer) == -1)
                    {
                        perror("shmdt error");
                        exit(EXIT_FAILURE);
                    }
                printf("sum of rand nums is %d\n", sum);
                sem_post(&semaphore);
                sleep(2);

            }
            else
            {
                int avg;
                sleep(2);
                if (sem_wait(&semaphore) == -1) {perror("sem_wait");}
                shm_chld_pointer = (int *)shmat(shmid, NULL, 0);
                avg = summ(shm_chld_pointer, size)/size;
                if(shmdt(shm_chld_pointer) == -1)
                    {
                        perror("shmdt error");
                        exit(EXIT_FAILURE);
                    }
                printf("avg of rand nums is %d\n", avg);
                sem_post(&semaphore);
                sleep(1);
            }
        }
        else
        {
            if(id1 == 0)
            {
                int min, max;
                sleep(3);
                if (sem_wait(&semaphore) == -1) {perror("sem_wait");}
                shm_chld_pointer = (int *)shmat(shmid, NULL, 0);
                max = shm_chld_pointer[size - 1];
                shm_chld_pointer[1] = max;
                if(shmdt(shm_chld_pointer) == -1)
                {
                    perror("shmdt error");
                    exit(EXIT_FAILURE);
                }
                sem_post(&semaphore);
            }
            else
            {
                if (sem_wait(&semaphore) == -1) {perror("sem_wait");}
                shm_pointer = (int *)shmat(shmid, NULL, 0);
                srand(time(NULL));

                for(int i = 0; i < size; ++i)
                    shm_pointer[i] = rand() % 1233 + 1;
                insert_sort(shm_pointer, size);

                if(shmdt(shm_pointer) == -1)
                {
                    perror("shmdt error");
                    exit(EXIT_FAILURE);
                }
                sem_post(&semaphore);

                sleep(3);

                if (sem_wait(&semaphore) == -1) {perror("sem_wait");}
                shm_pointer = (int *)shmat(shmid, NULL, 0);
                printf("min = %d, max = %d \n", shm_pointer[0], shm_pointer[1]);
                ++counter;

                if(shmdt(shm_pointer) == -1)
                {
                    perror("shmdt error");
                    exit(EXIT_FAILURE);
                }
                sem_post(&semaphore);
            }
        }
    }
    exit(EXIT_SUCCESS);
