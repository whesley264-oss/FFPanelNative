#include <jni.h>
#include <string>
#include <android/log.h>
#include <vector>
#include <cmath>

#define LOG_TAG "FFPanelNative"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// ============ ESP TYPES ============
#define ESP_BOX       0
#define ESP_LINE      1
#define ESP_SKELETON  2
#define ESP_NAME      3
#define ESP_GLOW      4
#define ESP_CORNERED  5

// ============ ENEMY STRUCT ============
struct Enemy {
    float x, y, z;           // Posição 3D
    float screenX, screenY; // Posição na tela (2D)
    float width, height;    // Dimensões na tela
    float distance;          // Distância do jogador
    float health;            // Vida (0-100)
    bool visible;            // Se está visível
    bool isEnemy;            // Se é inimigo
    char name[32];           // Nome do jogador
};

struct Feature {
    const char* id;
    const char* name;
    const char* description;
    bool enabled;
    float intensity;
};

// ESP Individual toggles
struct ESPState {
    bool boxEnabled;
    bool lineEnabled;
    bool skeletonEnabled;
    bool nameEnabled;
    bool glowEnabled;
    bool corneredEnabled;
    float red, green, blue;  // Cor do ESP
    float thickness;          // Espessura das linhas
    float glowIntensity;      // Intensidade do glow
};

struct AppState {
    Feature features[5];
    ESPState esp;
    float sensitivity;
    float aimIntensity;
    float modSpeed;
    int uptimeSeconds;
    bool masterEnabled;
};

static AppState g_state = {
    .features = {
        {"aim", "AIM ASSIST", "Mira automática", false, 75.0f},
        {"recoil", "NO RECOIL", "Elimina recuo", false, 100.0f},
        {"speed", "SPEED HACK", "+30% velocidade", false, 30.0f},
        {"fly", "FLY MOD", "Modo voar", false, 150.0f},
        {"esp", "ESP WALLHACK", "Ver inimigos", false, 100.0f}
    },
    .esp = {
        .boxEnabled = true,
        .lineEnabled = false,
        .skeletonEnabled = false,
        .nameEnabled = true,
        .glowEnabled = true,
        .corneredEnabled = false,
        .red = 1.0f, .green = 0.0f, .blue = 0.0f,
        .thickness = 2.0f,
        .glowIntensity = 0.5f
    },
    .sensitivity = 50.0f,
    .aimIntensity = 75.0f,
    .modSpeed = 100.0f,
    .uptimeSeconds = 0,
    .masterEnabled = false
};

// Simulated enemies for demo
static std::vector<Enemy> g_enemies;

static void updateDemoEnemies() {
    g_enemies.clear();
    
    const char* names[] = {"Player1", "SnipeMaster", "ProGamer", "NoobPlayer", 
                          "KillShot", "DarkNinja", "FireKing", "IceWolf"};
    
    Enemy e;
    for (int i = 0; i < 8; i++) {
        e.x = 100.0f + (i * 50.0f);
        e.y = 200.0f + (i * 30.0f);
        e.z = 50.0f + (i * 10.0f);
        e.screenX = 150.0f + (i * 90.0f);
        e.screenY = 200.0f + (i % 4) * 120.0f;
        e.width = 40.0f + (i % 3) * 5.0f;
        e.height = 70.0f + (i % 4) * 8.0f;
        e.distance = 30.0f + (i * 15.0f);
        e.health = 100.0f - (i * 8.0f);
        e.visible = (i % 2 == 0);
        e.isEnemy = true;
        snprintf(e.name, sizeof(e.name), "%s", names[i]);
        g_enemies.push_back(e);
    }
    
    LOGI("ESP Demo: %d enemies spawned", (int)g_enemies.size());
}

// Get active ESP types count
static int getActiveESPCount() {
    int count = 0;
    if (g_state.esp.boxEnabled) count++;
    if (g_state.esp.lineEnabled) count++;
    if (g_state.esp.skeletonEnabled) count++;
    if (g_state.esp.nameEnabled) count++;
    if (g_state.esp.glowEnabled) count++;
    if (g_state.esp.corneredEnabled) count++;
    return count;
}

extern "C" {

// ============ FEATURE FUNCTIONS ============

JNIEXPORT jint JNICALL
Java_com_ffpanel_panel_MainActivity_getFeatureCount(JNIEnv *env, jobject) {
    return 5;
}

JNIEXPORT jstring JNICALL
Java_com_ffpanel_panel_MainActivity_getFeatureName(JNIEnv *env, jobject, jint index) {
    if (index >= 0 && index < 5) {
        return env->NewStringUTF(g_state.features[index].name);
    }
    return env->NewStringUTF("");
}

JNIEXPORT jstring JNICALL
Java_com_ffpanel_panel_MainActivity_getFeatureDesc(JNIEnv *env, jobject, jint index) {
    if (index >= 0 && index < 5) {
        return env->NewStringUTF(g_state.features[index].description);
    }
    return env->NewStringUTF("");
}

JNIEXPORT jboolean JNICALL
Java_com_ffpanel_panel_MainActivity_isFeatureEnabled(JNIEnv *env, jobject, jint index) {
    if (index >= 0 && index < 5) {
        return g_state.features[index].enabled;
    }
    return JNI_FALSE;
}

JNIEXPORT void JNICALL
Java_com_ffpanel_panel_MainActivity_setFeatureEnabled(JNIEnv *env, jobject, jint index, jboolean enabled) {
    if (index >= 0 && index < 5) {
        g_state.features[index].enabled = enabled;
        if (index == 4 && enabled) {
            updateDemoEnemies();
            LOGI("ESP Wallhack activated - %d enemies detected", (int)g_enemies.size());
        }
        LOGI("Feature %s set to: %s", g_state.features[index].name, enabled ? "ON" : "OFF");
    }
}

JNIEXPORT jfloat JNICALL
Java_com_ffpanel_panel_MainActivity_getFeatureIntensity(JNIEnv *env, jobject, jint index) {
    if (index >= 0 && index < 5) {
        return g_state.features[index].intensity;
    }
    return 0.0f;
}

JNIEXPORT jint JNICALL
Java_com_ffpanel_panel_MainActivity_getActiveModsCount(JNIEnv *env, jobject) {
    int count = 0;
    for (int i = 0; i < 5; i++) {
        if (g_state.features[i].enabled) count++;
    }
    return count;
}

// ============ ESP MAIN TOGGLE ============

JNIEXPORT jboolean JNICALL
Java_com_ffpanel_panel_MainActivity_isESPEnabled(JNIEnv *env, jobject) {
    return g_state.features[4].enabled ? JNI_TRUE : JNI_FALSE;
}

// ============ ESP INDIVIDUAL TOGGLES ============

JNIEXPORT jboolean JNICALL
Java_com_ffpanel_panel_MainActivity_isBoxESPEnabled(JNIEnv *env, jobject) {
    return g_state.esp.boxEnabled ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT void JNICALL
Java_com_ffpanel_panel_MainActivity_setBoxESPEnabled(JNIEnv *env, jobject, jboolean enabled) {
    g_state.esp.boxEnabled = enabled;
    LOGI("Box ESP: %s", enabled ? "ON" : "OFF");
}

JNIEXPORT jboolean JNICALL
Java_com_ffpanel_panel_MainActivity_isLineESPEnabled(JNIEnv *env, jobject) {
    return g_state.esp.lineEnabled ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT void JNICALL
Java_com_ffpanel_panel_MainActivity_setLineESPEnabled(JNIEnv *env, jobject, jboolean enabled) {
    g_state.esp.lineEnabled = enabled;
    LOGI("Line ESP: %s", enabled ? "ON" : "OFF");
}

JNIEXPORT jboolean JNICALL
Java_com_ffpanel_panel_MainActivity_isSkeletonESPEnabled(JNIEnv *env, jobject) {
    return g_state.esp.skeletonEnabled ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT void JNICALL
Java_com_ffpanel_panel_MainActivity_setSkeletonESPEnabled(JNIEnv *env, jobject, jboolean enabled) {
    g_state.esp.skeletonEnabled = enabled;
    LOGI("Skeleton ESP: %s", enabled ? "ON" : "OFF");
}

JNIEXPORT jboolean JNICALL
Java_com_ffpanel_panel_MainActivity_isNameESPEnabled(JNIEnv *env, jobject) {
    return g_state.esp.nameEnabled ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT void JNICALL
Java_com_ffpanel_panel_MainActivity_setNameESPEnabled(JNIEnv *env, jobject, jboolean enabled) {
    g_state.esp.nameEnabled = enabled;
    LOGI("Name ESP: %s", enabled ? "ON" : "OFF");
}

JNIEXPORT jboolean JNICALL
Java_com_ffpanel_panel_MainActivity_isGlowESPEnabled(JNIEnv *env, jobject) {
    return g_state.esp.glowEnabled ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT void JNICALL
Java_com_ffpanel_panel_MainActivity_setGlowESPEnabled(JNIEnv *env, jobject, jboolean enabled) {
    g_state.esp.glowEnabled = enabled;
    LOGI("Glow ESP: %s", enabled ? "ON" : "OFF");
}

JNIEXPORT jboolean JNICALL
Java_com_ffpanel_panel_MainActivity_isCorneredESPEnabled(JNIEnv *env, jobject) {
    return g_state.esp.corneredEnabled ? JNI_TRUE : JNI_FALSE;
}

JNIEXPORT void JNICALL
Java_com_ffpanel_panel_MainActivity_setCorneredESPEnabled(JNIEnv *env, jobject, jboolean enabled) {
    g_state.esp.corneredEnabled = enabled;
    LOGI("Cornered Box ESP: %s", enabled ? "ON" : "OFF");
}

// ============ ESP ENEMY DATA ============

JNIEXPORT jint JNICALL
Java_com_ffpanel_panel_MainActivity_getEnemyCount(JNIEnv *env, jobject) {
    return (jint)g_enemies.size();
}

JNIEXPORT jfloat JNICALL
Java_com_ffpanel_panel_MainActivity_getEnemyX(JNIEnv *env, jobject, jint index) {
    if (index >= 0 && index < (int)g_enemies.size()) {
        return g_enemies[index].screenX;
    }
    return 0.0f;
}

JNIEXPORT jfloat JNICALL
Java_com_ffpanel_panel_MainActivity_getEnemyY(JNIEnv *env, jobject, jint index) {
    if (index >= 0 && index < (int)g_enemies.size()) {
        return g_enemies[index].screenY;
    }
    return 0.0f;
}

JNIEXPORT jfloat JNICALL
Java_com_ffpanel_panel_MainActivity_getEnemyWidth(JNIEnv *env, jobject, jint index) {
    if (index >= 0 && index < (int)g_enemies.size()) {
        return g_enemies[index].width;
    }
    return 0.0f;
}

JNIEXPORT jfloat JNICALL
Java_com_ffpanel_panel_MainActivity_getEnemyHeight(JNIEnv *env, jobject, jint index) {
    if (index >= 0 && index < (int)g_enemies.size()) {
        return g_enemies[index].height;
    }
    return 0.0f;
}

JNIEXPORT jfloat JNICALL
Java_com_ffpanel_panel_MainActivity_getEnemyDistance(JNIEnv *env, jobject, jint index) {
    if (index >= 0 && index < (int)g_enemies.size()) {
        return g_enemies[index].distance;
    }
    return 0.0f;
}

JNIEXPORT jfloat JNICALL
Java_com_ffpanel_panel_MainActivity_getEnemyHealth(JNIEnv *env, jobject, jint index) {
    if (index >= 0 && index < (int)g_enemies.size()) {
        return g_enemies[index].health;
    }
    return 0.0f;
}

JNIEXPORT jboolean JNICALL
Java_com_ffpanel_panel_MainActivity_isEnemyVisible(JNIEnv *env, jobject, jint index) {
    if (index >= 0 && index < (int)g_enemies.size()) {
        return g_enemies[index].visible ? JNI_TRUE : JNI_FALSE;
    }
    return JNI_FALSE;
}

JNIEXPORT jstring JNICALL
Java_com_ffpanel_panel_MainActivity_getEnemyName(JNIEnv *env, jobject, jint index) {
    if (index >= 0 && index < (int)g_enemies.size()) {
        return env->NewStringUTF(g_enemies[index].name);
    }
    return env->NewStringUTF("");
}

JNIEXPORT jint JNICALL
Java_com_ffpanel_panel_MainActivity_getActiveESPCount(JNIEnv *env, jobject) {
    return getActiveESPCount();
}

// ============ ESP COLOR & SETTINGS ============

JNIEXPORT jfloat JNICALL
Java_com_ffpanel_panel_MainActivity_getESPR(JNIEnv *env, jobject) {
    return g_state.esp.red;
}

JNIEXPORT jfloat JNICALL
Java_com_ffpanel_panel_MainActivity_getESPG(JNIEnv *env, jobject) {
    return g_state.esp.green;
}

JNIEXPORT jfloat JNICALL
Java_com_ffpanel_panel_MainActivity_getESPB(JNIEnv *env, jobject) {
    return g_state.esp.blue;
}

JNIEXPORT void JNICALL
Java_com_ffpanel_panel_MainActivity_setESPColor(JNIEnv *env, jobject, jfloat r, jfloat g, jfloat b) {
    g_state.esp.red = r;
    g_state.esp.green = g;
    g_state.esp.blue = b;
    LOGI("ESP Color: R=%.2f G=%.2f B=%.2f", r, g, b);
}

JNIEXPORT jfloat JNICALL
Java_com_ffpanel_panel_MainActivity_getESPGlowIntensity(JNIEnv *env, jobject) {
    return g_state.esp.glowIntensity;
}

JNIEXPORT void JNICALL
Java_com_ffpanel_panel_MainActivity_setESPGlowIntensity(JNIEnv *env, jobject, jfloat intensity) {
    g_state.esp.glowIntensity = intensity;
}

JNIEXPORT jfloat JNICALL
Java_com_ffpanel_panel_MainActivity_getESPThickness(JNIEnv *env, jobject) {
    return g_state.esp.thickness;
}

JNIEXPORT void JNICALL
Java_com_ffpanel_panel_MainActivity_setESPThickness(JNIEnv *env, jobject, jfloat thickness) {
    g_state.esp.thickness = thickness;
}

// ============ SETTINGS FUNCTIONS ============

JNIEXPORT jfloat JNICALL
Java_com_ffpanel_panel_MainActivity_getSensitivity(JNIEnv *env, jobject) {
    return g_state.sensitivity;
}

JNIEXPORT void JNICALL
Java_com_ffpanel_panel_MainActivity_setSensitivity(JNIEnv *env, jobject, jfloat value) {
    g_state.sensitivity = value;
}

JNIEXPORT jfloat JNICALL
Java_com_ffpanel_panel_MainActivity_getAimIntensity(JNIEnv *env, jobject) {
    return g_state.aimIntensity;
}

JNIEXPORT void JNICALL
Java_com_ffpanel_panel_MainActivity_setAimIntensity(JNIEnv *env, jobject, jfloat value) {
    g_state.aimIntensity = value;
}

JNIEXPORT jfloat JNICALL
Java_com_ffpanel_panel_MainActivity_getModSpeed(JNIEnv *env, jobject) {
    return g_state.modSpeed;
}

JNIEXPORT void JNICALL
Java_com_ffpanel_panel_MainActivity_setModSpeed(JNIEnv *env, jobject, jfloat value) {
    g_state.modSpeed = value;
}

JNIEXPORT void JNICALL
Java_com_ffpanel_panel_MainActivity_activateAll(JNIEnv *env, jobject) {
    for (int i = 0; i < 5; i++) {
        g_state.features[i].enabled = true;
    }
    // Enable all ESP types
    g_state.esp.boxEnabled = true;
    g_state.esp.lineEnabled = true;
    g_state.esp.skeletonEnabled = true;
    g_state.esp.nameEnabled = true;
    g_state.esp.glowEnabled = true;
    g_state.esp.corneredEnabled = true;
    g_state.masterEnabled = true;
    updateDemoEnemies();
    LOGI("All features + ESP types activated");
}

JNIEXPORT void JNICALL
Java_com_ffpanel_panel_MainActivity_deactivateAll(JNIEnv *env, jobject) {
    for (int i = 0; i < 5; i++) {
        g_state.features[i].enabled = false;
    }
    // Disable all ESP types
    g_state.esp.boxEnabled = false;
    g_state.esp.lineEnabled = false;
    g_state.esp.skeletonEnabled = false;
    g_state.esp.nameEnabled = false;
    g_state.esp.glowEnabled = false;
    g_state.esp.corneredEnabled = false;
    g_state.masterEnabled = false;
    LOGI("All features deactivated");
}

JNIEXPORT void JNICALL
Java_com_ffpanel_panel_MainActivity_resetAll(JNIEnv *env, jobject) {
    g_state.sensitivity = 50.0f;
    g_state.aimIntensity = 75.0f;
    g_state.modSpeed = 100.0f;
    g_state.esp.red = 1.0f;
    g_state.esp.green = 0.0f;
    g_state.esp.blue = 0.0f;
    g_state.esp.thickness = 2.0f;
    g_state.esp.glowIntensity = 0.5f;
    g_state.esp.boxEnabled = true;
    g_state.esp.lineEnabled = false;
    g_state.esp.skeletonEnabled = false;
    g_state.esp.nameEnabled = true;
    g_state.esp.glowEnabled = true;
    g_state.esp.corneredEnabled = false;
    for (int i = 0; i < 5; i++) {
        g_state.features[i].enabled = false;
        g_state.features[i].intensity = (i == 0) ? 75.0f : (i == 1) ? 100.0f : (i == 2) ? 30.0f : (i == 3) ? 150.0f : 100.0f;
    }
    g_state.masterEnabled = false;
    g_enemies.clear();
    LOGI("All settings reset to default");
}

JNIEXPORT jboolean JNICALL
Java_com_ffpanel_panel_MainActivity_isMasterEnabled(JNIEnv *env, jobject) {
    return g_state.masterEnabled ? JNI_TRUE : JNI_FALSE;
}

}
