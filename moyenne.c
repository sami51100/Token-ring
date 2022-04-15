#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <dirent.h>

int main(void)
{
    int n = 5;
    char rep1[4] = "id";
    char chemin[20];
    char visite[8] = "visite";
    int saut = 0, i = 0, j = 1;
    double m;
    int file;
    int offset;
    char PATH[20];

    double somme = 0;
    int div = 0;

    double moyGeneral[n];

    for (int k = 0; k < n; k++)
    {

        sprintf(PATH, "./%s%d", rep1, k);

        DIR *rep = opendir(PATH);

        if (rep != NULL)
        {
            struct dirent *ent;

            while ((ent = readdir(rep)) != NULL)
            {
                offset = telldir(rep);

                while (!saut && i <= 5)
                {
                    if (ent->d_name[i] != visite[i])
                    {
                        saut = 1;
                    }
                    i++;
                }
                if (!saut)
                {
                    sprintf(chemin, "./%s%d/%s", rep1, k, ent->d_name);

                    file = open(chemin, O_RDONLY);

                    if (file < 0)
                    {
                        printf("ouverture du fichier impossible \n");
                        return 0;
                    }

                    read(file, &m, sizeof(double));
                    somme += m;
                    div++;
                    close(file);
                }
                saut = 0;
                i = 0;
            }

            closedir(rep);
        }
        printf("TMA %d : %.2f/%d=%.2fms \n\n", k, somme, div, somme / div);
        moyGeneral[k] = somme / div;
        somme = 0;
        div = 0;
        m = 0;
    }

    for (int c = 0; c < n; c++)
    {

        somme += moyGeneral[c];
    }
    printf("TME:%.2fms \n", somme / n);
    return 0;
}