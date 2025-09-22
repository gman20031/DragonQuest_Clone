#pragma once
#include <iostream>

namespace BlackBoxEngine
{

    struct FVector2
    {
        FVector2(float _x, float _y) : x(_x), y(_y) {}
        ~FVector2() = default;

        bool operator==(const FVector2& other) const;
        friend auto operator<=>(const FVector2& lhs, const FVector2& rhs) = default;
        
        FVector2 operator-() const;

        FVector2& operator+=(const FVector2& rhs);
        FVector2& operator-=(const FVector2& rhs);
        FVector2& operator/=(const float rhs);
        FVector2& operator*=(const float rhs);
        //Increment/decrement
        FVector2& operator++();
        FVector2& operator--();
        FVector2 operator++(int);
        FVector2 operator--(int);

        float x;
        float y;

        static const FVector2& GetUnitVector();

        float GetLength() const;
        float GetSquareLength() const;
        float GetDotProduct(const FVector2& right) const;
        bool Zero() const { return x == 0 && y == 0; }

        void Normalize();
        FVector2 GetNormalizedVector() const;
    };

    std::ostream& operator<<(std::ostream& os, const FVector2& vector);

    FVector2 operator+(const FVector2& lhs, const FVector2& rhs);
    FVector2 operator-(const FVector2& lhs, const FVector2& rhs);
    FVector2 operator*(const FVector2& lhs, float rhs);
    FVector2 operator/(const FVector2& lhs, float rhs);
    FVector2 operator*(float lhs, const FVector2& rhs);
    FVector2 operator/(float lhs, const FVector2& rhs);

}
