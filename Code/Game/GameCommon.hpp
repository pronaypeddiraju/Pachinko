#pragma once
#include "Engine/Commons/EngineCommon.hpp"

//------------------------------------------------------------------------------------------------------------------------------

class RenderContext;
class InputSystem;
class AudioSystem;
class RandomNumberGenerator;

//------------------------------------------------------------------------------------------------------------------------------

constexpr float WORLD_WIDTH = 200.f;
constexpr float WORLD_HEIGHT = 100.f;
constexpr float WORLD_CENTER_X = WORLD_WIDTH / 2.f;
constexpr float WORLD_CENTER_Y = WORLD_HEIGHT / 2.f;
constexpr float SCREEN_ASPECT = 16.f/9.f;

constexpr float CAMERA_SHAKE_REDUCTION_PER_SECOND = 1.f;
constexpr float MAX_SHAKE = 2.0f;

constexpr float DEVCONSOLE_LINE_HEIGHT = 2.0f;

constexpr float BOX_MIN_WIDTH = 0.25f;
constexpr float BOX_MAX_WIDTH = 2.0f;

constexpr float DISC_MIN_RADIUS = 0.25f;
constexpr float DISC_MAX_RADIUS = 2.0f;

extern RenderContext* g_renderContext;
extern InputSystem* g_inputSystem;
extern AudioSystem* g_audio;
extern RandomNumberGenerator* g_randomNumGen;