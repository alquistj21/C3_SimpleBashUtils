#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct options {
  int e;
  int i;
  int v;
  int c;
  int l;
  int n;
} opt;

void GrepReadFlags(int argc, char *argv[], opt *options);
void GrepCount(opt *options, char *filename, char *pattern, int count_files);

int main(int argc, char *argv[]) {
  opt options = {0};
  GrepReadFlags(argc, argv, &options);
  char *pattern = argv[optind];
  optind++;
  int count_files = argc - optind;
  for (int i = optind; i < argc; i++) {
    char *filename = argv[i];
    GrepCount(&options, filename, pattern, count_files);
  }
  return 0;
}

void GrepReadFlags(int argc, char *argv[], opt *options) {
  int currentFlag;
  while ((currentFlag = getopt(argc, argv, "+eivcln")) != -1) {
    switch (currentFlag) {
      case 'e':
        options->e = 1;
        break;
      case 'i':
        options->i = 1;
        break;
      case 'v':
        options->v = 1;
        break;
      case 'c':
        options->c = 1;
        break;
      case 'l':
        options->l = 1;
        break;
      case 'n':
        options->n = 1;
        break;
      default:
        printf(
            "используйте флаги [-e, -i, -v, -c, -l, -n] образец [file_name]");
        exit(1);
    }
  }
}

void GrepCount(opt *options, char *filename, char *pattern, int count_files) {
  FILE *fp = fopen(filename, "r");
  if (fp) {
    char text[4096] = {0};
    int lines = 1, status, success = 0, loses = 0;
    int print_later = 0;
    int cflags = REG_EXTENDED;
    if (options->i) cflags = REG_ICASE;
    regex_t reg;
    regmatch_t pmatch[1];
    regcomp(&reg, pattern, cflags);
    while (fgets(text, 4096 - 1, fp) != NULL) {
      status = regexec(&reg, text, 1, pmatch, 0);
      if (strchr(text, '\n') == NULL) strcat(text, "\n");

      if (!status) {
        success++;

        if (options->c || options->l) {
          print_later = 1;
          continue;
        }
        if (!options->v) {
          if (count_files > 1) printf("%s:", filename);
        }
        if (options->n && !options->v) printf("%d:", lines);
        if (!options->v) printf("%s", text);

      } else if (options->v) {
        loses++;
        if (options->c || options->l) {
          print_later = 1;
          continue;
        }
        if (count_files > 1) printf("%s:", filename);
        if (options->n) printf("%d:", lines);
        printf("%s", text);
      }
      lines++;
    }

    if (print_later) {
      if (!options->l && count_files > 1) printf("%s:", filename);
      if (options->c && !options->v) printf("%d\n", success);
      if (options->c && options->v && !options->l) printf("%d\n", loses);
      if (options->l) printf("%s\n", filename);
    }

    regfree(&reg);
    fclose(fp);
  } else {
    fprintf(stderr, "файл не может быть открыт\n");
  }
}
