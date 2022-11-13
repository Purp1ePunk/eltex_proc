#include <stdio.h>
#include <stdlib.h>

// 1.3 (2 балла). Написать программу, вычисляющую площади квадратов
// с заданной длиной стороны. Длины сторон передаются как аргументы
// запуска. Расчеты делают родительский и дочерний процессы, разделяя
// задачи примерно поровну.

int main(int argc, char *argv[])
{
  pid_t p;
  int task1 = argc / 2;

  p = fork();

  if (p == 0) {
    for (int i = 1; i < task1+1; i++) {
      printf("CHILD: %f\n", atof(argv[i]) * atof(argv[i]));
    }
  }

  else {
    for (int i = task1+1; i < argc; i++) {
      printf("PARENT: %f\n", atof(argv[i]) * atof(argv[i]));
    }
  }

  printf("\n ---Success--- \n");
  return 0;
}
