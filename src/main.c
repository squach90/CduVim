/*
    Use case : make
               ./main --file (to open one)
               ./main        (just to see the editor)
*/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdbool.h>

#define MAX_LINES 100
#define MAX_LEN 1024

int main(int argc, char *argv[]) {
    struct termios old, new;
    char c;
    int lign = 1;
    int index = 0;
    char buffer[MAX_LEN];
    char* mode[] = {"", "-- INSERT --"};
    int currentMode = 0;
    int previousMode = currentMode;
    bool cmdMod = false;
    int openedFile = false;

    int cmdIndex = 0;
    char cmdBuffer[256];

    char lines[MAX_LINES][MAX_LEN] = {0};

    // File reader if one
    if (argc > 1) {
        FILE *file = fopen(argv[1], "r");
        if (file) {
            lign = 0;
            while (fgets(lines[lign], MAX_LEN, file)) {
                size_t len = strlen(lines[lign]);
                if (len > 0 && lines[lign][len - 1] == '\n') {
                    lines[lign][len - 1] = '\0';
                }
                lign++;
                if (lign >= MAX_LINES) break;
            }
            fclose(file);
            openedFile = true;
        } else {
            printf("Impossible d'ouvrir le fichier : %s\n", argv[1]);
        }
    }


    // Récupère la taille du terminal
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    int lastLine = w.ws_row;

    // Affiche le contenu du fichier
    if (openedFile) {
        for (int i = 0; i < lign; i++) {
            printf("%d %s\n", i + 1, lines[i]);
        }
    }


    tcgetattr(STDIN_FILENO, &old);
    new = old;
    new.c_lflag &= ~(ICANON | ECHO | ISIG | IEXTEN);
    tcsetattr(STDIN_FILENO, TCSANOW, &new);

    if (openedFile) {
        lign++;
    }
    printf("%d ", lign); // Current line
    fflush(stdout);

    while (1) {
        if (currentMode != previousMode) {
            printf("\0337");                 // save cursor
            printf("\033[%d;1H", lastLine);  // va à la dernière ligne
            printf("\033[K");                // efface la ligne
            printf("%s", mode[currentMode]); // affiche le mode
            printf("\0338");                 // restore cursor
            fflush(stdout);
            previousMode = currentMode;
        }

        c = getchar();
        if (c == 'i' && currentMode == 0) {
            currentMode = 1;
            printf("\0337");
            printf("\033[%d;1H", lastLine);
            printf("\033[K");
            printf("%s", mode[currentMode]);
            printf("\0338");
            fflush(stdout);
            continue;
        }

        if (c == ':') {
            cmdMod = true;
            printf("\0337");
            printf("\033[%d;1H", lastLine);
            printf("\033[K");
            printf(":");
            fflush(stdout);

            while (cmdMod) {
                char cmdChar = getchar();
                if (cmdChar == 10) { // Enter
                    cmdMod = false;
                    cmdBuffer[cmdIndex] = '\0';
                    if (strcmp(cmdBuffer, "q") == 0) {
                        system("clear");
                        tcsetattr(STDIN_FILENO, TCSANOW, &old);
                        return 0;
                    }

                    printf("\033[%d;1H", lastLine);
                    printf("\033[K");
                    printf("%s", mode[currentMode]);
                    printf("\0338");
                    fflush(stdout);

                    cmdIndex = 0;
                    break;
                }

                if (isprint(cmdChar)) {
                    cmdBuffer[cmdIndex++] = cmdChar;
                    printf("%c", cmdChar);
                    fflush(stdout);
                }
                if (cmdChar == 127 && cmdIndex > 0) { // Backspace
                    cmdIndex--;
                    cmdBuffer[cmdIndex] = '\0';
                    printf("\b \b");
                    fflush(stdout);
                }
            }
        }

        if (currentMode == 1) {
            if (isprint(c)) {
                buffer[index++] = c;
                printf("%c", c);
                fflush(stdout);

                printf("\0337");
                printf("\033[%d;1H", lastLine);
                printf("\033[K");
                printf("%s", mode[currentMode]);
                printf("\0338");
                fflush(stdout);
            }

            if (c == 127 && index > 0) {
                index--;
                buffer[index] = '\0';
                printf("\b \b");
                fflush(stdout);
            }

            if (c == 10) {
                buffer[index] = '\0';
                strcpy(lines[lign - 1], buffer);
                index = 0;
                lign++;
                printf("\n%d ", lign);
                fflush(stdout);
            }

            if (c == 27) currentMode = 0; // ESC
        }
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &old);
    return 0;
}
