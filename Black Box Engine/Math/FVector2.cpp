#include "FVector2.h"

namespace BlackBoxEngine
{
    const FVector2& FVector2::GetUnitVector()
    {
        static FVector2 unitVector(1, 1);
        return unitVector;
    }

    float FVector2::GetLength() const
    {
        return std::sqrt(GetSquareLength());
    }

    float FVector2::GetSquareLength() const
    {
        return (x * x) + (y * y);
    }

    float FVector2::GetDotProduct(const FVector2& right) const
    {
        return (
            x * right.x +
            y * right.y
        );
    }

    void FVector2::Normalize()
    {
        *this = *this / GetLength();
    }

    FVector2 FVector2::GetNormalizedVector() const
    {
        FVector2 temp = *this;
        temp.Normalize();
        return temp;
    }
    
    bool FVector2::operator==(const FVector2& other) const
    {
        return (
            this->x == other.x &&
            this->y == other.y
            );
    }

    FVector2 FVector2::operator-() const
    {
        return FVector2(-x, -y);
    }
   
    FVector2& FVector2::operator+=(const FVector2& rhs)
    {
        *this = *this + rhs;
        return *this;
    }
   
    FVector2& FVector2::operator-=(const FVector2& rhs)
    {
        *this = *this - rhs;
        return *this;
    }
    
    FVector2& FVector2::operator/=(const float rhs)
    {
        *this = *this / rhs;
        return *this;
    }
    
    FVector2& FVector2::operator*=(const float rhs)
    {
        *this = *this * rhs;
        return *this;
    }
    
    FVector2& FVector2::operator++()
    {
        ++x;
        ++y;
        return *this;
    }
    
    FVector2& FVector2::operator--()
    {
        --x;
        --y;
        return *this;
    }

    FVector2 FVector2::operator++(int)
    {
        FVector2 old = *this;
        ++x;
        ++y;
        return old;
    }

    FVector2 FVector2::operator--(int)
    {
        FVector2 old = *this;
        --x;
        --y;
        return old;
    }

    std::ostream& operator<<(std::ostream& os, const FVector2& vector)
    {
        os << vector.x << ',' << vector.y;
        return os;
    }

    FVector2 operator+(const FVector2& lhs, const FVector2& rhs)
    {
        return FVector2(
            lhs.x + rhs.x,
            lhs.y + rhs.y
        );
    }

    FVector2 operator-(const FVector2& lhs, const FVector2& rhs)
    {
        return FVector2(
            lhs.x - rhs.x,
            lhs.y - rhs.y
        );
    }

    FVector2 operator*(const FVector2& lhs, float rhs)
    {
        return FVector2(
            lhs.x * rhs,
            lhs.y * rhs
        );
    }
    
     FVector2 operator/(const FVector2& lhs, float rhs)
    {
        return FVector2(
            lhs.x / rhs,
            lhs.y / rhs
        );
    }
    
    FVector2 operator*(float lhs, const FVector2& rhs)
    {
        return FVector2(
            lhs - rhs.x,
            lhs - rhs.y
        );
    }
    
    FVector2 operator/(float lhs, const FVector2& rhs)
    {
        return FVector2(
            lhs - rhs.x,
            lhs - rhs.y
        );
    }
}

