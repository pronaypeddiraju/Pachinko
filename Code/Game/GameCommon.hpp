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

constexpr float BOX_MIN_WIDTH = 2.5f;
constexpr float BOX_MAX_WIDTH = 10.0f;

constexpr float DISC_MIN_RADIUS = 2.5f;
constexpr float DISC_MAX_RADIUS = 5.0f;

constexpr float MAX_ZOOM_STEPS = 30.f;

constexpr float CLIENT_ASPECT = 2.0f; // We are requesting a 1:1 aspect (square) window area

extern RenderContext* g_renderContext;
extern InputSystem* g_inputSystem;
extern AudioSystem* g_audio;
extern RandomNumberGenerator* g_randomNumGen;