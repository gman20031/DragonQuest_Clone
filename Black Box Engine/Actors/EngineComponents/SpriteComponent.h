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
        float m_width = 0;
        float m_height = 0;
        const char* m_pFilePath = nullptr;
        std::shared_ptr<BB_Texture> m_pTexture = nullptr;
        BB_Renderer* m_pRenderer = nullptr;
        TransformComponent* m_pTransform = nullptr;

    public:
        SpriteComponent(Actor* pOwner);

        void SetTexture(const char* pTexturePath);
        void SetDimensions(const float width, const float height);
        const BB_Point GetDimensions() const { return { m_width, m_height }; }

        virtual void Load(const XMLElementParser parser) override;
        virtual void Save(XMLElementParser parser) override;
        virtual void Render() override;
    };
};