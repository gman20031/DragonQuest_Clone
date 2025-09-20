#include "BoundingBox.h"

#include "../../Graphics/RenderingStructs.h"
#include <functional>

namespace BlackBoxEngine
{
    ////////////////////////////////////////////////////////////////////
    /// BoundingBox
    ////////////////////////////////////////////////////////////////////

    BoundingBox::BoundingBox(BB_FRectangle rect)
        : m_rect(rect)
    {
    }

    BoundingBox::BoundingBox(float x, float y, float w, float h)
        : m_rect{x,y,w,h}
    {
    }

    bool BoundingBox::Contains(FVector2 pos) const
    {                                   // POS CANT BE CONTAINED IF:                       |
        if (pos.x < Left())             // pos is more left than leftmost edge of box      |    
            return false;               //                                                 | AAAAAAAAAAAAA
        if (pos.x > Right())            // pos is more right than rightmost edge of box    | A           A
            return false;               //                                                 | A   B       A
        if (pos.y > Bottom())           // pos is farther down than bottommost edge of box | A           A
            return false;               //                                                 | A           A
        if (pos.y < Top())              // pos is higher up than topmost edge of box       | A           A
            return false;               //                                                 | AAAAAAAAAAAAA
        return true;                    //                                                 |
    }

    bool BoundingBox::Contains(const BoundingBox& other) const
    {                                   // OTHER  CANNOT BE CONTAINED IF :               | This = A , other = B
        if (Left() > other.Left())      // this left is farther RIGHT than other's left  |   
            return false;               //                                               |  AAAAAAAAAAAAA
        if (Right() < other.Right())    // this right is farther LEFT than bos's right   |  A           A
            return false;               //                                               |  A  BBBB     A
        if (Top() > other.Top())        // this top is LOWER than other's top            |  A  B  B     A
            return false;               //                                               |  A  BBBB     A
        if (Bottom() < other.Bottom())  // this bottom is HIGHER than other's bottom     |  A           A
            return false;               //                                               |  AAAAAAAAAAAAA 
        return true;                    // if all are true, other is contained           |   
    }                                                                                     

    bool BoundingBox::Intersects(const BoundingBox& other) const
    {                                   // CANNOT INTERSECT IF : 
        if (Left() > other.Right())     // this left is farther RIGHT than other's right |1   
            return false;               //                                               |2  AAAAAAAAAAAAA
        if (Right() < other.Left())     // this right is farther LEFT than box's left    |3  A           A
            return false;               //                                               |4  A       BBBBXBBBBB
        if (Top() > other.Bottom())     // this top is LOWER than other's Bottom         |5  A       B   A    B
            return false;               //                                               |6  A       B   A    B
        if (Bottom() < other.Top())     // this bottom is HIGHER than other's Top        |7  A       B   A    B
            return false;               //                                               |8  AAAAAAAAXAAAA    B
        return true;                    // if all are true, other is contained           |9          B        B
    }                                   //                                               |0          BBBBBBBBBB

}
