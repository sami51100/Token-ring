#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>
#include <sys/fcntl.h>
#include <sys/time.h>

int saut;

int main(int argc, const char *argv[])
{

    int n = atoi(argv[1]);
    int m = atoi(argv[2]);
    int z = 1;

    struct timeval t1, t2;
    double temp;

    if (argc != 3)
    {
        printf("Nombre d'argument invalide\n");
        exit(EXIT_FAILURE);
    }

    else
    {

        pid_t k;
        int i = 0;
        int tube[n][2];

        printf("processus à créer : %d \nnombre sauts : %d\n\n", n, m);

        for (i = 0; i < n; i++)
        {
            pipe(tube[i]);
        }

        for (int i = 0; i < n; i++)
        {

            k = fork();

            if (k > 0)
            {
                printf("processus fils cree [%d] : PID : %d\n", i, k);
            }
            else if (!k)
            {
                // creation du tube
                int in = 0;
                int out = 0;

                int finsaut = 0;
                int nbVisite = 1;

                char id[3] = "id";
                char PATH[10];
                sprintf(PATH, "%s%d", id, i);

                // visite du fichier
                char visite[20] = "visite_numero_";
                char vis[20];
                sprintf(vis, "%s%d", visite, z);
                int file;

                mkdir(PATH, 0755);

                if (chdir(PATH) != 0)
                {
                    fprintf(stderr, "erreur placement dans le dossier %s.\n", PATH);
                    exit(EXIT_FAILURE);
                }

                sleep(1);

                if (i == 0)
                {
                    write((tube[(i + 1) % n][1]), &z, sizeof(int));
                    printf("\nProcessus [%d] :  envoie du jeton \n\nSaut %d\n", i, z);
                }

                sleep(1);

                while (!finsaut)
                {
                    gettimeofday(&t1, NULL);
                    in = tube[i][0];
                    out = tube[(i + 1) % n][1];

                    for (int j = 0; j < n; j++)
                    {
                        if (tube[j][0] != in)
                        {
                            close(tube[j][0]);
                        }

                        else if (tube[j][1] != out)
                        {
                            close(tube[j][1]);
                        }
                    }

                    read(in, &saut, sizeof saut);
                    printf("\nProcessus [%d] : jeton recu \nVisite: %d\n", i, nbVisite);

                    sprintf(vis, "%s%d", visite, nbVisite);

                    file = open(vis, O_CREAT | O_WRONLY, 0606);

                    if (file < 0)
                    {
                        printf("erreur ouverture du fichier en écriture.\n");
                        exit(EXIT_FAILURE);
                    }

                    nbVisite++;
                    saut++;
                    if (saut >= (m + 1))
                    {
                        finsaut = 1;
                    }

                    else
                    {
                        printf("Processus [%d] :  envoie du jeton \n\nSaut %d\n", i, saut);
                        write(out, &saut, sizeof(int));
                    }

                    gettimeofday(&t2, NULL);
                    temp = ((t2.tv_sec - t1.tv_sec) * 1000000L + t2.tv_usec) - t1.tv_usec;

                    write(file, &temp, sizeof(double));

                    close(file);
                }

                close(in);
                close(out);

                break;
            }

            else
                printf("Erreur a fork()");
        }

        if (k > 0)
        {
            for (i = 0; i < n; ++i)
                wait(NULL);
        }
    }

    return 0;
}
