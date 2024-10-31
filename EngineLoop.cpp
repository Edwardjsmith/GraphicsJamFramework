#include "EngineLoop.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

static const char* GAssetPath = "Assets/Teapot/teapot.obj";

static void APIENTRY OpenglCallbackFunction(
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam)
{
    fprintf(stderr, "%s\n", message);

    if (severity == GL_DEBUG_SEVERITY_HIGH)
    {
        fprintf(stderr, "Aborting...\n");
        abort();
    }
}

std::string shaderSrc0;
std::string shaderSrc1;

ProcessState EngineLoop::Init()
{
#if RAYTRACER 1
    shaderSrc = GShaderPath + "SoftwareRaytracer" + GShaderExt;
#else
    shaderSrc1 = GShaderPath + "SoftwareRasterizer" + GShaderExt;
    shaderSrc0 = GShaderPath + "TriangleFilter" + GShaderExt;

    if (LoadAssets(GAssetPath) != ProcessState::OKAY)
    {
        return ProcessState::NOT_OKAY;
    }
#endif

    if (SDLInit() != ProcessState::OKAY)
    {
        return ProcessState::NOT_OKAY;
    }

    //// Enable the debug callback
    //glEnable(GL_DEBUG_OUTPUT);
    //glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    //glDebugMessageCallback(OpenglCallbackFunction, nullptr);
    //glDebugMessageControl(
    //    GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, true
    //);
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

    m_Camera->Draw();

    if (m_SDLWindow && m_mainSurface)
    {
        SDL_LockSurface(m_mainSurface);

        Uint32* const pixels = (Uint32*)m_mainSurface->pixels;

        const void* const pixelData = m_Camera->GetPixelData();

        for (int i = 0; i < SCREEN_WIDTH; ++i)
        {
            for (int j = 0; j < SCREEN_HEIGHT; ++j)
            {
                const int index = i + (j * SCREEN_WIDTH);
                const PixelData& data = static_cast<const PixelData*>(pixelData)[index];

                if (data.color == glm::vec4(0))
                {
                    float colourX = (i / (SCREEN_WIDTH * 2.0f)) * 255.0f;
                    float colourY = (j / (SCREEN_HEIGHT * 2.0f)) * 255.0f;

                    pixels[index] = SDL_MapRGB(m_mainSurface->format, colourX, colourY, 0.0f);
                }
                else
                {
                    const Uint8 r = static_cast<Uint8>(data.color.r);
                    const Uint8 g = static_cast<Uint8>(data.color.g);
                    const Uint8 b = static_cast<Uint8>(data.color.b);

                    pixels[index] = SDL_MapRGB(m_mainSurface->format, r, g, b);
                }
            }
        }

        SDL_UnlockSurface(m_mainSurface);
        SDL_UpdateWindowSurface(m_SDLWindow);

        m_Camera->ResetPixelData();
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

    m_lastMouseX = static_cast<float>(xPos);
    m_lastMouseY = static_cast<float>(yPos);
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

        glm::vec3 newPos = cameraPos;

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

        float offsetX = 0, offsetY = 0;
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

ProcessState EngineLoop::SDLInit()
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

    glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 target = glm::vec3(0.0f, 0.0f, -1.0f);

    m_Camera = std::make_unique<Camera>(pos, target, GWorldUp, 5.0f, 0.0f, 0.0f);

    if (m_Camera->InitTriangleFilterShader(shaderSrc0.c_str()) != ProcessState::OKAY)
    {
        return ProcessState::NOT_OKAY;
    }

    if (m_Camera->InitRasterShader(shaderSrc1.c_str()) != ProcessState::OKAY)
    {
        return ProcessState::NOT_OKAY;
    }

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

#if RAYTRACER 1
#else
extern std::vector<VertexInput> GVertexData;
extern std::vector<uint32_t> GIndexData;

extern int GDispatchX, GDispatchY;

ProcessState EngineLoop::LoadAssets(const char* pathName)
{
    tinyobj::attrib_t attribs;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string err = "";

    bool ret = tinyobj::LoadObj(&attribs, &shapes, &materials, nullptr, &err, GAssetPath, "../Assets/", true /*triangulate*/, true /*default_vcols_fallback*/);
    if (ret)
    {
        // Process vertices
        {
            // POD of indices of vertex data provided by tinyobjloader, used to map unique vertex data to indexed primitive
            struct IndexedPrimitive
            {
                uint32_t PosIdx;
                uint32_t NormalIdx;
                uint32_t UVIdx;

                bool operator<(const IndexedPrimitive& other) const
                {
                    return memcmp(this, &other, sizeof(IndexedPrimitive)) > 0;
                }
            };

            std::map<IndexedPrimitive, uint32_t> indexedPrims;
            for (size_t s = 0; s < shapes.size(); s++)
            {
                const tinyobj::shape_t& shape = shapes[s];

                uint32_t meshIdxBase = GIndexData.size();
                for (size_t i = 0; i < shape.mesh.indices.size(); i++)
                {
                    auto index = shape.mesh.indices[i];

                    // Fetch indices to construct an IndexedPrimitive to first look up existing unique vertices
                    int vtxIdx = index.vertex_index;
                    assert(vtxIdx != -1);

                    bool hasNormals = index.normal_index != -1;
                    bool hasUV = index.texcoord_index != -1;

                    int normalIdx = index.normal_index;
                    int uvIdx = index.texcoord_index;

                    IndexedPrimitive prim;
                    prim.PosIdx = vtxIdx;
                    prim.NormalIdx = hasNormals ? normalIdx : UINT32_MAX;
                    prim.UVIdx = hasUV ? uvIdx : UINT32_MAX;

                    auto res = indexedPrims.find(prim);
                    if (res != indexedPrims.end())
                    {
                        // Vertex is already defined in terms of POS/NORMAL/UV indices, just append index data to index buffer
                        GIndexData.push_back(res->second);
                    }
                    else
                    {
                        // New unique vertex found, get vertex data and append it to vertex buffer and update indexed primitives
                        auto newIdx = GVertexData.size();
                        indexedPrims[prim] = newIdx;
                        GIndexData.push_back(newIdx);

                        auto vx = attribs.vertices[3 * index.vertex_index];
                        auto vy = attribs.vertices[3 * index.vertex_index + 1];
                        auto vz = attribs.vertices[3 * index.vertex_index + 2];

                        glm::vec3 pos(vx, vy, vz);

                        glm::vec3 normal(0.f);
                        if (hasNormals)
                        {
                            auto nx = attribs.normals[3 * index.normal_index];
                            auto ny = attribs.normals[3 * index.normal_index + 1];
                            auto nz = attribs.normals[3 * index.normal_index + 2];

                            normal.x = nx;
                            normal.y = ny;
                            normal.z = nz;
                        }

                        glm::vec2 uv(0.f);
                        if (hasUV)
                        {
                            auto ux = attribs.texcoords[2 * index.texcoord_index];
                            auto uy = 1.f - attribs.texcoords[2 * index.texcoord_index + 1];

                            uv.s = glm::abs(ux);
                            uv.t = glm::abs(uy);
                        }

                        VertexInput uniqueVertex = { pos, normal, uv };
                        GVertexData.push_back(uniqueVertex);
                    }
                }
            }
        }

        GDispatchX = SCREEN_WIDTH / 16;//((GIndexData.size()) / 3);
        GDispatchY = SCREEN_HEIGHT / 16;

        return ProcessState::OKAY;
    }
    else
    {
        printf("ERROR: %s\n", err.c_str());
        return ProcessState::NOT_OKAY;
    }
}

#endif
