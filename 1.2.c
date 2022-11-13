#include <stdio.h>
#include <stdlib.h>

// 1.2 (1 балл). Написать программу, порождающую дочерний процесс.
// Затем и родительский, и дочерний процессы выводят значения
// аргументов запуска
// (int main (int argc, char *argv[])).
// Добавить функцию, вызываемую при нормальном завершении работы
// программы и проверить ее работу.
int main(int argc, char *argv[])
{
  pid_t p;
  p = fork();


  if (p == 0) {
    for (int i = 1; i < argc; i++) {
      printf("Argument (Parent) %d: %s\n", i, argv[i]);
    }
  }

  else {
    for(int i = 1; i < argc; i++) {
      printf("Argument (Child) %d: %s\n", i, argv[i]);
    }
  }

  printf("\nSuccess!\n");
  return 0;
}
