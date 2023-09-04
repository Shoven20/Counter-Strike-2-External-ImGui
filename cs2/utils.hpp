#pragma once
/* @NOTE: Sorry if it's a mess, I don't really feel like doing something hyper-structured and clean, again I share for those who want to understand the reasoning. Use what I've done as you wish no matter what. */





 
 
 
float ScreenCenterX;
float ScreenCenterY;

 
struct view_matrix_t {
    float* operator[ ](int index) {
        return matrix[index];
    }

    float matrix[4][4];
};
Vector2 screensize;
bool world_to_screen(const Vector3& pos, Vector3& out, view_matrix_t matrix) {
    out.x = matrix[0][0] * pos.x + matrix[0][1] * pos.y + matrix[0][2] * pos.z + matrix[0][3];
    out.y = matrix[1][0] * pos.x + matrix[1][1] * pos.y + matrix[1][2] * pos.z + matrix[1][3];

    float w = matrix[3][0] * pos.x + matrix[3][1] * pos.y + matrix[3][2] * pos.z + matrix[3][3];

    if (w < 0.01f)
        return false;

    float inv_w = 1.f / w;
    out.x *= inv_w;
    out.y *= inv_w;

    float x = screensize.x * .5f;
    float y = screensize.y * .5f;

    x += 0.5f * out.x * screensize.x + 0.5f;
    y -= 0.5f * out.y * screensize.y + 0.5f;

    out.x = x;
    out.y = y;

    return true;
}

namespace detail
{
    HWND window = 0;

    BOOL CALLBACK enum_windows(HWND hwnd, LPARAM param)
    {
        DWORD process_id;
        GetWindowThreadProcessId(hwnd, &process_id);
        if (process_id == param)
        {
            window = hwnd;
            return false;
        }
        return true;
    }
}
