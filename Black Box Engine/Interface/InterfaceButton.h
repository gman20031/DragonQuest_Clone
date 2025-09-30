#pragma once

#include <functional>

#include "../Graphics/Color.h"
#include "UserInterface.h"

namespace BlackBoxEngine
{
    using ButtonCallbackFunctionPtr = std::function<void()>;

    class InterfaceButton : public InterfaceNode
    {
    public:
        /**
        * Usable
        * uouseInteractable
        * pTextureFile
        * color
        * targetedColor
        * interactColor
        * callbackFunction
        */
        struct ButtonParams
        {
            bool usable = false;
            bool mouseInteractable = false;
            const char* pTextureFile = nullptr;
            ColorValue color = ColorPresets::blue;
            ColorValue targetedColor = ColorPresets::red;
            ColorValue interactColor = ColorPresets::white;
            ButtonCallbackFunctionPtr callbackFunction;
        };
    private:
        ButtonParams m_params;
        std::shared_ptr<BB_Texture> m_pTexture = nullptr;
        ColorValue m_buttonColor;

        virtual void UpdateThis();
        virtual void RenderThis(BB_Renderer* pRenderer, float rootX, float rootY) override;
    public:
        InterfaceButton(InterfaceNode* pParent, const char* pName, BB_FRectangle dimensions, const ButtonParams& params);
        virtual ~InterfaceButton() = default;

        virtual void OnTargeted() override;
        virtual void OnTargetedStop() override;
        virtual void OnInteracted() override;
        virtual void OnInteractStop() override;
    };

}
