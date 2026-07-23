#include <GLES3/gl3.h>
#include <EGL/egl.h>
#include <android/log.h>
#include <math.h>
#include <vector>
#include <string>

#define LOG_TAG "FFRenderer"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

struct Vec2 {
    float x, y;
};

struct Vec4 {
    float r, g, b, a;
};

struct Vertex {
    Vec2 position;
    Vec4 color;
};

class FFShader {
public:
    GLuint program;
    
    GLuint loadShader(GLenum type, const char* shaderSrc) {
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &shaderSrc, nullptr);
        glCompileShader(shader);
        return shader;
    }
    
    bool create(const char* vsSrc, const char* fsSrc) {
        GLuint vertexShader = loadShader(GL_VERTEX_SHADER, vsSrc);
        GLuint fragmentShader = loadShader(GL_FRAGMENT_SHADER, fsSrc);
        
        program = glCreateProgram();
        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);
        glLinkProgram(program);
        
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        
        return program != 0;
    }
    
    void use() {
        glUseProgram(program);
    }
    
    GLint getUniformLocation(const char* name) {
        return glGetUniformLocation(program, name);
    }
};

class FFRenderer {
private:
    FFShader shader;
    GLuint vao, vbo;
    GLint mvpMatrixLoc;
    
    const char* vertexShader = R"(
        #version 300 es
        layout(location = 0) in vec2 aPosition;
        layout(location = 1) in vec4 aColor;
        uniform mat4 uMVPMatrix;
        out vec4 vColor;
        void main() {
            gl_Position = uMVPMatrix * vec4(aPosition, 0.0, 1.0);
            vColor = aColor;
        }
    )";
    
    const char* fragmentShader = R"(
        #version 300 es
        precision mediump float;
        in vec4 vColor;
        out vec4 fragColor;
        void main() {
            fragColor = vColor;
        }
    )";
    
    std::vector<float> vertices;
    
    void addQuad(float x, float y, float w, float h, Vec4 color) {
        float x2 = x + w, y2 = y + h;
        Vertex v[] = {
            {{x, y}, color}, {{x2, y}, color}, {{x, y2}, color},
            {{x2, y}, color}, {{x2, y2}, color}, {{x, y2}, color}
        };
        for (auto& vtx : v) {
            vertices.push_back(vtx.position.x);
            vertices.push_back(vtx.position.y);
            vertices.push_back(vtx.color.r);
            vertices.push_back(vtx.color.g);
            vertices.push_back(vtx.color.b);
            vertices.push_back(vtx.color.a);
        }
    }
    
public:
    bool init(int width, int height) {
        LOGI("Initializing FF Renderer %dx%d", width, height);
        
        if (!shader.create(vertexShader, fragmentShader)) {
            LOGI("Shader creation failed!");
            return false;
        }
        
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        
        GLint posLoc = glGetAttribLocation(shader.program, "aPosition");
        GLint colLoc = glGetAttribLocation(shader.program, "aColor");
        
        glVertexAttribPointer(posLoc, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(posLoc);
        
        glVertexAttribPointer(colLoc, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(colLoc);
        
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        LOGI("Renderer initialized successfully");
        return true;
    }
    
    void render(float dt) {
        glClearColor(0.039f, 0.039f, 0.059f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        shader.use();
        
        float aspect = 1080.0f / 1920.0f;
        float mvp[] = {
            2.0f/aspect, 0, 0, 0,
            0, 2.0f, 0, 0,
            0, 0, 1, 0,
            -1, -1, 0, 1
        };
        
        GLint mvpLoc = glGetUniformLocation(shader.program, "uMVPMatrix");
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, mvp);
        
        vertices.clear();
        
        drawUI();
        
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
        
        glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 6);
    }
    
    void drawUI() {
        Vec4 bgDark = {0.039f, 0.039f, 0.059f, 1.0f};
        Vec4 cardBg = {0.071f, 0.071f, 0.102f, 1.0f};
        Vec4 green = {0.0f, 1.0f, 0.533f, 1.0f};
        Vec4 greenDim = {0.0f, 0.8f, 0.42f, 1.0f};
        Vec4 greenSubtle = {0.0f, 1.0f, 0.533f, 0.1f};
        Vec4 orange = {1.0f, 0.42f, 0.21f, 1.0f};
        Vec4 cyan = {0.0f, 0.83f, 1.0f, 1.0f};
        Vec4 magenta = {1.0f, 0.0f, 1.0f, 1.0f};
        Vec4 textSec = {0.54f, 0.54f, 0.6f, 1.0f};
        Vec4 white = {1.0f, 1.0f, 1.0f, 1.0f};
        
        float margin = 40.0f;
        float cardW = 480.0f;
        float cardH = 180.0f;
        
        for (int row = 0; row < 2; row++) {
            for (int col = 0; col < 2; col++) {
                float x = margin + col * (cardW + 30);
                float y = 400 + row * (cardH + 20);
                addQuad(x, y, cardW, cardH, cardBg);
            }
        }
    }
    
    void shutdown() {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
        LOGI("Renderer shutdown complete");
    }
};

static FFRenderer* g_renderer = nullptr;

extern "C" {
    JNIEXPORT void JNICALL
    Java_com_ffpanel_native_Renderer_init(JNIEnv *env, jclass, jint width, jint height) {
        if (!g_renderer) {
            g_renderer = new FFRenderer();
            g_renderer->init(width, height);
        }
    }
    
    JNIEXPORT void JNICALL
    Java_com_ffpanel_native_Renderer_render(JNIEnv *env, jclass, jfloat dt) {
        if (g_renderer) {
            g_renderer->render(dt);
        }
    }
    
    JNIEXPORT void JNICALL
    Java_com_ffpanel_native_Renderer_shutdown(JNIEnv *env, jclass) {
        if (g_renderer) {
            g_renderer->shutdown();
            delete g_renderer;
            g_renderer = nullptr;
        }
    }
}
