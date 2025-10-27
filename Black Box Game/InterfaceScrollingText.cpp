#include "InterfaceScrollingText.h"

static constexpr float kPadding = 4.f;
static constexpr int kMaxCharsPerLine = 19;
static constexpr int kMaxVisibleLines = 7;

BlackBoxEngine::ScrollingTextBox::ScrollingTextBox(
    InterfaceNode* pParent, const char* pName,
    BB_FRectangle rect, const Params& params )
    : InterfaceNode( pParent, pName, rect ), m_params( params )
{
    InterfaceText::Paremeters textParams{
        .pFontFile = m_params.fontFile,
        .pText = "test",
        .textSize = m_params.textSize,
        .color = m_params.color
    };

    m_textNode = MakeChildNode<InterfaceText>( "scrolling_text", {0,0,0,0}, textParams);
}

void BlackBoxEngine::ScrollingTextBox::SetText( const std::string& text, std::function<void()> onComplete )
{
    if ( m_fullText.empty() )
        m_fullText = text;
    else
        m_fullText += text;

    //m_params.onComplete = onComplete;


    m_params.onComplete = [onComplete]() {
        // Resume input after text completes
        BlackBoxManager::Get()->m_pInputManager->ResumeInput();
        if (onComplete)
            onComplete();
        };

    // Lock input during scrolling
    BlackBoxManager::Get()->m_pInputManager->StopAllInput();

    m_lines.push_back( "" );
    m_isComplete = false;
}

void BlackBoxEngine::ScrollingTextBox::UpdateThis()
{
    if ( m_isComplete )
        return;

    float dt = static_cast<float>(BlackBoxManager::Get()->GetDeltaTime());

    // --- Reveal characters over time ---
    m_accumulatedTime += dt * m_params.charsPerSecond;
    int charsToReveal = static_cast<int>(m_accumulatedTime);
    if ( charsToReveal > 0 )
    {
        m_accumulatedTime -= charsToReveal;
        RevealCharacters( charsToReveal );
    }
}

void BlackBoxEngine::ScrollingTextBox::RevealCharacters( int count )
{
    for ( int i = 0; i < count && m_currentChar < m_fullText.size(); ++i )
    {
        char c = m_fullText[m_currentChar++];

        if ( c == '\n' || m_lines.back().size() >= kMaxCharsPerLine )
            m_lines.push_back( "" );

        if ( c != '\n' )
            m_lines.back() += c;
    }

    UpdateDisplay();

    if ( m_currentChar >= m_fullText.size() )
    {
        m_isComplete = true;
        if ( m_params.onComplete )
            m_params.onComplete();
    }
}

void BlackBoxEngine::ScrollingTextBox::UpdateDisplay()
{
    if ( !m_textNode || !m_textNode->GetText() )
        return;

    while ( m_lines.size() > kMaxVisibleLines )
        m_lines.erase( m_lines.begin() ); // remove oldest line

    // Build display string with horizontal centering
    std::string display;
    for ( auto& line : m_lines )
        display += line + "\n";

    if ( !display.empty() && display.back() == '\n' )
        display.pop_back();

    m_textNode->GetText()->SetString( display.c_str(), display.size() );
}