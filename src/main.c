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
#include "../includes/fileReader.h"
#include "../includes/fileWriter.h"

#define MAX_LINES 100
#define MAX_LEN 1024

int main(int argc, char *argv[]) {
    struct termios old, new;
    char c;
    int lign = 0;              // nb of lines
    int index = 0;             // position in buffer
    char buffer[MAX_LEN];
    char* mode[] = {"", "-- INSERT --"}; // Can be -- NORMAL -- and -- INSERT --
    int currentMode = 0;
    int previousMode = currentMode;
    bool cmdMod = false;
    bool openedFile = false;

    int cmdIndex = 0;
    char cmdBuffer[256];

    char lines[MAX_LINES][MAX_LEN] = {0};

    // reading file if there is one
    if (argc > 1) {
        openedFile = readFile(argv[1], lines, &lign);
        if (!openedFile) {
            printf("Impossible d'ouvrir le fichier : %s\n", argv[1]);
        }
    }

    // get the terminal size
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    int lastLine = w.ws_row;

    // show file content
    for (int i = 0; i < lign; i++) {
        printf("%d %s\n", i + 1, lines[i]);
    }

    tcgetattr(STDIN_FILENO, &old);
    new = old;
    new.c_lflag &= ~(ICANON | ECHO | ISIG | IEXTEN);
    tcsetattr(STDIN_FILENO, TCSANOW, &new);

    printf("%d ", lign + 1);
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

        // Passage en mode insertion
        if (c == 'i' && currentMode == 0) {
            currentMode = 1;
            continue;
        }

        // Mode commande
        if (c == ':') {
            cmdMod = true;
            printf("\0337\033[%d;1H\033[K:", lastLine);
            fflush(stdout);

            while (cmdMod) {
                char cmdChar = getchar();
                if (cmdChar == 10) { // Enter
                    cmdMod = false;
                    cmdBuffer[cmdIndex] = '\0';

                    if (strcmp(cmdBuffer, "q") == 0) {
                        tcsetattr(STDIN_FILENO, TCSANOW, &old);
                        return 0;
                    }
                    if (strcmp(cmdBuffer, "w") == 0 || strcmp(cmdBuffer, "wq") == 0) {
                        if (index > 0) {           // if still char in buffer
                            buffer[index] = '\0';
                            strcpy(lines[lign], buffer);
                            lign++;
                            index = 0;
                        }
                        if (writeFile(argv[1], lines, lign)) {
                            printf("\nFichier sauvegardé avec succès !\n");
                            system("clear");
                        } else {
                            printf("\nErreur lors de la sauvegarde.\n");
                        }
                        if (strcmp(cmdBuffer, "wq") == 0) {
                            tcsetattr(STDIN_FILENO, TCSANOW, &old);
                            return 0;
                        }
                    }

                    cmdIndex = 0;
                    break;
                }

                if (isprint(cmdChar)) {
                    cmdBuffer[cmdIndex++] = cmdChar;
                    printf("%c", cmdChar);
                    fflush(stdout);
                } else if (cmdChar == 127 && cmdIndex > 0) { // Backspace
                    cmdIndex--;
                    cmdBuffer[cmdIndex] = '\0';
                    printf("\b \b");
                    fflush(stdout);
                }
            }
        }

        // Mode insertion
        if (currentMode == 1) {
            if (isprint(c)) {
                buffer[index++] = c;
                printf("%c", c);
                fflush(stdout);
            }

            if (c == 127 && index > 0) { // Backspace
                index--;
                buffer[index] = '\0';
                printf("\b \b");
                fflush(stdout);
            }

            if (c == 10) { // Enter
                buffer[index] = '\0';
                strcpy(lines[lign], buffer);
                lign++;        // incrémente seulement après avoir écrit la ligne
                index = 0;
                buffer[0] = '\0';
                printf("\n%d ", lign + 1);
                fflush(stdout);
            }

            if (c == 27) currentMode = 0; // ESC pour quitter le mode insertion
        }
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &old);
    return 0;
}
