#pragma once

#include <Resources/XML/BlackBoxXMLParser.h>

class GameStarter
{
    const char* m_pPlayerInfoXML;
    BlackBoxEngine::XMLElementParser m_playerStatParser;

    int   m_level;
    int   m_exp;
    int   m_hp;
    int   m_maxHP;
    int   m_mp;
    int   m_maxMP;
    int   m_gold;
    int   m_agility ;
    int   m_strength;
    float m_x = 0;
    float m_y = 0;

public:
    GameStarter( const char * pPlayerInfoXML );
    virtual ~GameStarter() = default;

    void SetPlayerStats( BlackBoxEngine::Actor* pPlayer ) const;
    void SetPlayerPosition( BlackBoxEngine::Actor* pPlayer ) const;
    void StartGamePlayer( BlackBoxEngine::Actor* pPlayer ) const;
    void Start() const;

};
