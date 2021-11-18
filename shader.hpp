#ifndef SHADER
#define SHADER

#include "globals.hpp"
#include "math_data.hpp"
#include "texture.hpp"
#include "time_data.hpp"
#include "utils.hpp"

#include "embedded_data.hpp"

enum FLIP
{
    FLIP_X_TRUE  = 1,
    FLIP_Y_TRUE  = 2,
    FLIP_X_FALSE = 4,
    FLIP_Y_FALSE = 8,
};

struct Shader
{
    uint id, vertex_id, fragment_id, vao, vbo;
};

struct Square
{
    vec2  pos, size;
    vec4  color;
    float angle;
    bool  outline;
};

struct Sprite
{
    vec2  pos, size;
    vec4  sprite;
    float angle;
    uint  flip;
    float alpha;
};

struct Light
{
    vec2 pos, size;
    bool offset;
};

struct Glow
{
    vec2 pos, size;
    vec4 color;
};

struct Text
{
    vec2 pos;
    char string[512];
};

typedef array<Shader> Shaders;
typedef array<Square> Squares;
typedef array<Sprite> Sprites;
typedef array<Light>  Lights;
typedef array<Glow>   Glows;
typedef array<Text>   Texts;

Glows   glows;
Lights  lights;
Squares squares;
Sprites sprites;
Texts   texts;

inline void copy_to_array (Lights lights, vec2 *arr)
{
    for (int i = 0; i < lights.length; i++)
    {
        Light n = lights[i];

        if (lights[i].offset)
        {
            arr[i].x = ((n.pos.x + (n.size.x / 2) - camera.x) / W);
            arr[i].y = 1 - ((n.pos.y + (n.size.y / 2) - camera.y) / H);
        }
        else
        {
            arr[i].x = ((n.pos.x + (n.size.x / 2)) / W);
            arr[i].y = 1 - ((n.pos.y + (n.size.y / 2)) / H);
        }
    }
}

inline uint compile (const char *filename, GLenum type)
{
    uint  id  = glCreateShader (type);
    char *src = read_file (filename);

    glShaderSource (id, 1, &src, NULL);
    glCompileShader (id);

    GLint isCompiled = 0;
    glGetShaderiv (id, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv (id, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        char infoLog[512];
        glGetShaderInfoLog (id, maxLength, &maxLength, &infoLog[0]);

        // We don't need the shader anymore.
        glDeleteShader (id);

        // Use the infoLog as you see fit.

        printf ("%s\n", infoLog);

        // In this simple program, we'll just leave
        assert (1 != 1);
    }

    return id;
}

inline Shader create (const char *vertex_file, const char *fragment_file)
{
    Shader shader = {
        glCreateProgram (),
        compile (vertex_file, GL_VERTEX_SHADER),
        compile (fragment_file, GL_FRAGMENT_SHADER),
    };

    glAttachShader (shader.id, shader.vertex_id);
    glAttachShader (shader.id, shader.fragment_id);
    glLinkProgram (shader.id);

    GLint isLinked = 0;
    glGetProgramiv (shader.id, GL_LINK_STATUS, (int *)&isLinked);
    if (isLinked == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetProgramiv (shader.id, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        char infoLog[512];
        glGetProgramInfoLog (shader.id, maxLength, &maxLength, &infoLog[0]);

        // We don't need the program anymore.
        glDeleteProgram (shader.id);
        // Don't leak shaders either.
        glDeleteShader (shader.vertex_id);
        glDeleteShader (shader.fragment_id);

        // Use the infoLog as you see fit.

        printf ("%s\n", infoLog);

        // In this simple program, we'll just leave
        assert (1 != 1);
    }

    return shader;
}

inline uint compile_from_src (const char *src, GLenum type)
{
    uint id = glCreateShader (type);

    glShaderSource (id, 1, &src, NULL);
    glCompileShader (id);

    GLint isCompiled = 0;
    glGetShaderiv (id, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv (id, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        char infoLog[512];
        glGetShaderInfoLog (id, maxLength, &maxLength, &infoLog[0]);

        // We don't need the shader anymore.
        glDeleteShader (id);

        // Use the infoLog as you see fit.

        printf ("%s\n", infoLog);

        // In this simple program, we'll just leave
        assert (1 != 1);
    }

    return id;
}

inline Shader create_from_src (const char *vertex_src, const char *fragment_src)
{
    Shader shader = {
        glCreateProgram (),
        compile_from_src (vertex_src, GL_VERTEX_SHADER),
        compile_from_src (fragment_src, GL_FRAGMENT_SHADER),
    };

    glAttachShader (shader.id, shader.vertex_id);
    glAttachShader (shader.id, shader.fragment_id);
    glLinkProgram (shader.id);

    GLint isLinked = 0;
    glGetProgramiv (shader.id, GL_LINK_STATUS, (int *)&isLinked);
    if (isLinked == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetProgramiv (shader.id, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        char infoLog[512];
        glGetProgramInfoLog (shader.id, maxLength, &maxLength, &infoLog[0]);

        // We don't need the program anymore.
        glDeleteProgram (shader.id);
        // Don't leak shaders either.
        glDeleteShader (shader.vertex_id);
        glDeleteShader (shader.fragment_id);

        // Use the infoLog as you see fit.

        printf ("%s\n", infoLog);

        // In this simple program, we'll just leave
        assert (1 != 1);
    }

    return shader;
}

inline void set_uniform (Shader shader, const char *name, vec2 value)
{
    float data[2] = { value.x, value.y };

    glUniform2fv (glGetUniformLocation (shader.id, name), 2, data);
}

inline void set_uniform (Shader shader, const char *name, mat4 value)
{
    float matrix[4][4];

    value.raw (matrix);

    glUniformMatrix4fv (glGetUniformLocation (shader.id, name), 1, GL_TRUE,
                        matrix[0]);
}

inline void set_uniform (Shader shader, const char *name, int value)
{
    glUniform1i (glGetUniformLocation (shader.id, name), value);
}

inline void set_uniform (Shader shader, const char *name, float value)
{
    glUniform1f (glGetUniformLocation (shader.id, name), value);
}

inline void set_uniform (Shader shader, const char *name, vec4 value)
{
    float data[4] = { value.x, value.y, value.z, value.w };

    glUniform4fv (glGetUniformLocation (shader.id, name), 1, data);
}

inline void set_uniform (Shader shader, const char *name, vec2 val[], int size)
{
    float data[size * 2];

    for (int i = 0, j = 0; i < size; i++, j += 2)
    {
        data[j]     = val[i].x;
        data[j + 1] = val[i].y;
    }

    glUniform2fv (glGetUniformLocation (shader.id, name), size, data);
}

inline Shader init_default_shader ()
{
    const float points[] = {
        0.0f, 1.0f,    //
        1.0f, 0.0f,    //
        0.0f, 0.0f,    //

        0.0f, 1.0f,    //
        1.0f, 1.0f,    //
        1.0f, 0.0f,    //
    };

    Shader shader = create_from_src (vertex_raw, fragment_raw);

    glGenVertexArrays (1, &shader.vao);
    glGenBuffers (1, &shader.vbo);

    glBindVertexArray (shader.vao);
    glBindBuffer (GL_ARRAY_BUFFER, shader.vbo);

    glVertexAttribPointer (0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray (0);

    glBufferData (GL_ARRAY_BUFFER, sizeof (points), points, GL_STATIC_DRAW);

    glUseProgram (shader.id);
    set_uniform (shader, "u_image", 0);
    set_uniform (shader, "u_projection", ortho (W, H));

    return shader;
}

inline Shader init_lighting_shader ()
{
    float points[] = {
        -1.0f, 1.0f,  0.0f, 1.0f,    //
        -1.0f, -1.0f, 0.0f, 0.0f,    //
        1.0f,  -1.0f, 1.0f, 0.0f,    //

        -1.0f, 1.0f,  0.0f, 1.0f,    //
        1.0f,  -1.0f, 1.0f, 0.0f,    //
        1.0f,  1.0f,  1.0f, 1.0f,
    };

    Shader shader = create_from_src (light_vertex_raw, light_fragment_raw);

    glGenVertexArrays (1, &shader.vao);
    glGenBuffers (1, &shader.vbo);

    glBindVertexArray (shader.vao);

    glBindBuffer (GL_ARRAY_BUFFER, shader.vbo);
    glBufferData (GL_ARRAY_BUFFER, sizeof (points), &points, GL_STATIC_DRAW);
    glVertexAttribPointer (0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof (float),
                           (void *)0);
    glEnableVertexAttribArray (0);

    glUseProgram (shader.id);

    set_uniform (shader, "u_image", 0);
    set_uniform (shader, "u_projection", ortho (W, W));
    set_uniform (shader, "u_max_lights", 0);

    glUseProgram (0);

    return shader;
}

inline void draw (Shader shader, Square square)
{
    mat4 matrix = get_model (square.pos, square.size, square.angle);

    set_uniform (shader, "u_type", 1);
    set_uniform (shader, "u_color", square.color);
    set_uniform (shader, "u_model", matrix);

    // Outline...
    if (square.outline)
        glDrawArrays (GL_LINE_LOOP, 1, 4);
    else
        glDrawArrays (GL_TRIANGLES, 0, 6);
}

inline void set_sprite (Shader shader, Texture texture, vec4 sprite)
{
    vec4 transform = {
        sprite.x / texture.w,
        sprite.y / texture.h,
        sprite.z / texture.w,
        sprite.w / texture.h,
    };

    set_uniform (shader, "u_offset", transform);
}

inline void draw (Shader shader, Texture texture, Sprite sprite)
{
    mat4 matrix = get_model (sprite.pos, sprite.size, sprite.angle);

    set_sprite (shader, texture, sprite.sprite);

    if (sprite.flip & FLIP_X_TRUE)
        set_uniform (shader, "u_flip_x", true);
    else if (sprite.flip & FLIP_X_FALSE)
        set_uniform (shader, "u_flip_x", false);

    if (sprite.flip & FLIP_Y_TRUE)
        set_uniform (shader, "u_flip_y", true);
    else if (sprite.flip & FLIP_Y_FALSE)
        set_uniform (shader, "u_flip_y", false);

    set_uniform (shader, "u_alpha", sprite.alpha);

    set_uniform (shader, "u_type", 0);
    set_uniform (shader, "u_model", matrix);

    glDrawArrays (GL_TRIANGLES, 0, 6);
}

vec4 alphabet (const char i)
{
    switch (i)
    {
        case 'a': return { 0, 0, 8, 8 };
        case 'b': return { 8, 0, 8, 8 };
        case 'c': return { 16, 0, 8, 8 };
        case 'd': return { 24, 0, 8, 8 };
        case 'e': return { 32, 0, 8, 8 };
        case 'f': return { 40, 0, 8, 8 };

        case 'g': return { 48, 0, 8, 8 };
        case 'h': return { 56, 0, 8, 8 };
        case 'i': return { 64, 0, 8, 8 };
        case 'j': return { 72, 0, 8, 8 };

        case 'k': return { 0, 8, 8, 8 };
        case 'l': return { 8, 8, 8, 8 };
        case 'm': return { 16, 8, 8, 8 };
        case 'n': return { 24, 8, 8, 8 };
        case 'o': return { 32, 8, 8, 8 };
        case 'p': return { 40, 8, 8, 8 };
        case 'q': return { 48, 8, 8, 8 };
        case 'r': return { 56, 8, 8, 8 };
        case 's': return { 64, 8, 8, 8 };
        case 't': return { 72, 8, 8, 8 };

        case 'u': return { 0, 16, 8, 8 };
        case 'v': return { 8, 16, 8, 8 };
        case 'w': return { 16, 16, 8, 8 };
        case 'x': return { 24, 16, 8, 8 };
        case 'y': return { 32, 16, 8, 8 };
        case 'z': return { 40, 16, 8, 8 };
        case '.': return { 48, 16, 8, 8 };
        case ' ': return { 56, 16, 8, 8 };

        case '0': return { 0, 32, 8, 8 };
        case '1': return { 8, 32, 8, 8 };
        case '2': return { 16, 32, 8, 8 };
        case '3': return { 24, 32, 8, 8 };
        case '4': return { 32, 32, 8, 8 };
        case '5': return { 40, 32, 8, 8 };
        case '6': return { 48, 32, 8, 8 };
        case '7': return { 56, 32, 8, 8 };
        case '8': return { 64, 32, 8, 8 };
        case '9': return { 72, 32, 8, 8 };

        default: return { 0, 0, 8, 8 };
    }
}

inline void draw (Shader shader, Texture texture, Text text)
{
    set_uniform (shader, "u_flip_x", true);
    set_uniform (shader, "u_flip_y", true);

    set_uniform (shader, "u_type", 0);

    for (size_t i = 0; i < strlen (text.string); i++)
    {
        mat4 matrix = get_model (text.pos, { 8.f, 8.f }, 0);

        set_sprite (shader, texture, alphabet (text.string[i]));
        set_uniform (shader, "u_model", matrix);

        glDrawArrays (GL_TRIANGLES, 0, 6);

        text.pos.x += 6;
    }
}

inline void draw (Shader shader, Glow glow)
{
    glow.pos = glow.pos - (glow.size / 2.f);
    glow.pos = glow.pos - (vec2) { W - glow.size.x, H - glow.size.y };

    mat4 model = get_model (glow.pos, { W * 2, H * 2 });

    set_uniform (shader, "u_type", 2);
    set_uniform (shader, "u_model", model);
    set_uniform (shader, "u_color", glow.color);

    glDrawArrays (GL_TRIANGLES, 0, 6);
}

uint FBO, texture_color_buffer;

void init_frame_buffer (SDL_Window *window, bool erase = false)
{
    if (erase)
    {
        glDeleteFramebuffers (1, &FBO);
        glDeleteTextures (1, &texture_color_buffer);
    }

    int w, h, ratio;

    glGenFramebuffers (1, &FBO);
    glBindFramebuffer (GL_FRAMEBUFFER, FBO);

    glGenTextures (1, &texture_color_buffer);
    glBindTexture (GL_TEXTURE_2D, texture_color_buffer);

    SDL_GL_GetDrawableSize (window, &w, &h);

    if (maintain_aspect_ratio)
    {
        if (w > h)
            ratio = (int)(h / H) * H;
        else if (h > w || h == w)
            ratio = (int)(w / W) * W;

        w -= ratio;
        h -= ratio;
        glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, ratio, ratio, 0, GL_RGB,
                      GL_UNSIGNED_BYTE, NULL);
    }
    else
    {
        glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB,
                      GL_UNSIGNED_BYTE, NULL);
    }

    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                            texture_color_buffer, 0);

    if (glCheckFramebufferStatus (GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        printf ("ERROR::FRAMEBUFFER:: Framebuffer is not "
                "complete!");
    }

    glBindFramebuffer (GL_FRAMEBUFFER, 0);
}

void batch_render (SDL_Window *window, Shader light_shader,
                   Shader sprite_shader, Texture spritesheet,
                   Texture font_texture)
{
    int w, h, ratio;

    SDL_GL_GetDrawableSize (window, &w, &h);

    if (maintain_aspect_ratio)
    {
        if (w > h)
            ratio = (int)(h / H) * H;
        else if (h > w || h == w)
            ratio = (int)(w / W) * W;

        w -= ratio;
        h -= ratio;

        glViewport (0, 0, ratio, ratio);
    }
    else
    {
        glViewport (0, 0, w, h);
    }

    glBindFramebuffer (GL_FRAMEBUFFER, FBO);

    glClearColor (0.f, 0.0f, 0.0f, 1.f);
    glClear (GL_COLOR_BUFFER_BIT);

    glUseProgram (sprite_shader.id);
    glBindVertexArray (sprite_shader.vao);

    glActiveTexture (GL_TEXTURE0);
    glBindTexture (GL_TEXTURE_2D, spritesheet.id);

    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (int i = 0; i < sprites.length; i++)
        draw (sprite_shader, spritesheet, sprites[i]);

    for (int i = 0; i < squares.length; i++)
        draw (sprite_shader, squares[i]);

    if (maintain_aspect_ratio)
        glViewport (w / 2, h / 2, ratio, ratio);
    else
        glViewport (0, 0, w, h);

    glBindFramebuffer (GL_FRAMEBUFFER, 0);
    glClearColor (0.08f, 0.08f, 0.08f, 1.0f);
    glClear (GL_COLOR_BUFFER_BIT);

    glUseProgram (light_shader.id);
    glBindVertexArray (light_shader.vao);

    glActiveTexture (GL_TEXTURE0);
    glBindTexture (GL_TEXTURE_2D, texture_color_buffer);

    vec2 u_lights[lights.length];

    copy_to_array (lights, u_lights);

    set_uniform (light_shader, "u_max_lights", (int)lights.length);
    set_uniform (light_shader, "u_lights", u_lights, lights.length);

    glDrawArrays (GL_TRIANGLES, 0, 6);

    glActiveTexture (GL_TEXTURE0);
    glBindTexture (GL_TEXTURE_2D, spritesheet.id);

    glUseProgram (sprite_shader.id);
    glBindVertexArray (sprite_shader.vao);

    glBlendFunc (GL_SRC_ALPHA, GL_ONE);
    for (int i = 0; i < glows.length; i++)
        draw (sprite_shader, glows[i]);

    glActiveTexture (GL_TEXTURE0);
    glBindTexture (GL_TEXTURE_2D, font_texture.id);

    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Text t;
    sprintf (t.string, "fps %.2f", time_data::fps);
    // strcpy (t.string, "marco wey que pedo");
    t.pos = { 0, 0 };

    draw (sprite_shader, font_texture, t);

    SDL_GL_SwapWindow (window);

    squares.length = sprites.length = glows.length = texts.length
        = lights.length                            = 0;
}

#endif
