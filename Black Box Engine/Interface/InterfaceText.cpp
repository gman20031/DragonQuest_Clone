#include "InterfaceText.h"

#include "../Graphics/Text Rendering/Text.h"
#include "../Graphics/Text Rendering/TextRenderer.h"
#include "../Resources/ResourceManager.h"

namespace BlackBoxEngine
{

    void BlackBoxEngine::InterfaceText::RenderThis(BB_Renderer* pRenderer, float x, float y)
    {
        pRenderer->GetTextVariant()->RenderText(m_pText, x + m_nodeDimensions.x, y + m_nodeDimensions.y);
    }

    BlackBoxEngine::InterfaceText::InterfaceText(
        InterfaceNode* pParent,
        const char* pName,
        BB_FRectangle dimensions,
        const Parameters& params)
        : InterfaceNode(pParent, pName, dimensions)
        , m_params(params)
    {
        m_pText = ResourceManager::GetText(params.pText, params.pFontFile, params.textSize);
        m_pText->SetTextWrapWidthPixels(static_cast<int>(dimensions.w));
    }

}