// C pangram checker
// Reads a C source file, ignores comments and lists unique characters used as a table

#include <stdbool.h>
#include <stdio.h>

// isUsed[0] = 0x20 (space)
// isUsed[1] = 0x21 (!)
// ...
// isUsed[95] = 0x7E (~)
bool isUsed[96];

typedef enum {
    IN_CODE,
    IN_CODE_SLASH,
    IN_LINE_COMMENT,
    IN_BLOCK_COMMENT,
    IN_BLOCK_COMMENT_STAR /*,
    IN_STRING_LITERAL,
    IN_CHARACTER_LITERAL*/
} State;

int main(int argc, char const *argv[]) {
    if (argc > 2) {
        fprintf(stderr, "Usage: %s [file]\n", argv[0]);
        fprintf(stderr, "If no file is specified, stdin is used.\n");
        return 1;
    }
    FILE *fp;
    if (argc == 2) {
        fp = fopen(argv[1], "r");
        if (fp == NULL) {
            perror(argv[1]);
            return 1;
        }
    } else {
        fp = stdin;
    }

    // Reads file character by character and sets isUsed[] accordingly until EOF
    // if encountered line comment, ignores until newline
    // if encountered block comment, ignores until */
    // if encountered string literal, ignores until closing "
    // if encountered character literal, ignores until closing '
    State s = IN_CODE;
    int c;
    while ((c = fgetc(fp)) != EOF) {
        switch (s) {
            case IN_CODE:
                if (c == '/') {
                    s = IN_CODE_SLASH;
                /*
                } else if (c == '"') {
                    s = IN_STRING_LITERAL;
                } else if (c == '\'') {
                    s = IN_CHARACTER_LITERAL;
                */
                } else {
                    isUsed[c - 0x20] = true;
                }
                break;
            case IN_CODE_SLASH:
                if (c == '/') {
                    s = IN_LINE_COMMENT;
                } else if (c == '*') {
                    s = IN_BLOCK_COMMENT;
                } else {
                    s = IN_CODE;
                    isUsed['/' - 0x20] = true;
                    isUsed[c - 0x20] = true;
                }
                break;
            case IN_LINE_COMMENT:
                if (c == '\n') {
                    s = IN_CODE;
                }
                break;
            case IN_BLOCK_COMMENT:
                if (c == '*') {
                    s = IN_BLOCK_COMMENT_STAR;
                }
                break;
            case IN_BLOCK_COMMENT_STAR:
                if (c == '/') {
                    s = IN_CODE;
                } else if (c != '*') {
                    s = IN_BLOCK_COMMENT;
                }
                break;
            /*
            case IN_STRING_LITERAL:
                if (c == '\\') {
                    fgetc(fp);
                }
                if (c == '"') {
                    s = IN_CODE;
                }
                break;
            case IN_CHARACTER_LITERAL:
                if (c == '\\') {
                    fgetc(fp);
                }
                if (c == '\'') {
                    s = IN_CODE;
                }
                break;
            */
        }
    }

    // Prints out the characters used, in table
    bool isPangram = true;
    printf("   0 1 2 3 4 5 6 7 8 9 A B C D E F\n");
    for (int i = 0; i < 96; i += 16) {
        printf("%02x", i + 0x20);
        for (int j = 0; j < 16; j++) {
            if (isUsed[i + j]) {
                printf(" %c", i + j + 0x20);
            } else {
                isPangram = false;
                printf("  ");
            }
        }
        printf("\n");
    }
    printf("The input is %sa pangram.\n", isPangram ? "" : "not ");

    return 0;
}
