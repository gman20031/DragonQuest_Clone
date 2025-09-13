#include "SimpleTextComponent.h"

#include "../../System/Log.h"
#include "../../Actors/Actor.h"
#include "../../Resources/ResourceManager.h"

#include "../../Graphics/Text Rendering/Font.h"
#include "../../Graphics/Text Rendering/Text.h"
#include "../../Graphics/Text Rendering/TextRenderer.h"

namespace BlackBoxEngine
{
    SimpleTextComponent::SimpleTextComponent(Actor* pOwner)
        : Component(pOwner)
    {
        m_pTransform = FindOrCreateComponent<TransformComponent>(pOwner);
    }

    void SimpleTextComponent::SetTextWithFont(const char* pText, const char* pFontFilePath, float textSize)
    {
        SetFont(pFontFilePath , textSize);
        SetText(pText);
    }
    void SimpleTextComponent::SetFont(const char* pFontFilePath, float size)
    {
        m_pFont = ResourceManager::GetFont(pFontFilePath, size);
    }

    void SimpleTextComponent::SetText(const char* pText, ColorValue color)
    {
        if (!m_pFont)
        {
            BB_LOG(LogType::kError, "Can create text without a font, font is nullptr");
            return;
        }
        m_pText = ResourceManager::GetText(pText, m_pFont);
        m_pText->SetColor(color);
    }

    void SimpleTextComponent::SetPointSize(float size)
    {
        if (!m_pFont)
        {
            BB_LOG(LogType::kError, "need to set text Font before setting size or text");
            return;
        }
        if (!m_pText)
        {
            BB_LOG(LogType::kError, "need to set text before changing text size");
            return;
        }
        m_pFont = ResourceManager::GetFont(m_pFont->GetFontFilePath() , size);
        SetText(m_pText->GetString());
    }

    void SimpleTextComponent::Load(const XMLElementParser parser)
    {
        const char* pfilePath = nullptr;
        const char* ptextString = nullptr;
        float textSize = 0;

        parser.GetChildVariable("FontFilePath", &pfilePath);
        parser.GetChildVariable("Textcontents",  &ptextString);
        parser.GetChildVariable("TextSize", &textSize);

        SetFont(pfilePath, textSize);
        SetText(ptextString);
    }

    void SimpleTextComponent::Save(XMLElementParser parser)
    {
        parser.NewChildVariable("FontFilePath", m_pFont->GetFontFilePath() );
        parser.NewChildVariable("Textcontents", m_pText->GetString() );
        parser.NewChildVariable("TextSize", m_pFont->GetFontSize() );
    }

    void SimpleTextComponent::Render()
    {
        FVector2 pos = m_pTransform->m_position;
        pos += m_offset;
        m_pTextRenderer->RenderText(m_pText, pos.x, pos.y);
    }

    const char* SimpleTextComponent::GetText() const
    {
        return m_pText->GetString();
    
    }
    float SimpleTextComponent::GetPointSize() const
    {
        return m_pText->GetPointSize();
    }
}




