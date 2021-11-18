#include "base64.hpp"
#include "utils.hpp"
#include <dirent.h>
#include <unistd.h>

#include <SDL2/SDL.h>

string lines = "#ifndef EMBEDDED_DATA\n#define EMBEDDED_DATA\n";

void push_wav (const char *variable, const char *filename)
{
    SDL_RWops *rw = SDL_RWFromFile (filename, "rb");

    if (rw == NULL)
        return;

    Sint64 res_size = SDL_RWsize (rw);
    char * res      = (char *)malloc (res_size + 1);

    Sint64 nb_read_total = 0, nb_read = 1;
    char * buf = res;

    while (nb_read_total < res_size && nb_read != 0)
    {
        nb_read = SDL_RWread (rw, buf, 1, (res_size - nb_read_total));
        nb_read_total += nb_read;
        buf += nb_read;
    }

    SDL_RWclose (rw);

    if (nb_read_total != res_size)
    {
        free (res);
        return;
    }

    string line = "const char *";

    line += variable;
    line += " = \"";
    line += base64_encode ((BYTE *)res, res_size);
    line += "\";\n";

    lines += line;

    if (line.data)
        free (line.data);
}

void push_shader (const char *variable, const char *filename)
{
    FILE *file = fopen (filename, "r");

    if (!file)
        return;

    string line = "const char *";
    line += variable;
    line += " = \"";

    char c       = fgetc (file);
    int  comment = 0;

    while (c != EOF)
    {
        if (c == '/')
            comment++;
        else if (c == '\n')
        {
            line += "\\n";
            comment = 0;
        }
        else if (c != '\n' && comment < 2)
        {
            if (comment == 1)
                line += '/';

            line += c;

            comment = 0;
        }

        c = fgetc (file);
    }

    line += "\";\n";

    lines += line;

    if (line.data)
        free (line.data);

    fclose (file);
}

int main (int argc, char **argv)
{
    FILE *file = fopen ("embedded_data.hpp", "w+b");

    DIR *          d = opendir (".");
    struct dirent *dir;

    if (d)
    {
        while ((dir = readdir (d)) != NULL)
        {
            int    dot = 0;
            string extension;

            for (int i = 0; i < strlen (dir->d_name); i++)
            {
                if (dir->d_name[i] == '.')
                {
                    dot = i + 1;
                }
                else if (dot)
                {
                    extension.push (dir->d_name[i]);
                }
            }

            string variable = "";

            for (int i = 0; i < dot - 1; i++)
                variable += dir->d_name[i];

            variable += "_raw";

            if (extension == "wav")
            {
                push_wav (variable.c_str (), dir->d_name);
            }
            else if (extension == "glsl")
            {
                push_shader (variable.c_str (), dir->d_name);
            }
        }

        closedir (d);

        lines += "\n#endif";

        fwrite (lines.data, 1, lines.length, file);
    }
}
