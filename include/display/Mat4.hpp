#ifndef MAT4_HPP
#define MAT4_HPP

#include <cmath>

// Structure pour une matrice 4x4
struct Mat4 {
    float m[16];
    
    Mat4() {
        // Matrice identité
        for(int i = 0; i < 16; i++) m[i] = 0.0f;
        m[0] = m[5] = m[10] = m[15] = 1.0f;
    }
    
    // Constructeur pour matrice de projection
    static Mat4 perspective(float fov, float aspect, float near, float far) {
        Mat4 result;
        float f = 1.0f / tan(fov * 0.5f * 3.14159f / 180.0f);
        result.m[0] = f / aspect;
        result.m[5] = f;
        result.m[10] = (far + near) / (near - far);
        result.m[11] = -1.0f;
        result.m[14] = (2.0f * far * near) / (near - far);
        result.m[15] = 0.0f;
        return result;
    }
    
    // Constructeur pour matrice de vue (lookAt)
    static Mat4 lookAt(float eyeX, float eyeY, float eyeZ, 
                      float centerX, float centerY, float centerZ,
                      float upX, float upY, float upZ) {
        Mat4 result;
        
        // Calculer les vecteurs de base
        float z[3] = {eyeX - centerX, eyeY - centerY, eyeZ - centerZ};
        float len = sqrt(z[0]*z[0] + z[1]*z[1] + z[2]*z[2]);
        z[0] /= len; z[1] /= len; z[2] /= len;
        
        float x[3] = {upY*z[2] - upZ*z[1], upZ*z[0] - upX*z[2], upX*z[1] - upY*z[0]};
        len = sqrt(x[0]*x[0] + x[1]*x[1] + x[2]*x[2]);
        x[0] /= len; x[1] /= len; x[2] /= len;
        
        float y[3] = {z[1]*x[2] - z[2]*x[1], z[2]*x[0] - z[0]*x[2], z[0]*x[1] - z[1]*x[0]};
        
        // Remplir la matrice
        result.m[0] = x[0]; result.m[1] = y[0]; result.m[2] = z[0]; result.m[3] = 0.0f;
        result.m[4] = x[1]; result.m[5] = y[1]; result.m[6] = z[1]; result.m[7] = 0.0f;
        result.m[8] = x[2]; result.m[9] = y[2]; result.m[10] = z[2]; result.m[11] = 0.0f;
        result.m[12] = -(x[0]*eyeX + x[1]*eyeY + x[2]*eyeZ);
        result.m[13] = -(y[0]*eyeX + y[1]*eyeY + y[2]*eyeZ);
        result.m[14] = -(z[0]*eyeX + z[1]*eyeY + z[2]*eyeZ);
        result.m[15] = 1.0f;
        
        return result;
    }
};

#endif