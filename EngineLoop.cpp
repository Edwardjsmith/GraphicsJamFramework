#include "EngineLoop.h"

ProcessState EngineLoop::Init()
{
    if (SDLInit() != ProcessState::OKAY)
    {
        return ProcessState::NOT_OKAY;
    }

    if (OpenGLInit("Shaders/SoftwareRasterizer.shader") != ProcessState::OKAY)
    {
        return ProcessState::NOT_OKAY;
    }

    return ProcessState::OKAY;
}

ProcessState EngineLoop::Draw()
{
    if (m_ComputeShader == nullptr)
    {
        return ProcessState::NOT_OKAY;
    }

    m_ComputeShader->Use();
    m_ComputeShader->SetFloat("delta", glfwGetTime());

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_pixelBuffer);

    glDispatchCompute(1, 1, 1);

    glMemoryBarrier(GL_ALL_BARRIER_BITS);

    glBindBuffer(GL_ARRAY_BUFFER, m_pixelBuffer);
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, (SCREEN_WIDTH * SCREEN_HEIGHT) * sizeof(glm::vec4), (GLvoid*)m_pixelData);

    if (m_SDLWindow && m_mainSurface)
    {
        SDL_LockSurface(m_mainSurface);

        Uint32* pixels = (Uint32*)m_mainSurface->pixels;

        for (int i = 0; i < m_mainSurface->h; ++i)
        {
            for (int j = 0; j < m_mainSurface->w; ++j)
            {
                const glm::vec4& col = m_pixelData[m_mainSurface->w * i + j];
                pixels[m_mainSurface->w * i + j] = SDL_MapRGB(m_mainSurface->format, col.r, col.g, col.b);
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

ProcessState EngineLoop::SDLInit()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("Unable to init SDL video, Error code: %s\n", SDL_GetError());
        return ProcessState::NOT_OKAY;
    }
    else
    {
        m_SDLWindow = SDL_CreateWindow("Eds Software Rasterizer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

        if (m_SDLWindow == nullptr)
        {
            printf("Could not create SDL window %s\n", SDL_GetError());
            return ProcessState::NOT_OKAY;
        }
        else
        {
            m_mainSurface = SDL_GetWindowSurface(m_SDLWindow);
        }
    }

    return ProcessState::OKAY;
}

void EngineLoop::SDLCleanup()
{
    m_mainSurface = nullptr;
    SDL_DestroyWindow(m_SDLWindow);
    m_SDLWindow = nullptr;

    SDL_Quit();
}

ProcessState EngineLoop::OpenGLInit(const char* computePath)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1, 1, "Eds Software Rasterizer", NULL, NULL);

    if (window == nullptr)
    {
        OpenGLCleanup();
        printf("Unable to create GLFW window\n");
        return ProcessState::NOT_OKAY;
    }
    glfwMakeContextCurrent(window);

    m_OpenGLWindow = window;

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("Failed to initialize GLAD\n");
        return ProcessState::NOT_OKAY;
    }

    m_ComputeShader = std::make_unique<ComputeShader>(computePath);

    glGenBuffers(1, &m_pixelBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_pixelBuffer);
    glBufferData(GL_ARRAY_BUFFER, (SCREEN_WIDTH * SCREEN_HEIGHT) * sizeof(glm::vec4), 0, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return ProcessState::OKAY;
}

void EngineLoop::OpenGLCleanup()
{
    glfwDestroyWindow(m_OpenGLWindow);
    glfwTerminate();

    m_ComputeShader.reset();
}
