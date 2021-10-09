#ifndef MATH_DATA_HPP
#define MATH_DATA_HPP

#include "math.h"

struct vec2
{
    float x, y;

    vec2 operator+ (vec2 a)
    {
        return { x + a.x, y + a.y };
    }

    vec2 operator- (vec2 a)
    {
        return { x - a.x, y - a.y };
    }

    vec2 operator+ (float value)
    {
        return { x + value, y + value };
    }

    vec2 operator- (float value)
    {
        return { x - value, y - value };
    }

    vec2 operator* (float value)
    {
        return { x * value, y * value };
    }

    vec2 operator/ (float value)
    {
        return { x / value, y / value };
    }

    void operator+= (vec2 a)
    {
        x += a.x;
        y += a.y;
    }

    void operator*= (vec2 a)
    {
        x *= a.x;
        y *= a.y;
    }

    float &operator[] (int i)
    {
        switch (i)
        {
            case 0: return x;
            default: return y;
        }
    }
};

struct vec3
{
    float x, y, z;

    float &operator[] (int i)
    {
        switch (i)
        {
            case 0: return x;
            case 1: return y;
            default: return z;
        }
    }
};

struct vec4
{
    float x, y, z, w;

    float &operator[] (int i)
    {
        switch (i)
        {
            case 0: return x;
            case 1: return y;
            case 2: return z;
            default: return w;
        }
    }
};

struct mat4
{
    vec4 rows[4];

    vec4 &operator[] (int i)
    {
        return rows[i];
    }

    void raw (float out[4][4])
    {
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                out[i][j] = rows[i][j];
            }
        }
    }
};

inline mat4 identity ()
{
    mat4 matrix;

    // {1, 0, 0, 0}
    // {0, 1, 0, 0}
    // {0, 0, 1, 0}
    // {0, 0, 0, 1}

    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            matrix[i][j] = (i == j) ? 1 : 0;

    return matrix;
}

inline void copy (float in[4][4], float out[4][4])
{
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            out[i][j] = in[i][j];
}

inline mat4 mul (mat4 m1, mat4 m2)
{
    mat4 result;

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            float val = 0;

            for (int k = 0; k < 4; k++)
            {
                val += (m1[i][k] * m2[k][j]);
            }

            result[i][j] = val;
        }
    }

    return result;
}

inline void translate (mat4 &matrix, vec2 pos)
{
    mat4 trans_matrix = identity ();

    // {1, 0, 0, pos.x}
    // {0, 1, 0, pos.y}
    // {0, 0, 1, 0    }
    // {0, 0, 0, 1    }

    trans_matrix[0][3] = pos.x;
    trans_matrix[1][3] = pos.y;

    matrix = mul (matrix, trans_matrix);
}

inline void scale (mat4 &matrix, vec2 size)
{
    mat4 scale_matrix = identity ();

    // {size.x, 0,      0, 0}
    // {0,      size.y, 0, 0}
    // {0,      0,      1, 0}
    // {0,      0,      0, 1}

    scale_matrix[0][0] = size.x;
    scale_matrix[1][1] = size.y;

    matrix = mul (matrix, scale_matrix);
}

inline void rotate (mat4 &matrix, float angle)
{
    // {size.x, 0,      0, 0}
    // {0,      size.y, 0, 0}
    // {0,      0,      1, 0}
    // {0,      0,      0, 1}

    angle = angle * (3.1415f / 180.f);

    mat4 rotation_matrix = identity ();

    rotation_matrix[0][0] = cos (angle);
    rotation_matrix[0][1] = (-sin (angle));

    rotation_matrix[1][0] = sin (angle);
    rotation_matrix[1][1] = cos (angle);

    matrix = mul (matrix, rotation_matrix);
}

inline mat4 ortho (float W, float H)
{
    float r = W, t = 0;
    float l = 0, b = H;
    float f = 1, n = -1;

    mat4 matrix = identity ();

    matrix[0][0] = 2.f / (r - l);
    matrix[0][3] = -(r + l) / (r - l);

    matrix[1][1] = 2.f / (t - b);
    matrix[1][3] = -(t + b) / (t - b);

    matrix[2][2] = -2.f / (f - n);
    matrix[2][3] = -(f + n) / (f - n);

    return matrix;
}

#endif
