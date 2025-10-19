#include "InterfaceText.h"

#include "../BlackBoxManager.h"
#include "../Graphics/Text Rendering/Text.h"
#include "../Graphics/Text Rendering/TextRenderer.h"
#include "../Resources/ResourceManager.h"

namespace BlackBoxEngine
{

    void BlackBoxEngine::InterfaceText::RenderThis(BB_Renderer* pRenderer, float x, float y)
    {
        pRenderer->GetTextVariant()->RenderText(m_pText, x + m_nodeRenderRect.x, y + m_nodeRenderRect.y);
    }

    InterfaceText::~InterfaceText()
    {
        BlackBoxManager::Get()->m_pMessagingManager->RemoveListener( m_messageId );
    }

    BlackBoxEngine::InterfaceText::InterfaceText(
        InterfaceNode* pParent,
        const char* pName,
        BB_FRectangle dimensions,
        const Paremeters& params)
        : InterfaceNode(pParent, pName, dimensions)
        , m_params(params)
    {
        assert( params.pText );
        assert( params.pFontFile );
        m_pText = ResourceManager::GetText(params.pText, params.pFontFile, params.textSize);
        auto zoom = BlackBoxManager::Get()->m_pMainCamera->GetCameraWindowZoom(BlackBoxManager::Get()->GetWindow());
        m_pText->SetTextWrapWidthPixels(static_cast<int>(dimensions.w * zoom.x));

        auto ScreenSizeChanged = [this]([[maybe_unused]]const Message& message)
           {
               auto* pCamera = BlackBoxManager::Get()->m_pMainCamera;
               auto zoom = pCamera->GetCameraWindowZoom(BlackBoxManager::Get()->GetWindow());
               if ( m_nodeRenderRect.w <= 0 )
                   return;
               m_pText->SetTextWrapWidthPixels(static_cast<int>(m_nodeRenderRect.w * zoom.x));
           };
        m_messageId = BlackBoxManager::Get()->m_pMessagingManager->RegisterListenerString(
            "WindowSizeChanged", ScreenSizeChanged
        );
    }

}