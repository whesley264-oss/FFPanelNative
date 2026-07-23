#include <GLES3/gl3.h>
#include <math.h>
#include <vector>

struct Vec2 { float x, y; };
struct Vec4 { float r, g, b, a; };

class UIColors {
public:
    static Vec4 ffGreen() { return {0.0f, 1.0f, 0.533f, 1.0f}; }
    static Vec4 ffGreenDim() { return {0.0f, 0.8f, 0.42f, 1.0f}; }
    static Vec4 ffGreenSubtle() { return {0.0f, 1.0f, 0.533f, 0.15f}; }
    static Vec4 ffOrange() { return {1.0f, 0.42f, 0.21f, 1.0f}; }
    static Vec4 ffCyan() { return {0.0f, 0.83f, 1.0f, 1.0f}; }
    static Vec4 ffMagenta() { return {1.0f, 0.0f, 1.0f, 1.0f}; }
    static Vec4 bgDark() { return {0.039f, 0.039f, 0.059f, 1.0f}; }
    static Vec4 bgCard() { return {0.071f, 0.071f, 0.102f, 1.0f}; }
    static Vec4 textPrimary() { return {1.0f, 1.0f, 1.0f, 1.0f}; }
    static Vec4 textSecondary() { return {0.54f, 0.54f, 0.6f, 1.0f}; }
    static Vec4 danger() { return {1.0f, 0.278f, 0.341f, 1.0f}; }
};

class Button {
public:
    float x, y, w, h;
    const char* text;
    Vec4 bgColor;
    Vec4 textColor;
    bool isPrimary;
    
    Button(float px, float py, float pw, float ph, const char* ptext, bool primary = false) 
        : x(px), y(py), w(pw), h(ph), text(ptext), isPrimary(primary) {
        if (primary) {
            bgColor = UIColors::ffGreen();
            textColor = UIColors::bgDark();
        } else {
            bgColor = UIColors::bgDark();
            textColor = UIColors::ffGreen();
        }
    }
    
    bool contains(float px, float py) {
        return px >= x && px <= x + w && py >= y && py <= y + h;
    }
};

class Toggle {
public:
    float x, y, w, h;
    bool* value;
    
    Toggle(float px, float py, float pw, float ph, bool* pval) 
        : x(px), y(py), w(pw), h(ph), value(pval) {}
    
    bool contains(float px, float py) {
        return px >= x && px <= x + w && py >= y && py <= y + h;
    }
    
    void toggle() {
        *value = !(*value);
    }
};

class Slider {
public:
    float x, y, w, h;
    float* value;
    float minVal, maxVal;
    
    Slider(float px, float py, float pw, float ph, float* pval, float min, float max)
        : x(px), y(py), w(pw), h(ph), value(pval), minVal(min), maxVal(max) {}
    
    bool contains(float px, float py) {
        return px >= x && px <= x + w && py >= y - 20 && py <= y + h + 20;
    }
    
    void setFromX(float px) {
        float ratio = (px - x) / w;
        if (ratio < 0) ratio = 0;
        if (ratio > 1) ratio = 1;
        *value = minVal + ratio * (maxVal - minVal);
    }
    
    float getThumbX() {
        float ratio = (*value - minVal) / (maxVal - minVal);
        return x + ratio * w;
    }
};

class FeatureCard {
public:
    int index;
    float x, y, w, h;
    const char* title;
    const char* subtitle;
    bool* enabled;
    float* intensity;
    Vec4 accentColor;
    
    FeatureCard(int idx, float px, float py, float pw, float ph, 
                const char* ptitle, const char* psub, bool* penab, float* pinten)
        : index(idx), x(px), y(py), w(pw), h(ph), title(ptitle), subtitle(psub),
          enabled(penab), intensity(pinten) {
        accentColor = UIColors::ffGreen();
    }
    
    bool contains(float px, float py) {
        return px >= x && px <= x + w && py >= y && py <= y + h;
    }
};

class StatCard {
public:
    float x, y, w, h;
    const char* label;
    const char* value;
    Vec4 color;
    
    StatCard(float px, float py, float pw, float ph, const char* plbl, const char* pval, Vec4 c)
        : x(px), y(py), w(pw), h(ph), label(plbl), value(pval), color(c) {}
};
