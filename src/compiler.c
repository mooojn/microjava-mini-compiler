#include <stdio.h>
#include <string.h>

int lexer_main(int argc, char **argv);
int recursive_main(int argc, char **argv);
int predictive_main(int argc, char **argv);
int lr_main(int argc, char **argv);

static void print_usage(const char *program) {
    fprintf(stderr, "Usage: %s <lexer|recursive|predictive|lr|all> <source.mj> [--dump-symbols]\n", program);
}

static int run_one(const char *mode, const char *program, const char *source, const char *option) {
    char *argv_with_option[3];
    char *argv_plain[2];

    argv_plain[0] = (char *)program;
    argv_plain[1] = (char *)source;

    argv_with_option[0] = (char *)program;
    argv_with_option[1] = (char *)source;
    argv_with_option[2] = (char *)option;

    if (strcmp(mode, "lexer") == 0) {
        return lexer_main(2, argv_plain);
    }
    if (strcmp(mode, "recursive") == 0) {
        return option == NULL ? recursive_main(2, argv_plain) : recursive_main(3, argv_with_option);
    }
    if (strcmp(mode, "predictive") == 0) {
        return predictive_main(2, argv_plain);
    }
    if (strcmp(mode, "lr") == 0) {
        return lr_main(2, argv_plain);
    }

    fprintf(stderr, "Unknown mode: %s\n", mode);
    return 1;
}

int main(int argc, char **argv) {
    const char *mode;
    const char *source;
    const char *option = NULL;
    int status = 0;

    if (argc < 3 || argc > 4) {
        print_usage(argv[0]);
        return 1;
    }

    mode = argv[1];
    source = argv[2];
    if (argc == 4) {
        option = argv[3];
    }

    if (strcmp(mode, "all") == 0) {
        printf("== Recursive descent parser ==\n");
        status |= run_one("recursive", "recursive", source, option);
        printf("\n== Predictive parser ==\n");
        status |= run_one("predictive", "predictive", source, NULL);
        printf("\n== LR parser ==\n");
        status |= run_one("lr", "lr", source, NULL);
        return status == 0 ? 0 : 1;
    }

    return run_one(mode, argv[0], source, option);
}
