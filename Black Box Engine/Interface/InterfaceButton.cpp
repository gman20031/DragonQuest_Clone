#include "InterfaceButton.h"

#include "../Graphics/Renderer.h"

namespace BlackBoxEngine
{
    void InterfaceButton::RenderThis(BB_Renderer* pRenderer, float rootX, float rootY)
    {
        BB_FRectangle dest = m_nodeDimensions;
        dest.x += rootX;
        dest.y += rootY;

        if (m_pTexture)
        {
            pRenderer->DrawTextureScreenColored(
                m_pTexture.get(),
                nullptr,
                &dest,
                &m_buttonColor
            );
        }
        else
            pRenderer->DrawRectScreenFilled(dest, m_buttonColor);
    }

    InterfaceButton::InterfaceButton(
        InterfaceNode* pParent,
        const char* pName,
        BB_FRectangle dimensions,
        const Parameters& params
    )
        : InterfaceNode(pParent, pName, dimensions)
        , m_params(params)
        , m_buttonColor(params.color)
    {

    }

    void InterfaceButton::OnTargeted()
    {
        m_buttonColor = m_params.targetedColor;
    }

    void InterfaceButton::OnInteracted()
    {
        m_buttonColor = m_params.interactColor;
    }


}

