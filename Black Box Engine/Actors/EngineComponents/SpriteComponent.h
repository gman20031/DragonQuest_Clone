#pragma once

#include <memory>

#include "TransformComponent.h"
#include "../Component.h"
#include "../../Resources/TextureFactory.h"
#include "../../Graphics/Renderer.h"

namespace BlackBoxEngine
{
    class SpriteComponent : public Component
    {
    public:
        GENERATE_ID("SpriteComponent");

    private:
        const char* m_pFilePath = nullptr;
        std::shared_ptr<BB_Texture> m_pTexture = nullptr;
        BB_Renderer* m_pRenderer = nullptr;
        TransformComponent* m_pTransform = nullptr;

        bool m_useFullImage = true;
        BB_FRectangle m_textureOffset{ 0,0,0,0 };

        float m_width = 0;
        float m_height = 0;
    public:
        SpriteComponent(Actor* pOwner);
        virtual ~SpriteComponent() = default;

        void SetTexture(const char* pTexturePath);
        void SetTextureOffset(BB_FRectangle offset);
        void SetDimensions(const float width, const float height);
        const BB_FPoint GetDimensions() const { return { m_width, m_height }; }

        virtual void Load(const XMLElementParser parser) override;
        virtual void Save(XMLElementParser parser) override;
        virtual void Render() override;
    };
};