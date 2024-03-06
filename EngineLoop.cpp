#include "EngineLoop.h"

ProcessState EngineLoop::Init()
{
    if (SDLInit("Shaders/SoftwareRaytracer.shader") != ProcessState::OKAY)
    {
        return ProcessState::NOT_OKAY;
    }

    return ProcessState::OKAY;
}



ProcessState EngineLoop::Draw()
{
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

                const float r = data.color.r;
                const float g = data.color.g;
                const float b = data.color.b;

                //const float coordX = data.coords.x;
                //const float coordY = data.coords.y;

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

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return ProcessState::OKAY;
}

ProcessState EngineLoop::Update()
{
    SDL_PollEvent(&SDLEvent);
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
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

            context = SDL_GL_CreateContext(m_SDLWindow);

            if (context == nullptr)
            {
                SDLCleanup();
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


    glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

    m_Camera = std::make_unique<Camera>(pos, target, worldUp, 5.0f, 0.0f, -90.0f);
    m_Camera->InitShader(computePath);

    return ProcessState::OKAY;
}

void EngineLoop::SDLCleanup()
{
    m_mainSurface = nullptr;
    SDL_DestroyWindow(m_SDLWindow);
    m_SDLWindow = nullptr;

    SDL_Quit();
}