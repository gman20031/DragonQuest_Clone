#include "UserInterface.h"

#include "../BlackBoxManager.h"
#include "../Graphics/Renderer.h"
#include "../Resources/ResourceManager.h"
#include "../BlackBoxManager.h"

namespace BlackBoxEngine
{
    ////////////////////////////////////////////////////////////////
    /// Interface Highlighter
    ////////////////////////////////////////////////////////////////


    void InterfaceHighlighter::UpdateIconPosition()
    {
        m_iconPosition.x = m_renderPosition.x + m_params.m_iconOffset.x;
        m_iconPosition.y = m_renderPosition.y + m_params.m_iconOffset.y;
    }

    void InterfaceHighlighter::UpdateRenderPosition()
    {
        m_renderPosition.x = m_pHighlightedNode->GetDimensions().x;
        m_renderPosition.y = m_pHighlightedNode->GetDimensions().y;
    }

    void InterfaceHighlighter::RenderIcon(BB_Renderer* pRenderer) const
    {
        if (!m_pIconTexture)
        {
            BB_LOG(LogType::kError, "Attempted to draw renderIcon without setting an icon texture");
            return;
        }
        pRenderer->DrawTextureScreen(m_pIconTexture.get(), nullptr, &m_iconPosition);
    }

    void InterfaceHighlighter::RenderUnderline(BB_Renderer* pRenderer) const
    {
        BB_FRectangle dest{
            m_renderPosition.x, m_renderPosition.y + m_renderPosition.h,
            m_renderPosition.w, static_cast<float>(m_params.m_lineWidth)
        };
        pRenderer->DrawRectScreenFilled(dest, m_params.m_underlineColor);
    }

    void InterfaceHighlighter::RenderColor(BB_Renderer* pRenderer) const
    {
        pRenderer->DrawRectScreenFilled(m_renderPosition, m_params.m_highlightColor);
    }

    InterfaceHighlighter::InterfaceHighlighter(UserInterface* pAttachedInterface)
        : m_pAttachedInterface(pAttachedInterface)
    {
    }

    void InterfaceHighlighter::Render(BB_Renderer* pRenderer) const
    {
        if (m_params.m_mode & kModeIcon)
            RenderIcon(pRenderer);
        if (m_params.m_mode & kModeColored)
            RenderColor(pRenderer);
        if (m_params.m_mode & kModeUnderline)
            RenderUnderline(pRenderer);
    }

    void InterfaceHighlighter::SetTarget(InterfaceNode* pNode)
    {
        m_pHighlightedNode = pNode;
        m_renderPosition = pNode->GetDimensions();
    }

    void InterfaceHighlighter::SetMode(uint8_t mode)
    {
        m_params.m_mode = mode;
        if (mode & kModeIcon)
            UpdateIconPosition();
    }

    void InterfaceHighlighter::SetParmeters(Parameters params)
    {
        m_params = params;
        auto* pRenderer = BlackBoxManager::Get()->GetWindow()->GetRenderer();
        if (params.m_pIconFile)
            m_pIconTexture = ResourceManager::GetTexture(pRenderer, params.m_pIconFile);
    }

    ////////////////////////////////////////////////////////////////
    /// User Interface
    ////////////////////////////////////////////////////////////////

    void UserInterface::MoveCursor(Direction dir)
    {
        auto* pNode = m_pCursorPosition->GetAdjacentNode(dir);
        if (!pNode)
            return;
        m_pCursorPosition = pNode;
        pNode->OnTargeted();
        m_highlighter.SetTarget(pNode);
    }

    void UserInterface::SelectTargetedNode()
    {
        m_pCursorPosition->OnInteracted();
    }

    void UserInterface::SetupKeysForInputTarget()
    {
        // ensure we dont overlap keys
        m_pInputTarget->m_keyDown.ClearListeners();
        using enum Direction;
        
        m_pInputTarget->m_keyDown.RegisterListener(m_keycodes.m_up,   [this]() {MoveCursor(kUp); });
        m_pInputTarget->m_keyDown.RegisterListener(m_keycodes.m_down, [this]() {MoveCursor(kDown); });
        m_pInputTarget->m_keyDown.RegisterListener(m_keycodes.m_right,[this]() {MoveCursor(kRight); });
        m_pInputTarget->m_keyDown.RegisterListener(m_keycodes.m_left, [this]() {MoveCursor(kLeft); });
        m_pInputTarget->m_keyDown.RegisterListener(m_keycodes.m_select, [this]() {SelectTargetedNode(); });
    }

    UserInterface::UserInterface()
        : m_highlighter(this)
    {
        m_pRootNode = new InterfaceNode(nullptr, "_root");
    }

    UserInterface::~UserInterface()
    {
        assert(m_pRootNode);
        delete m_pRootNode;
    }

    InputManager::InputTarget* UserInterface::GetInputTarget()
    {
        if (m_pInputTarget)
            return m_pInputTarget;
        m_pInputTarget = new InputManager::InputTarget;
        SetupKeysForInputTarget();
        return m_pInputTarget;
    }

    void UserInterface::SetInterfaceKeys(const InterfaceKeys& keys)
    {
        m_keycodes = keys;
        SetupKeysForInputTarget();
    }

    void UserInterface::SetInterfaceKeys(KeyCode up, KeyCode down, KeyCode right, KeyCode left, KeyCode select)
    {
        m_keycodes.m_up = up;
        m_keycodes.m_down = down;
        m_keycodes.m_left = left;
        m_keycodes.m_right = right;
        m_keycodes.m_select = select;
        SetupKeysForInputTarget();
    }

    InterfaceNode* UserInterface::FindNode(const char* pName)
    {
        return m_pRootNode->GetChildNode(pName);
    }

    void UserInterface::SetCursorTarget(InterfaceNode* pNode)
    {
        m_pCursorPosition = pNode;
        pNode->OnTargeted();
        m_highlighter.SetTarget(pNode);
    }

    void UserInterface::AddToScreen()
    {
        m_managerId =  BlackBoxManager::Get()->m_pInterfaceManager->AddInterfaceToScreen(this);
    }

    void UserInterface::RemoveFromScreen()
    {
        if (m_managerId < 0)
            return;
        BlackBoxManager::Get()->m_pInterfaceManager->RemoveInterface(m_managerId);
        m_managerId = -1;
    }

    void UserInterface::Start()
    {
        m_pRootNode->Start();
    }

    void UserInterface::Update()
    {
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
        , m_nodeDimensions(dimensions)
        , m_nameHash(StringHash(pName))
    {

    }

    InterfaceNode::~InterfaceNode()
    {
        for (auto* pNode : m_childNodes)
            delete pNode;
    }

    void InterfaceNode::SetOffset(float x, float y)
    {
        m_nodeDimensions.x = x;
        m_nodeDimensions.y = y;
    }

    void InterfaceNode::SetSize(float w, float h)
    {
        m_nodeDimensions.w = w;
        m_nodeDimensions.h = h;
    }

    void InterfaceNode::Start()
    {
        StartThis();
        for (auto& node : m_childNodes)
            node->Start();
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
            node->Render(pRenderer, rootX + m_nodeDimensions.x, rootY + m_nodeDimensions.y);
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
}