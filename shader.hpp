#ifndef SHADER
#define SHADER

#include "globals.hpp"
#include "math_data.hpp"
#include "texture.hpp"
#include "utils.hpp"

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

enum FLIP
{
    FLIP_X_TRUE  = 1,
    FLIP_Y_TRUE  = 2,
    FLIP_X_FALSE = 4,
    FLIP_Y_FALSE = 8,
};

struct Sprite
{
    vec2  pos, size;
    vec4  sprite;
    float angle;
    uint  flip;
};

struct Light
{
    vec2 pos, size;
};

struct Glow
{
    vec2 pos, size;
    vec4 color;
};

List<Light> lights;
List<Glow>  glows;

void copy_to_array (List<Light> lights, vec2 *arr)
{
    int i = 0;

    for (auto n = lights.first; n != 0; n = n->next, i++)
    {
        arr[i].x = ((n->data.pos.x + (n->data.size.x / 2) - camera.x) / W);
        arr[i].y = 1 - ((n->data.pos.y + (n->data.size.y / 2) - camera.y) / H);
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

    Shader shader = create ("vertex.glsl", "fragment.glsl");

    glGenVertexArrays (1, &shader.vao);
    glGenBuffers (1, &shader.vbo);

    glBindVertexArray (shader.vao);
    glBindBuffer (GL_ARRAY_BUFFER, shader.vbo);

    glVertexAttribPointer (0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray (0);

    glBufferData (GL_ARRAY_BUFFER, sizeof (points), points, GL_STATIC_DRAW);

    glUseProgram (shader.id);
    set_uniform (shader, "u_image", 0);
    set_uniform (shader, "u_projection", ortho (W, W));

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

    Shader shader = create ("light_vertex.glsl", "light_fragment.glsl");

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

    set_uniform (shader, "u_type", 0);
    set_uniform (shader, "u_image", sprite.sprite);
    set_uniform (shader, "u_model", matrix);

    glDrawArrays (GL_TRIANGLES, 0, 6);
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

List<Square> squares;
List<Sprite> sprites;

void batch_render (SDL_Window *window, Shader light_shader,
                   Shader sprite_shader, Texture spritesheet)
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
    for (auto i = sprites.first; i != limit (sprites); i = i->next)
        draw (sprite_shader, spritesheet, i->data);

    for (auto i = squares.first; i != limit (squares); i = i->next)
        draw (sprite_shader, i->data);

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

    vec2 u_lights[lights.size];

    copy_to_array (lights, u_lights);

    set_uniform (light_shader, "u_max_lights", (int)lights.size);
    set_uniform (light_shader, "u_lights", u_lights, lights.size);

    glDrawArrays (GL_TRIANGLES, 0, 6);

    glActiveTexture (GL_TEXTURE0);
    glBindTexture (GL_TEXTURE_2D, spritesheet.id);

    glUseProgram (sprite_shader.id);
    glBindVertexArray (sprite_shader.vao);

    glBlendFunc (GL_SRC_ALPHA, GL_ONE);
    for (auto i = glows.first; i != limit (glows); i = i->next)
        draw (sprite_shader, i->data);

    SDL_GL_SwapWindow (window);

    reset (squares);
    reset (sprites);
    reset (glows);
}

#endif
