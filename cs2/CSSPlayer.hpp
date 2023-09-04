#pragma once
#include <thread>
#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include "Driver.h"

struct Vector2 {
public:
    float x;
    float y;

    inline Vector2() : x(0), y(0) {}
    inline Vector2(float x, float y) : x(x), y(y) {}

    inline float Distance(Vector2 v) {
        return sqrtf(((v.x - x) * (v.x - x) + (v.y - y) * (v.y - y)));
    }

    inline Vector2 operator+(const Vector2& v) const {
        return Vector2(x + v.x, y + v.y);
    }

    inline Vector2 operator-(const Vector2& v) const {
        return Vector2(x - v.x, y - v.y);
    }
    void Normalize()
    {
        if (x > 89.0f)
            x -= 180.f;

        if (x < -89.0f)
            x += 180.f;

        if (y > 180.f)
            y -= 360.f;

        if (y < -180.f)
            y += 360.f;
    }
};
struct Vector3
{
    // constructor
    constexpr Vector3(
        const float x = 0.f,
        const float y = 0.f,
        const float z = 0.f) noexcept :
        x(x), y(y), z(z) { }

    inline static float sqrtf(float number)
    {
        long i;
        float x2, y;
        const float threehalfs = 1.5F;

        x2 = number * 0.5F;
        y = number;
        i = *(long*)&y;
        i = 0x5f3759df - (i >> 1);
        y = *(float*)&i;
        y = y * (threehalfs - (x2 * y * y));
        y = y * (threehalfs - (x2 * y * y));

        return 1 / y;
    }

    // operator overloads
    constexpr const Vector3& operator-(const Vector3& other) const noexcept
    {
        return Vector3{ x - other.x, y - other.y, z - other.z };
    }

    constexpr const Vector3& operator+(const Vector3& other) const noexcept
    {
        return Vector3{ x + other.x, y + other.y, z + other.z };
    }

    constexpr const Vector3& operator/(const float factor) const noexcept
    {
        return Vector3{ x / factor, y / factor, z / factor };
    }

    constexpr const Vector3& operator*(const float factor) const noexcept
    {
        return Vector3{ x * factor, y * factor, z * factor };
    }

    // utils
    constexpr const Vector3& ToAngle() const noexcept
    {
        return Vector3{
            std::atan2(-z, std::hypot(x, y)) * (180.0f / (float)3.14),
            std::atan2(y, x) * (180.0f / (float)3.14),
            0.0f };
    }

    constexpr const bool IsZero() const noexcept
    {
        return x == 0.f && y == 0.f && z == 0.f;
    }

    float distancee(Vector3 vec)
    {
        return sqrt(
            pow(vec.x - x, 2) +
            pow(vec.y - y, 2)
        );
    }



    float x, y, z;
};

constexpr DWORD64 s_dwEntityList_Offset = 0x1679C38;
constexpr DWORD64 viewmatrix_Offset = 0x1713CF0;
constexpr DWORD64 localplayer_Offset = 0x1713348;

constexpr DWORD64 s_dwPawnHealth_Offset = 0x808;
constexpr DWORD64 s_dwPlayerPawn_Offset = 0x5dc;
constexpr DWORD64 s_dwSanitizedName_Offset = 0x720;
constexpr DWORD64 s_Position_Offset = 0x12AC;
constexpr DWORD64 s_teamnum_Offset = 0x3bf;
constexpr DWORD64 s_bIsLocalPlayerController_Offset = 0x6A0;
constexpr DWORD64 s_bPawnHasDefuser = 0x810;

DWORD64 g_dwEntList = 0;

class C_BaseEntity;
class CEntityIdentity
{
public:
    C_BaseEntity* m_pPrevByClass()
    {
        C_BaseEntity* pPrevByClass1 = read<C_BaseEntity*>((DWORD64)(this) + 0x68);
        C_BaseEntity* pPrevByClass = read<C_BaseEntity*>((DWORD64)pPrevByClass1);
        
        return pPrevByClass;
    }

    C_BaseEntity* m_pNextByClass()
    {
        C_BaseEntity* pNextByClass1 = read<C_BaseEntity*>((DWORD64)(this) + 0x70);
        C_BaseEntity* pNextByClass = read<C_BaseEntity*>((DWORD64)pNextByClass1);

        return pNextByClass;
    }

    std::string m_designerName()
    {
        std::string sBuffer = { };
        sBuffer.resize(32);

        DWORD64 pDesignerName = read<DWORD64>((DWORD64)(this) + 0x20);
        if (!pDesignerName)
            return { };

        sBuffer = read_str(pDesignerName);

        return sBuffer;
    }
};

class C_BaseEntity
{
public:
    static uintptr_t getPlayer(int index) {
        return read<uintptr_t>(g_dwEntList + index * 0x10);
    }
    static C_BaseEntity* GetBaseEntity(int nIdx)
    {
        std::uint64_t ListEntry = read<std::uint64_t>(g_dwEntList + (8 * (nIdx & 0x7FFF) >> 9) + 16);
        if (!ListEntry)
            return 0;
        C_BaseEntity* ListEntry2 = read<C_BaseEntity*>((ListEntry + 120 * (nIdx & 0x1FF)));

        return ListEntry2;
    }
public:
    CEntityIdentity* m_pEntityIdentity()
    {
        CEntityIdentity* a = read<CEntityIdentity*>((DWORD64)(this) + 0x10);
        return a;
    }
};

class C_CSPlayerPawn
{
public:
    static C_CSPlayerPawn* GetPlayerPawn(std::uint32_t uHandle)
    {
        std::uint64_t ListEntry = read<std::uint64_t>(g_dwEntList + 0x8 * ((uHandle & 0x7FFF) >> 9) + 16);
        if (!ListEntry)
            return 0;

        C_CSPlayerPawn* pCSPlayerPawn = read<C_CSPlayerPawn*>(ListEntry + 120 * (uHandle & 0x1FF));

        return pCSPlayerPawn;
    }
};

class CCSPlayerController : public C_BaseEntity
{
public:
    std::string m_sSanitizedPlayerName()
    {
        std::string sBuffer = { };
        sBuffer.resize(32);

        DWORD64 SanitizedPlayerName = read<DWORD64>(((DWORD64)(this) + s_dwSanitizedName_Offset));
        if (!SanitizedPlayerName)
            return { };

        sBuffer = read_str(SanitizedPlayerName);
        return sBuffer;
    }

    int Teamnum()
    {
        int team = read<int>((DWORD64)(this) + s_teamnum_Offset);
        return team;
    }   
    
    bool HaveDefuseKit()
    {
        bool team = read<bool>((DWORD64)(this) + s_bPawnHasDefuser);
        return team;
    }

    int m_iPawnHealth()
    {
        DWORD64 health = read<DWORD64>((DWORD64)(this) + s_dwPawnHealth_Offset);
        return health;
    }

    Vector3 pos(C_CSPlayerPawn* pawn)
    {
        return read<Vector3>((DWORD64)pawn + s_Position_Offset);
    }


    bool m_bIsLocalPlayerController()
    {
        return read<bool>((DWORD64)(this) + s_bIsLocalPlayerController_Offset);
    } 

    C_CSPlayerPawn* m_hPlayerPawn()
    {
        std::uint32_t playerpawn = read<std::uint32_t>((DWORD64)(this) + s_dwPlayerPawn_Offset);
        return C_CSPlayerPawn::GetPlayerPawn(playerpawn);
    }
};
