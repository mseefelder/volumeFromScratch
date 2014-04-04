#include "../include/texture.hpp"

GLuint Texture::create(GLenum type, GLenum int_format, int w, int h, GLenum fmt, GLenum pix_type, const GLvoid* data, int dpt) {

    tex_type = type;
    internal_format = int_format;
    width = w;
    height = h;
    format = fmt;
    pixel_type = pix_type;
    lod = 0;
    depth = dpt;

    if (tex_id != 0)
        glDeleteTextures(1, &tex_id);

    glGenTextures(1, &tex_id);
    glBindTexture(tex_type, tex_id);
    if(tex_type == GL_TEXTURE_2D || tex_type == GL_TEXTURE_RECTANGLE) {
        glTexImage2D(tex_type, lod, internal_format, width, height, 0, format, pixel_type, data);
    }
    else if (tex_type == GL_TEXTURE_3D) {
        glTexImage3D(tex_type, lod, internal_format, width, height, depth, 0, format, pixel_type, data);
    }
    else if (tex_type == GL_TEXTURE_1D) {
        glTexImage1D(tex_type, lod, internal_format, width, 0, format, pixel_type, data);
    }

    // default parameters
    setTexParameters();

    glBindTexture(tex_type, 0);
    return tex_id;
}

/*//NEW------------------------------
GLuint Texture::resetTexture(GLenum type, GLenum int_format, int w, int h, GLenum fmt, GLenum pix_type, const GLvoid* data, int dpt) {

    tex_type = type;
    internal_format = int_format;
    width = w;
    height = h;
    format = fmt;
    pixel_type = pix_type;
    lod = 0;
    depth = dpt;


    if (tex_id != 0)
        glDeleteTextures(1, &tex_id);

    glGenTextures(1, &tex_id);

    glBindTexture(tex_type, tex_id);

    if(tex_type == GL_TEXTURE_2D || tex_type == GL_TEXTURE_RECTANGLE) {
        glTexImage2D(tex_type, lod, internal_format, width, height, 0, format, pixel_type, data);
    }
    else if (tex_type == GL_TEXTURE_3D) {
        glTexImage3D(tex_type, lod, internal_format, width, height, depth, 0, format, pixel_type, data);
    }
    else if (tex_type == GL_TEXTURE_1D) {
        glTexImage1D(tex_type, lod, internal_format, width, 0, format, pixel_type, data);
    }

    // default parameters
    setTexParameters();

    glBindTexture(tex_type, 0);
    return tex_id;
}
//*/

void Texture::setTexParameters(GLenum wraps, GLenum wrapt, GLenum wrapr, GLenum magfilter, GLenum minfilter) {
    glTexParameteri(tex_type, GL_TEXTURE_WRAP_S, wraps);
    glTexParameteri(tex_type, GL_TEXTURE_WRAP_T, wrapt);
    if (tex_type == GL_TEXTURE_3D){
        glTexParameteri(tex_type, GL_TEXTURE_WRAP_R, wrapr);
    }
    glTexParameteri(tex_type, GL_TEXTURE_MAG_FILTER, magfilter);
    glTexParameteri(tex_type, GL_TEXTURE_MIN_FILTER, minfilter);
}

void Texture::setTexParametersMipMap(int maxlevel, int baselevel, GLenum wraps, GLenum wrapt, GLenum magfilter, GLenum minfilter) {

    glTexParameteri(tex_type, GL_TEXTURE_MIN_FILTER, minfilter);
    glTexParameteri(tex_type, GL_TEXTURE_MAG_FILTER, magfilter);
    glTexParameteri(tex_type, GL_TEXTURE_WRAP_S, wraps);
    glTexParameteri(tex_type, GL_TEXTURE_WRAP_T, wrapt);
    glTexParameteri(tex_type, GL_TEXTURE_BASE_LEVEL, baselevel );
    glTexParameteri(tex_type, GL_TEXTURE_MAX_LEVEL, maxlevel );

    glGenerateMipmap(tex_type);
}


void Texture::update(const GLvoid* data) {

    glBindTexture(tex_type, tex_id);
    if(tex_type == GL_TEXTURE_2D || tex_type == GL_TEXTURE_RECTANGLE) {
        glTexImage2D(tex_type, lod, internal_format, width, height, 0, format,pixel_type,data);
    }
    else if (tex_type == GL_TEXTURE_3D) {
        glTexImage3D(tex_type, lod, internal_format,width, height, depth, 0,format,pixel_type,data);
    }
    else if (tex_type == GL_TEXTURE_1D) {
        glTexImage1D(tex_type, lod, internal_format,width,0,format,pixel_type,data);
    }
    glBindTexture(tex_type,0);

}

void Texture::bind(int texture_unit) {
    unit = texture_unit;
    texManager->bindTexture(tex_type, tex_id, texture_unit);
}

int Texture::bind(void) {    
    unit = texManager->bindTexture(tex_type, tex_id);
    return unit;
}

int Texture::bindImageRW(void) {
    unit = texManager->bindTexture(tex_type, tex_id);
    if (unit != -1) {
        glBindImageTexture(unit, tex_id, 0, false, 0,  GL_READ_WRITE, internal_format);
    }
    return unit;
}

void Texture::bindImageRW(int texture_unit) {
    unit = texture_unit;
    texManager->bindTexture(tex_type, tex_id, texture_unit);
    glBindImageTexture(texture_unit, tex_id, 0, /*to use 3d texture*/GL_TRUE, 0,  GL_READ_WRITE, internal_format);
}

int Texture::bindImageFormatRW(GLenum fmt) {
    unit = texManager->bindTexture(tex_type, tex_id);

    if (unit != -1) {
        glBindImageTexture(unit, tex_id, 0, false, 0,  GL_READ_WRITE, fmt);
    }
    return unit;
}

void Texture::bindImageFormatRW(int texture_unit, GLenum fmt) {
    unit = texture_unit;
    texManager->bindTexture(tex_type, tex_id, texture_unit);
    glBindImageTexture(texture_unit, tex_id, 0, false, 0,  GL_READ_WRITE, fmt);
}

void Texture::unbind() {
    //texManager->unbindTexture(tex_type, unit);
    texManager->unbindTextureID(tex_type, tex_id);
    unit = -1;
}
