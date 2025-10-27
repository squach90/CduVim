
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdbool.h>

int main() {
    struct termios old, new;
    char c;
    int lign = 1;
    int index = 0;
    char buffer[1024]; // 1024 char max per line
    char* mode[] = {"", "-- INSERT --"};
    int currentMode = 0;
    int previousMode = currentMode;
    bool cmdMod = false;

    int cmdIndex = 0;
    char cmdBuffer[256];

    char lines[100][1024]; // 100 line max, 1024 char each

    
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    int lastLine = w.ws_row;
    
    
    // Get terminal Settings
    tcgetattr(STDIN_FILENO, &old);
    new = old;
    
    // turn off echo and canonical mode
    new.c_lflag &= ~(ICANON | ECHO | ISIG | IEXTEN);
    tcsetattr(STDIN_FILENO, TCSANOW, &new);
    
    printf("Vim in C :\n");
    
    printf("%d ", lign);
    while (1) {

        if (currentMode != previousMode) {
            // mettre à jour le mode
            printf("\0337");                 // save cursor
            printf("\033[%d;1H", lastLine);  // va à la dernière ligne
            printf("\033[K");                // efface la ligne
            printf("%s", mode[currentMode]); // affiche -- INSERT --
            printf("\0338");                 // restore cursor
            fflush(stdout);

            previousMode = currentMode;          // mémorise le mode actuel
        }


        c = getchar();
        if (c == 'i' && currentMode == 0) {
            currentMode = 1;
            
            printf("\0337");                 // save cursor
            printf("\033[%d;1H", lastLine);  // va à la dernière ligne
            printf("\033[K");                // efface la ligne
            printf("%s", mode[currentMode]); // affiche -- INSERT --
            printf("\0338");                 // restore cursor
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

                    // erase cmd line and go back to text
                    printf("\033[%d;1H", lastLine);
                    printf("\033[K");
                    printf("%s", mode[currentMode]);
                    printf("\0338");
                    fflush(stdout);

                    cmdIndex = 0; // reset cmdIndex (buffer)
                    break;
                }

                if (isprint(cmdChar)) {
                    cmdBuffer[cmdIndex++] = cmdChar;
                    printf("%c", cmdChar); // print what user is typing
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
                
                // Update status
                printf("\0337");
                printf("\033[%d;1H", lastLine);
                printf("\033[K");
                printf("%s", mode[currentMode]);
                printf("\0338");
                fflush(stdout);

            }
    
            if (c == 127 && index > 0) {
                index--;
                buffer[index] = '\0'; // erase char form buffer
                printf("\b \b");
                fflush(stdout);
            }

            if (c == 10) {
                buffer[index] = '\0';
                strcpy(lines[lign-1], buffer); // save ligne
                index = 0; // reset index (buffer)
                lign++;
                printf("\n%d ", lign);
            }

            if (c == 27) currentMode = 0; // ESC
        }

    }

    // Restaure le terminal
    tcsetattr(STDIN_FILENO, TCSANOW, &old);
    return 0;
}
