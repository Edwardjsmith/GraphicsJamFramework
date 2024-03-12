#include "EngineLoop.h"

ProcessState EngineLoop::Init()
{
    std::string shaderSrc;
#if RAYTRACER 1
    shaderSrc = "Shaders/SoftwareRaytracer.shader";
#else
    shaderSrc = "Shaders/SoftwareRasterizer.shader";
#endif

    if (SDLInit(shaderSrc.c_str()) != ProcessState::OKAY)
    {
        return ProcessState::NOT_OKAY;
    }

    return ProcessState::OKAY;
}



ProcessState EngineLoop::Draw()
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (m_Camera == nullptr)
    {
        printf("Camera was null!");
        return ProcessState::NOT_OKAY;
    }

    m_Camera->Draw((float)0);

    if (m_SDLWindow && m_mainSurface)
    {
        SDL_LockSurface(m_mainSurface);

        Uint32* const pixels = (Uint32*)m_mainSurface->pixels;

        void* const pixelData = m_Camera->GetPixelData();

        for (int i = 0; i < SCREEN_WIDTH; ++i)
        {
            for (int j = 0; j < SCREEN_HEIGHT; ++j)
            {
                PixelData& data = static_cast<PixelData*>(pixelData)[i + (j * SCREEN_WIDTH)];

                const Uint8 r = data.color.r;
                const Uint8 g = data.color.g;
                const Uint8 b = data.color.b;

                pixels[i + (j * SCREEN_WIDTH)] = SDL_MapRGB(m_mainSurface->format, r, g, b);
            }
        }

        SDL_UnlockSurface(m_mainSurface);
        SDL_UpdateWindowSurface(m_SDLWindow);
    }
    else
    {
        printf("Window or surface was null!");
        return ProcessState::NOT_OKAY;
    }

    return ProcessState::OKAY;
}

void EngineLoop::HandleMouseMovement(float& offsetX, float& offsetY)
{
    int xPos, yPos;
    SDL_GetMouseState(&xPos, &yPos);

    offsetX = (xPos - m_lastMouseX) * m_sensitivity;
    offsetY = (m_lastMouseY - yPos) * m_sensitivity; // reversed since y-coordinates range from bottom to top

    m_lastMouseX = xPos;
    m_lastMouseY = yPos;
}

ProcessState EngineLoop::Update(float delta)
{
    SDL_PollEvent(&SDLEvent);

    if (m_Camera)
    {
        m_Camera->Update(delta);

        const glm::vec3& cameraPos = m_Camera->GetPosition();
        const glm::vec3& cameraUp = m_Camera->GetCameraUp();
        const glm::vec3& cameraForward = m_Camera->GetCameraForward();
        const glm::vec3& cameraRight = m_Camera->GetCameraRight();
        const float cameraSpeed = m_Camera->GetCameraSpeed() * delta;

        glm::vec3 newPos = glm::vec3(0);

        switch (SDLEvent.type)
        {
            case SDL_KEYDOWN:
            {
                switch (SDLEvent.key.keysym.sym)
                {
                    case SDLK_LEFT:
                    {
                        newPos = cameraPos - glm::normalize(glm::cross(cameraForward, cameraUp)) * cameraSpeed;
                        m_Camera->SetPosition(newPos);
                        break;
                    }
                    case SDLK_RIGHT:
                    {
                        newPos = cameraPos + glm::normalize(glm::cross(cameraForward, cameraUp)) * cameraSpeed;
                        m_Camera->SetPosition(newPos);
                        break;
                    }
                    case SDLK_UP:
                    {
                        newPos = cameraPos + (cameraForward * cameraSpeed);
                        m_Camera->SetPosition(newPos);
                        break;
                    }
                    case SDLK_DOWN:
                    {
                        newPos = cameraPos - (cameraForward * cameraSpeed);
                        m_Camera->SetPosition(newPos);
                        break;
                    }
                    default: break;

                }
                break;
            }
            case SDL_KEYUP:
            {
                switch (SDLEvent.key.keysym.sym)
                {
                case SDLK_LEFT:
                {
                    break;
                }
                case SDLK_RIGHT:
                {
                    break;
                }
                case SDLK_UP:
                {
                    break;
                }
                case SDLK_DOWN:
                {
                    break;
                }
                default: break;

                }
                break;
            }
        }

        float offsetX, offsetY;
        HandleMouseMovement(offsetX, offsetY);

        m_Camera->SetYaw(m_Camera->GetYaw() + offsetX);
        m_Camera->SetPitch(m_Camera->GetPitch() + offsetY);
    }


    return ProcessState::OKAY;
}

void EngineLoop::Cleanup()
{
    SDLCleanup();
}

bool EngineLoop::ShutdownPending() const
{
    return SDLEvent.type == SDL_QUIT;
}

ProcessState EngineLoop::SDLInit(const char* computePath)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("Unable to init SDL video, Error code: %s\n", SDL_GetError());
        return ProcessState::NOT_OKAY;
    }
    else
    {
        m_SDLWindow = SDL_CreateWindow("Eds Graphics Jam Sandpit", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);

        if (m_SDLWindow == nullptr)
        {
            printf("Could not create SDL window %s\n", SDL_GetError());
            return ProcessState::NOT_OKAY;
        }
        else
        {
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);

            context = SDL_GL_CreateContext(m_SDLWindow);

            if (context == nullptr)
            {
                return ProcessState::NOT_OKAY;
            }

            m_mainSurface = SDL_GetWindowSurface(m_SDLWindow);
        }
    }

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
    {
        printf("Failed to initialize GLAD\n");
        return ProcessState::NOT_OKAY;
    }


    glm::vec3 pos = glm::vec3(0.0f, 0.0f, -3.0f);
    glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

    m_Camera = std::make_unique<Camera>(pos, target, worldUp, 5.0f, 0.0f, 0.0f);

    if (m_Camera->InitShader(computePath) != ProcessState::OKAY)
    {
        return ProcessState::NOT_OKAY;
    }

    SDL_SetRelativeMouseMode(SDL_TRUE);

    return ProcessState::OKAY;
}

void EngineLoop::SDLCleanup()
{
    m_mainSurface = nullptr;
    SDL_DestroyWindow(m_SDLWindow);
    m_SDLWindow = nullptr;
    SDL_GL_DeleteContext(context);

    SDL_Quit();
}