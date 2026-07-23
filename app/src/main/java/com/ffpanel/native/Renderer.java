package com.ffpanel.native;

public class Renderer {
    static {
        System.loadLibrary("ffpanel");
    }
    
    public static native void init(int width, int height);
    public static native void render(float dt);
    public static native void shutdown();
}
