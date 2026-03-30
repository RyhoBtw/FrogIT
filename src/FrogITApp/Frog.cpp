#include "Frog.hpp"

#include "Constants.hpp"
#include "ResourceManager.hpp"

#include <chrono>
#include <cmath>

namespace {

constexpr float HOP_DISTANCE_MIN = 60.f;
constexpr float HOP_DISTANCE_MAX = 200.f;
constexpr float HOP_DURATION_MIN = 0.25f;
constexpr float HOP_DURATION_MAX = 0.45f;
constexpr float HOP_HEIGHT_MIN = 25.f;
constexpr float HOP_HEIGHT_MAX = 60.f;
constexpr float PAUSE_MIN = 0.8f;
constexpr float PAUSE_MAX = 3.5f;
constexpr float SPEECH_DISPLAY_TIME = 4.0f;
constexpr float PI = 3.14159265f;

const std::vector<std::string>& getPhrasesForHour(int hour)
{
    static const std::vector<std::string> morning = {
        "Guten Morgen!\nSchon wach?",
        "Quak! Fruehstueck\nvergessen?",
        "Kaffee gefaellig?\n*quak*",
        "Noch muede?\nIch auch!",
        "Der fruehe Frosch\nfaengt die Fliege!"
    };
    static const std::vector<std::string> midday = {
        "Mittagspause!\n*quak quak*",
        "Hunger? Ich hab\nnoch Fliegen!",
        "Schon Mittag?\nWie die Zeit\nvergeht!",
        "Quak! Nicht\nvergessen zu essen!",
        "Pause machen\nist wichtig! *quak*"
    };
    static const std::vector<std::string> afternoon = {
        "Na, produktiv\nheute? *quak*",
        "Ich beobachte\ndich... quak!",
        "Noch ein\nbisschen arbeiten!",
        "Quak! Fast\nFeierabend!",
        "Frosch-Tipp:\nMal durchatmen!"
    };
    static const std::vector<std::string> evening = {
        "Feierabend!\n*quak quak*",
        "Ab ins Bett!\nQuaaak!",
        "Noch wach?\nIch auch... quak",
        "Gute Nacht!\n*leises quak*",
        "Schlaf gut!\nTraeume von\nFroeschen!"
    };

    if (hour >= 5 && hour < 11) return morning;
    if (hour >= 11 && hour < 14) return midday;
    if (hour >= 14 && hour < 18) return afternoon;
    return evening;
}

} // namespace

Frog::Frog(const std::string& textureOpen, const std::string& textureClosed)
    : m_sprite(ResourceManager::getTexture(textureOpen))
    , m_texOpen(ResourceManager::getTexture(textureOpen))
    , m_texClosed(ResourceManager::getTexture(textureClosed))
{
    float textureHeight = static_cast<float>(m_sprite.getTexture().getSize().y);
    m_scale = FROG_TARGET_HEIGHT / textureHeight;
    m_sprite.setScale({ m_scale, m_scale });
}

void Frog::randomizePosition(sf::Vector2u desktopSize)
{
    auto scaled = getScaledSize();
    std::uniform_real_distribution<float> distX(0.f, static_cast<float>(desktopSize.x - scaled.x));
    std::uniform_real_distribution<float> distY(0.f, static_cast<float>(desktopSize.y - scaled.y));
    m_position = { distX(m_rng), distY(m_rng) };
    m_hopStart = m_position;
    m_hopEnd = m_position;
}

void Frog::pickNextHop(sf::Vector2u desktopSize)
{
    auto scaled = getScaledSize();
    float maxX = static_cast<float>(desktopSize.x - scaled.x);
    float maxY = static_cast<float>(desktopSize.y - scaled.y);

    std::uniform_real_distribution<float> hopDist(HOP_DISTANCE_MIN, HOP_DISTANCE_MAX);
    std::uniform_real_distribution<float> angleDist(0.f, 2.f * PI);
    float dist = hopDist(m_rng);
    float angle = angleDist(m_rng);

    m_hopStart = m_position;
    m_hopEnd = {
        std::clamp(m_position.x + dist * std::cos(angle), 0.f, maxX),
        std::clamp(m_position.y + dist * std::sin(angle), 0.f, maxY)
    };

    std::uniform_real_distribution<float> durDist(HOP_DURATION_MIN, HOP_DURATION_MAX);
    std::uniform_real_distribution<float> heightDist(HOP_HEIGHT_MIN, HOP_HEIGHT_MAX);
    m_hopDuration = durDist(m_rng);
    m_hopHeight = heightDist(m_rng);
    m_hopProgress = 0.f;
    m_isHopping = true;

    // Set facing direction
    float dx = m_hopEnd.x - m_hopStart.x;
    auto texSize = m_texOpen.getSize();
    if (dx < 0 && m_facingRight) {
        m_sprite.setScale({ -m_scale, m_scale });
        m_sprite.setPosition({ static_cast<float>(texSize.x) * m_scale, 0.f });
        m_facingRight = false;
        m_directionChanged = true;
    } else if (dx >= 0 && !m_facingRight) {
        m_sprite.setScale({ m_scale, m_scale });
        m_sprite.setPosition({ 0.f, 0.f });
        m_facingRight = true;
        m_directionChanged = true;
    }

    // Open mouth while hopping
    m_sprite.setTexture(m_texOpen);
}

void Frog::update(float deltaTime, sf::Vector2u desktopSize)
{
    if (!m_window.isOpen()) return;

    updateSpeechBubble(deltaTime);

    if (m_isHopping) {
        m_hopProgress += deltaTime / m_hopDuration;
        if (m_hopProgress >= 1.f) {
            m_hopProgress = 1.f;
            m_isHopping = false;
            m_position = m_hopEnd;

            // Close mouth when landing
            m_sprite.setTexture(m_texClosed);

            m_hopsRemaining--;
            if (m_hopsRemaining <= 0) {
                std::uniform_real_distribution<float> pauseDist(PAUSE_MIN, PAUSE_MAX);
                m_pauseTimer = pauseDist(m_rng);
            } else {
                // Short pause between chained hops
                std::uniform_real_distribution<float> microPause(0.1f, 0.3f);
                m_pauseTimer = microPause(m_rng);
            }
        } else {
            // Parabolic hop arc
            float t = m_hopProgress;
            float arcY = -4.f * m_hopHeight * t * (1.f - t); // parabola peaking at t=0.5

            sf::Vector2f basePos = m_hopStart + (m_hopEnd - m_hopStart) * t;
            m_position = { basePos.x, basePos.y + arcY };
        }

        m_window.setPosition(sf::Vector2i(
            static_cast<int>(m_position.x),
            static_cast<int>(m_position.y)));
        return;
    }

    // Pause between hops
    if (m_pauseTimer > 0.f) {
        m_pauseTimer -= deltaTime;
        return;
    }

    // Start a new sequence of hops
    std::uniform_int_distribution<int> hopCountDist(1, 3);
    m_hopsRemaining = hopCountDist(m_rng);
    pickNextHop(desktopSize);
}

void Frog::handleClick()
{
    auto now = std::chrono::system_clock::now();
    auto timeT = std::chrono::system_clock::to_time_t(now);
    auto* localTime = std::localtime(&timeT);
    int hour = localTime->tm_hour;

    const auto& phrases = getPhrasesForHour(hour);
    std::uniform_int_distribution<size_t> dist(0, phrases.size() - 1);
    m_speechText = phrases[dist(m_rng)];
    m_showSpeech = true;
    m_speechTimer = SPEECH_DISPLAY_TIME;
}

bool Frog::hasSpeechBubble() const { return m_showSpeech; }

const std::string& Frog::getSpeechText() const { return m_speechText; }

sf::Vector2f Frog::getSpeechBubblePosition() const
{
    auto scaled = getScaledSize();
    return { m_position.x + static_cast<float>(scaled.x) / 2.f,
             m_position.y - 10.f };
}

void Frog::updateSpeechBubble(float deltaTime)
{
    if (!m_showSpeech) return;
    m_speechTimer -= deltaTime;
    if (m_speechTimer <= 0.f) {
        m_showSpeech = false;
    }
}

sf::Sprite& Frog::getSprite() { return m_sprite; }

sf::RenderWindow& Frog::getWindow() { return m_window; }

sf::Vector2u Frog::getScaledSize() const
{
    auto texSize = m_texOpen.getSize();
    return sf::Vector2u(
        static_cast<unsigned int>(static_cast<float>(texSize.x) * m_scale),
        static_cast<unsigned int>(static_cast<float>(texSize.y) * m_scale));
}

float Frog::getScale() const { return m_scale; }

bool Frog::isFacingRight() const { return m_facingRight; }

bool Frog::consumeDirectionChanged()
{
    if (m_directionChanged) {
        m_directionChanged = false;
        return true;
    }
    return false;
}
