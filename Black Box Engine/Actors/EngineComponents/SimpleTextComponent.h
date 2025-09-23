#pragma once

#include <memory>

#include "TransformComponent.h"
#include "../../Graphics/Color.h"
#include "../Component.h"

namespace BlackBoxEngine
{
    class BB_Font;
    class BB_Text;
    class BB_TextRenderer;

    class SimpleTextComponent : public Component
    {
    public:
        GENERATE_ID("SimpleTextComponent");
    private:
        FVector2 m_offset{ 0,0 };
        std::shared_ptr<BB_Text> m_pText;
        std::shared_ptr<BB_Font> m_pFont;
        BB_TextRenderer* m_pTextRenderer = nullptr;
        TransformComponent* m_pTransform = nullptr;

    public:
        SimpleTextComponent(Actor* pOwner);
        virtual ~SimpleTextComponent() = default;

        void SetTextWithFont(const char* pText, const char* pFontFilePath, float textSize);
        void SetFont(const char* pFontFilePath, float size);
        void SetText(const char* pText, ColorValue color = { 255,255,255,255 });
        void SetPointSize(float size);
        void SetOffset(FVector2 newOffset) { m_offset = newOffset; }
    
        const char* GetText() const;
        std::shared_ptr<BB_Font> GetFont() const { return m_pFont; }
        float GetPointSize() const;
        ColorValue GetColor() const;
        FVector2 GetOffset() const { return m_offset; }

        virtual void Load(const XMLElementParser parser) override;
        virtual void Save(XMLElementParser parser) override;
        virtual void Render() override;
    };

}