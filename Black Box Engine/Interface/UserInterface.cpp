#include "UserInterface.h"

#include "../BlackBoxManager.h"
#include "../Graphics/Renderer.h"
#include "../Graphics/Sprite.h"
#include "../Resources/ResourceManager.h"

namespace BlackBoxEngine
{
    ////////////////////////////////////////////////////////////////
    /// Interface Highlighter
    ////////////////////////////////////////////////////////////////

    void InterfaceHighlighter::UpdateRenderPosition(InterfaceNode* pNode)
    {
        m_renderDestination = pNode->GetRenderBox();
        BB_FPoint screenPos = pNode->GetScreenPosition();
        m_renderDestination.x = screenPos.x;
        m_renderDestination.y = screenPos.y;
        m_iconDestination.x = m_renderDestination.x + m_params.iconOffset.x;
        m_iconDestination.y = m_renderDestination.y + m_params.iconOffset.y;
    }

    void InterfaceHighlighter::RenderUnderline(BB_Renderer* pRenderer) const
    {
        BB_FRectangle dest = m_renderDestination;
        dest.y += m_renderDestination.h - m_params.lineWidth;
        dest.h = m_params.lineWidth;
        pRenderer->DrawRectScreenFilled(dest, m_params.underlineColor);
    }

    void InterfaceHighlighter::RenderColor(BB_Renderer* pRenderer) const
    {
        pRenderer->DrawRectScreenFilled(m_renderDestination, m_params.highlighterColor);
    }

    InterfaceHighlighter::InterfaceHighlighter(UserInterface* pAttachedInterface)
        : m_pAttachedInterface(pAttachedInterface)
    {
    }

    InterfaceHighlighter::~InterfaceHighlighter()
    {
        if ( m_pSprite )
            delete m_pSprite;
    }

    void InterfaceHighlighter::Render(BB_Renderer* pRenderer) const
    {
        if ( m_params.mode & kModeIcon )
            m_pSprite->Render( m_iconDestination , true );
        if ( m_params.mode & kModeColored )
            RenderColor( pRenderer );
        if ( m_params.mode & kModeUnderline )
            RenderUnderline( pRenderer );
    }

    void InterfaceHighlighter::Start()
    {
        if ( m_pSprite )
            m_pSprite->GameStart();
    }
    
    void InterfaceHighlighter::Stop()
    {
        if ( m_pSprite )
            m_pSprite->StopAnimating();
    }

    void InterfaceHighlighter::SetTarget(InterfaceNode* pNode)
    {
        assert( pNode );
        m_pHighlightedNode = pNode;
        m_renderDestination = pNode->GetRenderBox();
        UpdateRenderPosition(pNode);
    }

    void InterfaceHighlighter::SetMode(uint8_t mode)
    {
        m_params.mode = mode;
        UpdateRenderPosition(m_pHighlightedNode);
    }

    void InterfaceHighlighter::SetParameters(const Parameters& params)
    {
        m_params = params;
        if ( params.mode & kModeIcon && !params.pSpriteFile )
        {
            m_params.mode |= ~kModeIcon;
            BB_LOG( LogType::kError, "Set highlighter mode to icon without providing file" );
        }

        if ( m_params.mode & kModeIcon )
        {
            m_pSprite = new Sprite;
            m_pSprite->Load( ResourceManager::GetRawXMLDATA(params.pSpriteFile) );
            if ( params.iconSize.x == 0 || params.iconSize.y == 0 )
                BB_LOG( LogType::kError, "Set highlight mode to icon without providing icon size, will not render" );
            m_iconDestination.w = params.iconSize.x;
            m_iconDestination.h = params.iconSize.y;
        }
    }

    ////////////////////////////////////////////////////////////////
    /// User Interface
    ////////////////////////////////////////////////////////////////

    void UserInterface::MoveCursor(Direction dir)
    {
        if ( !m_pCursorPosition )
            return;
        auto* pNode = m_pCursorPosition->GetAdjacentNode(dir);
        if (!pNode)
            return;
        m_pCursorPosition->OnTargetedStop();
        m_pCursorPosition = pNode;
        pNode->OnTargeted();
        m_highlighter.SetTarget(pNode);
    }

    void UserInterface::SelectTargetedNode()
    {
        if ( !m_pCursorPosition )
            return;
        m_pCursorPosition->OnInteracted();
    }

    void UserInterface::DeSelectTargetNode()
    {
        if ( !m_pCursorPosition )
        {
            BB_LOG( LogType::kWarning, "Attempted to deselect node without a target" );
            return;
        }
        m_pCursorPosition->OnInteractStop();
    }

    void UserInterface::SetupKeysForInputTarget()
    {
        using enum Direction;
        
        assert( m_pInputTarget );

        // ensure we dont overlap keys
        m_pInputTarget->m_keyDown.ClearListeners();
        m_pInputTarget->m_keyUp.ClearListeners();
        
        m_pInputTarget->m_keyDown.RegisterListener(m_keycodes.up,   [this]() {MoveCursor(kUp); });
        m_pInputTarget->m_keyDown.RegisterListener(m_keycodes.down, [this]() {MoveCursor(kDown); });
        m_pInputTarget->m_keyDown.RegisterListener(m_keycodes.right,[this]() {MoveCursor(kRight); });
        m_pInputTarget->m_keyDown.RegisterListener(m_keycodes.left, [this]() {MoveCursor(kLeft); });
        m_pInputTarget->m_keyDown.RegisterListener(m_keycodes.select, [this]() {SelectTargetedNode(); });
        m_pInputTarget->m_keyUp.RegisterListener(m_keycodes.select, [this]() {DeSelectTargetNode(); });
    }

    void UserInterface::Start()
    {
        m_pRootNode->Start();
        m_highlighter.Start();
    }

    void UserInterface::Stop()
    {
        m_pRootNode->Stop();
        m_highlighter.Stop();
    }

    UserInterface::UserInterface()
        : m_highlighter(this)
        , m_pInputTarget(new InputManager::InputTarget)
    {
        m_pRootNode = new InterfaceNode(nullptr, "_root");
        SetupKeysForInputTarget();
    }

    UserInterface::~UserInterface()
    {
        assert(m_pRootNode);
        BlackBoxManager::Get()->m_pInterfaceManager->RemoveInterface( m_managerId );
        delete m_pRootNode;
    }

    InputManager::InputTarget* UserInterface::GetInputTarget()
    {
        assert( m_pInputTarget );
        return m_pInputTarget;
    }

    void UserInterface::SetInterfaceKeys(const InterfaceKeys& keys)
    {
        m_keycodes = keys;
        SetupKeysForInputTarget();
    }

    void UserInterface::SetInterfaceKeys(KeyCode up, KeyCode down, KeyCode right, KeyCode left, KeyCode select)
    {
        m_keycodes.up = up;
        m_keycodes.down = down;
        m_keycodes.left = left;
        m_keycodes.right = right;
        m_keycodes.select = select;
        SetupKeysForInputTarget();
    }

    InterfaceNode* UserInterface::FindNode(const char* pName)
    {
        return m_pRootNode->GetChildNode(pName);
    }

    void UserInterface::SetCursorTarget(InterfaceNode* pNode)
    {
        m_pCursorPosition = pNode;
        if ( !pNode )
            return;
        pNode->OnTargeted();
        m_highlighter.SetTarget(pNode);
    }

    void UserInterface::AddToScreen()
    {
        m_managerId =  BlackBoxManager::Get()->m_pInterfaceManager->AddInterfaceToScreen(this);
        Start();
    }

    void UserInterface::RemoveFromScreen()
    {
        if (m_managerId < 0)
            return;
        BlackBoxManager::Get()->m_pInterfaceManager->RemoveInterface(m_managerId);
        m_managerId = -1;
        Stop();
    }

    void UserInterface::Update()
    {
        if(m_pRootNode)
            m_pRootNode->Update();
    }

    void UserInterface::Render([[maybe_unused]]BB_Renderer* pRenderer)
    {
        assert(m_pRootNode);
        m_pRootNode->Render(pRenderer, 0, 0);
        m_highlighter.Render(pRenderer);
    }

    ////////////////////////////////////////////////////////////////
    /// Interface Node
    ////////////////////////////////////////////////////////////////

    InterfaceNode::InterfaceNode(InterfaceNode* pParent , const char* pName)
        : InterfaceNode(pParent, pName, {0,0,0,0})
    {

    }

    InterfaceNode::InterfaceNode(InterfaceNode* pParent, const char* pName, BB_FRectangle dimensions)
        : m_pParent(pParent)
        , m_nodeRenderRect(dimensions)
        , m_nameHash(StringHash(pName))
    {

    }

    InterfaceNode::~InterfaceNode()
    {
        RemoveAllChildNodes();
    }

    void InterfaceNode::SetOffset(float x, float y)
    {
        m_nodeRenderRect.x = x;
        m_nodeRenderRect.y = y;
    }

    void InterfaceNode::SetSize(float w, float h)
    {
        m_nodeRenderRect.w = w;
        m_nodeRenderRect.h = h;
    }

    void InterfaceNode::Start()
    {
        StartThis();
        for (auto& node : m_childNodes)
            node->Start();
    }

    void InterfaceNode::Stop()
    {
        StopThis();
        for ( auto& node : m_childNodes )
            node->Stop();
    }

    void InterfaceNode::Update()
    {
        UpdateThis();
        for (auto& node : m_childNodes)
            node->Update();
    }

    void InterfaceNode::Render(BB_Renderer* pRenderer, float rootX, float rootY)
    {
        RenderThis(pRenderer, rootX, rootY);
        for (auto& node : m_childNodes)
            node->Render(pRenderer, rootX + m_nodeRenderRect.x, rootY + m_nodeRenderRect.y);
    }

    InterfaceNode* InterfaceNode::GetAdjacentNode(Direction dir) const
    {
        return m_adjacentNodes[static_cast<uint32_t>(dir)];
    }

    void InterfaceNode::SetAdjacentNode(Direction dir, InterfaceNode* pNode)
    {
        m_adjacentNodes[static_cast<uint32_t>(dir)] = pNode;
    }

    InterfaceNode* InterfaceNode::GetChildNode(const char* pName)
    {
        return GetChildNode(StringHash(pName));
    }

    InterfaceNode* InterfaceNode::GetChildNode(uint32_t nameHash)
    {
        if (nameHash == m_nameHash)
            return this;
        if (m_childNodes.empty())
            return nullptr;
        for (auto* pChild : m_childNodes)
        {
            auto* pTest = pChild->GetChildNode(nameHash);
            if (pTest)
                return pTest;
        }

        // child node not attached
        return nullptr; 
    }

    void InterfaceNode::RemoveAllChildNodes()
    {
        for ( auto* pNode : m_childNodes )
            delete pNode;
        m_childNodes.clear();
    }

    void InterfaceNode::RemoveChildNode( InterfaceNode* pChildNode )
    {
        RemoveChildNode( pChildNode->m_nameHash );
    }

    void InterfaceNode::RemoveChildNode( const char* pName )
    {
        RemoveChildNode( StringHash( pName ) );
    }

    void InterfaceNode::RemoveChildNode( uint32_t nameHash )
    {
        for ( auto it = m_childNodes.begin() ; it != m_childNodes.end() ; ++it)
        {
            auto* pChildNode = *it;
            if ( pChildNode->m_nameHash == nameHash )
            {
                delete pChildNode;
                m_childNodes.erase( it );
                return;
            }
        }
    }

    BB_FPoint InterfaceNode::GetScreenPosition()
    {
        if (m_pParent)
        {
            BB_FPoint pos = m_pParent->GetScreenPosition();
            pos.x += m_nodeRenderRect.x;
            pos.y += m_nodeRenderRect.y;
            return pos;
        }
        else
            return { m_nodeRenderRect.x , m_nodeRenderRect.y };

    }
}