#ifndef BLOOMRENDERER_HPP
#define BLOOMRENDERER_HPP

#include "glad.h"
#include <iostream>

// Pipeline bloom :
//   1) Rendu scene -> FBO HDR (sceneColorTex)
//   2) P passe "bright extract" : seuil de luminance -> brightTex
//   3) Passes de flou gaussien (H puis V) itérées sur pingPongTex[0,1]
//   4) Passe de compositage final : scene + bloom dans le framebuffer par défaut
class BloomRenderer {
private:
    unsigned int fboScene;
    unsigned int sceneColorTex;
    unsigned int sceneDepthTex;

    unsigned int fboBright;
    unsigned int brightTex;

    unsigned int pingPongFBO[2];
    unsigned int pingPongTex[2];

    unsigned int brightPassProgram;
    unsigned int blurProgram;
    unsigned int compositeProgram;

    unsigned int quadVAO, quadVBO;

    int width = 0, height = 0;

    // uniform locations
    int locBrightTex = -1, locBrightThreshold = -1;
    int locBlurImage = -1, locBlurHorizontal = -1, locTexelSize = -1;
    int locCompositeScene = -1, locCompositeBloom = -1, locCompositeIntensity = -1;

    unsigned int compileShader(const char* source, GLenum type) {
        unsigned int shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, NULL);
        glCompileShader(shader);
        int success; char infoLog[512];
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 512, NULL, infoLog);
            std::cerr << "Erreur compilation shader bloom : " << infoLog << std::endl;
        }
        return shader;
    }

    unsigned int buildProgram(const char* vsSrc, const char* fsSrc) {
        unsigned int vs = compileShader(vsSrc, GL_VERTEX_SHADER);
        unsigned int fs = compileShader(fsSrc, GL_FRAGMENT_SHADER);
        unsigned int prog = glCreateProgram();
        glAttachShader(prog, vs);
        glAttachShader(prog, fs);
        glLinkProgram(prog);
        int success; char infoLog[512];
        glGetProgramiv(prog, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(prog, 512, NULL, infoLog);
            std::cerr << "Erreur lien programme bloom : " << infoLog << std::endl;
        }
        glDeleteShader(vs);
        glDeleteShader(fs);
        return prog;
    }

    void deleteFBOs() {
        if (fboScene) {
            glDeleteFramebuffers(1, &fboScene);
            glDeleteTextures(1, &sceneColorTex);
            glDeleteTextures(1, &sceneDepthTex);
            fboScene = 0;
        }
        if (fboBright) {
            glDeleteFramebuffers(1, &fboBright);
            glDeleteTextures(1, &brightTex);
            fboBright = 0;
        }
        for (int i = 0; i < 2; i++) {
            if (pingPongFBO[i]) {
                glDeleteFramebuffers(1, &pingPongFBO[i]);
                glDeleteTextures(1, &pingPongTex[i]);
                pingPongFBO[i] = 0;
            }
        }
    }

public:
    float threshold = 0.6f;   // luminance minimum pour qu'un pixel alimente le bloom
    float intensity = 1.2f;  // intensité du glow ajouté à la scène
    int   blurPasses = 5;    // nombre d'itérations de flou gaussien (chaque itération = 1 horizontale + 1 verticale)

    BloomRenderer() {
        // ---- Shaders ----
        static const char* screenVS = R"(
            #version 330 core
            layout (location = 0) in vec2 aPos;
            out vec2 vUV;
            void main() {
                vUV = aPos * 0.5 + 0.5;
                gl_Position = vec4(aPos, 0.0, 1.0);
            }
        )";

        // Bright pass : ne garde que les pixels dont la luminance > threshold
        static const char* brightFS = R"(
            #version 330 core
            in vec2 vUV;
            out vec4 FragColor;
            uniform sampler2D uTex;
            uniform float uThreshold;
            void main() {
                vec3 c = texture(uTex, vUV).rgb;
                float l = dot(c, vec3(0.2126, 0.7152, 0.0722));
                float contribution = max(0.0, l - uThreshold);
                float scale = l > 0.0001 ? contribution / l : 0.0;
                FragColor = vec4(c * scale, 1.0);
            }
        )";

        // Flou gaussien 9-tap (passe H ou V)
        static const char* blurFS = R"(
            #version 330 core
            in vec2 vUV;
            out vec4 FragColor;
            uniform sampler2D uImage;
            uniform vec2 uTexelSize;
            uniform bool uHorizontal;
            void main() {
                float weight[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);
                vec3 result = texture(uImage, vUV).rgb * weight[0];
                vec2 dir = uHorizontal ? vec2(uTexelSize.x, 0.0) : vec2(0.0, uTexelSize.y);
                for (int i = 1; i < 5; i++) {
                    result += texture(uImage, vUV + dir * float(i)).rgb * weight[i];
                    result += texture(uImage, vUV - dir * float(i)).rgb * weight[i];
                }
                FragColor = vec4(result, 1.0);
            }
        )";

        // Compositage final HDR -> LDR avec tone mapping ACES soft
        static const char* compositeFS = R"(
            #version 330 core
            in vec2 vUV;
            out vec4 FragColor;
            uniform sampler2D uScene;
            uniform sampler2D uBloom;
            uniform float uBloomIntensity;
            vec3 aces(vec3 x) {
                const float a = 2.51, b = 0.03, c = 2.43, d = 0.59, e = 0.14;
                return clamp((x*(a*x+b))/(x*(c*x+d)+e), 0.0, 1.0);
            }
            void main() {
                vec3 scene = texture(uScene, vUV).rgb;
                vec3 bloom = texture(uBloom, vUV).rgb;
                vec3 hdr = scene + bloom * uBloomIntensity;
                FragColor = vec4(aces(hdr), 1.0);
            }
        )";

        brightPassProgram = buildProgram(screenVS, brightFS);
        blurProgram       = buildProgram(screenVS, blurFS);
        compositeProgram  = buildProgram(screenVS, compositeFS);

        locBrightTex         = glGetUniformLocation(brightPassProgram, "uTex");
        locBrightThreshold   = glGetUniformLocation(brightPassProgram, "uThreshold");

        locBlurImage         = glGetUniformLocation(blurProgram, "uImage");
        locTexelSize         = glGetUniformLocation(blurProgram, "uTexelSize");
        locBlurHorizontal    = glGetUniformLocation(blurProgram, "uHorizontal");

        locCompositeScene    = glGetUniformLocation(compositeProgram, "uScene");
        locCompositeBloom     = glGetUniformLocation(compositeProgram, "uBloom");
        locCompositeIntensity= glGetUniformLocation(compositeProgram, "uBloomIntensity");

        // ---- Quad plein écran ----
        float quadVerts[] = {
            -1.0f, -1.0f,
             1.0f, -1.0f,
            -1.0f,  1.0f,
             1.0f,  1.0f,
        };
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerts), quadVerts, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glBindVertexArray(0);
    }

    ~BloomRenderer() {
        deleteFBOs();
        glDeleteProgram(brightPassProgram);
        glDeleteProgram(blurProgram);
        glDeleteProgram(compositeProgram);
        glDeleteVertexArrays(1, &quadVAO);
        glDeleteBuffers(1, &quadVBO);
    }

    // (Re)cré les FBOs et textures à la taille de la fenêtre
    void resize(int w, int h) {
        if (w == width && h == height && fboScene != 0) return;
        deleteFBOs();
        width = w; height = h;

        // Scene FBO (HDR via RGBA16F)
        glGenFramebuffers(1, &fboScene);
        glBindFramebuffer(GL_FRAMEBUFFER, fboScene);

        glGenTextures(1, &sceneColorTex);
        glBindTexture(GL_TEXTURE_2D, sceneColorTex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_HALF_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sceneColorTex, 0);

        glGenTextures(1, &sceneDepthTex);
        glBindTexture(GL_TEXTURE_2D, sceneDepthTex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, sceneDepthTex, 0);

        // Bright FBO
        glGenFramebuffers(1, &fboBright);
        glBindFramebuffer(GL_FRAMEBUFFER, fboBright);
        glGenTextures(1, &brightTex);
        glBindTexture(GL_TEXTURE_2D, brightTex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_HALF_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brightTex, 0);

        // Ping-pong FBOs pour le flou itératif
        for (int i = 0; i < 2; i++) {
            glGenFramebuffers(1, &pingPongFBO[i]);
            glBindFramebuffer(GL_FRAMEBUFFER, pingPongFBO[i]);
            glGenTextures(1, &pingPongTex[i]);
            glBindTexture(GL_TEXTURE_2D, pingPongTex[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_HALF_FLOAT, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingPongTex[i], 0);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    // À appeler avant le rendu de la scène. Le SceneFBO est lié et cleared (noir profond).
    void beginScene() {
        glBindFramebuffer(GL_FRAMEBUFFER, fboScene);
        glViewport(0, 0, width, height);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    // À appeler après le rendu de la scène. Exécute bright pass + flou gaussien itératif.
    void renderBloom() {
        glDepthMask(GL_FALSE);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);

        // 1) Bright pass : sceneColorTex -> brightTex (seuil)
        glBindFramebuffer(GL_FRAMEBUFFER, fboBright);
        glViewport(0, 0, width, height);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(brightPassProgram);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, sceneColorTex);
        glUniform1i(locBrightTex, 0);
        glUniform1f(locBrightThreshold, threshold);
        drawQuad();

        // 2) Flou gaussien itératif sur pingPongTex[0,1]，départ depuis brightTex
        bool horizontal = true;
        bool firstIteration = true;
        for (int pass = 0; pass < blurPasses * 2; pass++) {
            glBindFramebuffer(GL_FRAMEBUFFER, pingPongFBO[horizontal ? 1 : 0]);
            glViewport(0, 0, width, height);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            glUseProgram(blurProgram);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, firstIteration ? brightTex : pingPongTex[horizontal ? 0 : 1]);
            glUniform1i(locBlurImage, 0);
            glUniform2f(locTexelSize, 1.0f / width, 1.0f / height);
            glUniform1i(locBlurHorizontal, horizontal ? 1 : 0);
            drawQuad();
            firstIteration = false;
            horizontal = !horizontal;
        }

        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
    }

    // Composite scene + bloom dans le framebuffer par défaut (fenêtre)
    void renderToScreen() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, width, height);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        glUseProgram(compositeProgram);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, sceneColorTex);
        glUniform1i(locCompositeScene, 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, pingPongTex[0]); // dernier résultat du flou (horizontal==false -> index 0)
        glUniform1i(locCompositeBloom, 1);
        glUniform1f(locCompositeIntensity, intensity);
        drawQuad();
    }

    void drawQuad() {
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
};

using BloomRendererPtr = std::shared_ptr<BloomRenderer>;

#endif