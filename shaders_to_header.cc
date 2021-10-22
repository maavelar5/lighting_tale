#include <iostream>
#include <vector>

std::string lines = "#ifndef SHADER_SHIT\n#define SHADER_SHIT\n";

void push (std::string variable, const char *filename)
{
    FILE *file = fopen (filename, "r");

    if (!file)
        return;

    lines += "const char* " + variable + " = \"";

    char c       = fgetc (file);
    int  comment = 0;

    while (c != EOF)
    {
        if (c == '/')
            comment++;
        else if (c == '\n')
        {
            lines += "\\n";
            comment = 0;
        }
        else if (c != '\n' && comment < 2)
        {
            if (comment == 1)
                lines += '/';

            lines += c;

            comment = 0;
        }

        c = fgetc (file);
    }

    lines += "\";\n";

    fclose (file);
}

int main (int argc, char **argv)
{
    push ("vertex", "vertex.glsl");
    push ("fragment", "fragment.glsl");

    push ("light_vertex", "light_vertex.glsl");
    push ("light_fragment", "light_fragment.glsl");

    lines += "#endif";

    FILE *file = fopen ("shader_release.hpp", "w+");

    if (!file)
        return -1;

    fputs (lines.c_str (), file);

    fclose (file);

    std::cout << lines;
}
