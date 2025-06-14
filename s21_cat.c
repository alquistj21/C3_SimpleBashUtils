#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  int b;
  int e;
  int n;
  int s;
  int t;
  int v;
} Flags;

int CatReadFlags(int argc, char *argv[], Flags *options);
int Cat(int argc, char *argv[], Flags *options);
void CatFile(char *file, Flags *options);

int main(int argc, char *argv[]) {
  Flags options = {0};
  CatReadFlags(argc, argv, &options);
  Cat(argc, argv, &options);
}

int CatReadFlags(int argc, char *argv[], Flags *options) {
  int Flag = 1, currentFlag, longFlag;
  static struct option longOptions[] = {{"number-nonblank", 0, 0, 'b'},
                                        {"number", 0, 0, 'n'},
                                        {"squeeze-blank", 0, 0, 's'},
                                        {0, 0, 0, 0}};
  while ((currentFlag = getopt_long(argc, argv, "benstvET", longOptions,
                                    &longFlag)) != -1) {
    switch (currentFlag) {
      case 'b':
        options->b = 1;
        options->n = 0;
        break;
      case 'e':
        options->e = 1;
        options->v = 1;
        break;
      case 'n':
        options->n = options->b ? 0 : 1;
        break;
      case 's':
        options->s = 1;
        break;
      case 't':
        options->t = 1;
        options->v = 1;
        break;
      case 'v':
        options->v = 1;
        break;
      case 'E':
        options->e = 1;
        break;
      case 'T':
        options->t = 1;
        break;
      default:
        Flag = 0;
        fprintf(stderr, "no this flag");
    }
  }
  return Flag;
}

int Cat(int argc, char *argv[], Flags *options) {
  int counter = 0;
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] != '-') {
      counter = 1;
      CatFile(argv[i], options);
    }
  }
  if (!counter) {
    fprintf(stderr, "no file");
  }
  return counter;
}

void CatFile(char *file, Flags *options) {
  FILE *txt_doc = fopen(file, "r");
  if (txt_doc == NULL) {
    fprintf(stderr, "Нет такого файла: %s !", file);
  } else {
    int symbol_file = fgetc(txt_doc);
    int l1 = 1, l2 = 1, l3 = 0, count = 1;
    while (!feof(txt_doc)) {
      if (options->b && l1 && symbol_file != '\n') {
        printf("%6d\t", count++);
      }
      if (options->e && symbol_file == '\n') {
        if (l3 != -1) {
          printf("$");
        }
      }
      if (options->n && l1) {
        if (l3 != -1) {
          printf("%6d\t", count++);
        }
      }
      if (options->s && l1 && symbol_file == '\n') {
        if (l3) {
          l3 = -1;
        }
      }
      if (options->t && symbol_file == '\t') {
        printf("^I");
        l3 = -1;
      }
      if (options->v) {
        int ch = (int)symbol_file;
        if (symbol_file < 0) {
          symbol_file &= 127;
          ch = (int)symbol_file;
          ch += 128;
        }
        if (ch != 9 && ch != 10 && ch < 32) {
          printf("^");
          symbol_file += 64;
        } else if (ch == 127) {
          printf("^");
          symbol_file = '?';
        } else if (ch > 160 && ch <= 255) {
          symbol_file -= 128;
        }
      }
      if (l3 != -1) {
        printf("%c", symbol_file);
      }
      l1 = (symbol_file == '\n') ? 1 : 0;
      l3 = (l2 && symbol_file == '\n') ? 1 : 0;
      l2 = (l1 && symbol_file == '\n') ? 1 : 0;
      symbol_file = fgetc(txt_doc);
    }
    fclose(txt_doc);
  }
}