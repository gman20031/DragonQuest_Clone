#pragma once

#include "../Graphics/Color.h"
#include "UserInterface.h"

namespace BlackBoxEngine
{
    class BB_Text;

    class InterfaceText : public InterfaceNode
    {
    public:
        struct Paremeters
        {
            const char* pFontFile;
            const char* pText;
            float textSize;
            ColorValue color = ColorPresets::white;
        };

    private:
        Paremeters m_params;
        std::shared_ptr<BB_Text> m_pText;
        uint64_t m_messageId;

        virtual void RenderThis(BB_Renderer* pRenderer, float x, float y) override;
    public:
        InterfaceText(InterfaceNode* pParent, const char* pName, BB_FRectangle dimensions, const Paremeters& params);
        virtual ~InterfaceText();

        std::shared_ptr<BB_Text> GetText() { return m_pText; }
    };

}
