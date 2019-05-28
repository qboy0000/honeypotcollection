#include <stdio.h>
#include <stdlib.h>

#define FIFO_FILE       "/fifo/testfifo"

int main(int argc, char *argv[])
{
        FILE *fp;

        if ( argc != 2 ) {
                printf("USAGE: fifoclient [string]\n");
                exit(1);
        }

        if((fp = fopen(FIFO_FILE, "w")) == NULL) {
                perror("fopen");
                exit(1);
        }

        fprintf(fp, "%s\n", argv[1]);

        fclose(fp);
        return(0);
}
