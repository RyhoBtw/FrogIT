#pragma once

// UI constants
constexpr int FRAME_TOP_BORDER_Y = 116;
constexpr int FRAME_LEFT_BORDER_X = 64;
constexpr int FRAME_INNER_X = 311;
constexpr int FRAME_INNER_Y = 441;
constexpr unsigned int FRAMERATE_LIMIT = 60U;
constexpr float UI_CURSOR_OFFSET_Y = 24.0F;
constexpr float UI_BUTTON_WIDTH_DEFAULT = 20.0F;
constexpr float UI_DISTANCE_MULT_BUTTON = 2.5F;
constexpr float FROG_TARGET_HEIGHT = 150.0F;

// SoundScape constants
constexpr float SOUND_VOLUME_MIN = 0.0F;
constexpr float SOUND_VOLUME_MAX = 100.0F;// SFML volume range
constexpr float SOUND_VOLUME_DEFAULT = 80.0F;

constexpr float AMBIENT_FREQ_MIN = 0.0F;// triggers per minute (0 = never)
constexpr float AMBIENT_FREQ_MAX = 10.0F;
constexpr float AMBIENT_FREQ_DEFAULT = 3.0F;

constexpr float AMBIENT_INTERVAL_BASE = 60.0F;// mean seconds between plays at frequency == 1 (60s / triggers-per-minute)
constexpr float AMBIENT_INTERVAL_JITTER = 0.4F;// +/- 40% around the mean interval
constexpr float AMBIENT_FREQ_EPSILON = 0.001F;// below this frequency, never fire

constexpr int SOUNDSCAPE_NAME_MAX = 64;// InputText buffer size

inline constexpr const char* SOUNDSCAPE_CONFIG_FILE = "soundscapes.cfg";
inline constexpr const char* SOUND_DIR_LOOPING = "assets/sounds/looping";
inline constexpr const char* SOUND_DIR_AMBIENT = "assets/sounds/ambient";
inline constexpr const char* SOUND_KEY_PREFIX_LOOPING = "looping/";
inline constexpr const char* SOUND_KEY_PREFIX_AMBIENT = "ambient/";

// Frog management constants
constexpr int FROG_COUNT_MAX = 200;
constexpr int FROG_COUNT_MIN = 1;

// Frog hop constants
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