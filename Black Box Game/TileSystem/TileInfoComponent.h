#pragma once

#include <Actors/Component.h>
#include <Graphics/Texture.h>

class TileInfoComponent : public BlackBoxEngine::Component
{
public:
    using TexturePtr = std::shared_ptr<BlackBoxEngine::BB_Texture>;
    GENERATE_ID("TileInfoComponent");
private:
    const char* m_pTexturePath = nullptr;
    TexturePtr m_pTexture = nullptr;
    BlackBoxEngine::BB_FRectangle m_imageSource{ 0,0,0,0 };

    bool m_isWalkable = true;
    uint64_t m_encounterChance = 0;

public:
    TileInfoComponent(BlackBoxEngine::Actor* pActor);

    void SetTexture(const char* pFilePath);
    void SetImageSource(BlackBoxEngine::BB_FRectangle source) { m_imageSource = source; }

    TexturePtr GetTexture() const { return m_pTexture; };
    BlackBoxEngine::BB_FRectangle GetSourceRectangle() const{ return m_imageSource; }

    virtual void Save(BlackBoxEngine::XMLElementParser parser) override;
    virtual void Load(const BlackBoxEngine::XMLElementParser parser) override;
    virtual void Start() override;

    bool IsWalkable() { return m_isWalkable; }
    bool CheckEncounter() const;
};