#ifndef __TEXTURE__
#define __TEXTURE__

#include <textureManager.hpp>
#include <iostream>
#include <GL/glew.h>

using namespace std;

/**
 * @brief The Texture class holds one texture, it can be a simple texture or an FBO texture
 */
class Texture {

	public:

    ///Default Constructor
    Texture() {
        tex_id = 0;
    }

    /// Default Destructor
    ~Texture() {
        if (tex_id != 0) {
            glDeleteTextures(1, &tex_id);
        }

    }


    /**
     * Creates a texture object and returns its handler.
     * @param texType Type of GL texture (usually GL_TEXTURE_2D)
     * @param interalFormat Format of texel (usually GL_RGBA or GL_RGBA32F for precision)
     * @param width Width of texture
     * @param height Height of texture
     * @param format Format of texel channels (usually GL_RGBA)
     * @param pixelType Type of one channel of a texel (usually GL_FLOAT or GL_USIGNED_BYTE)
     * @param data Pointer to data to fill the texture
     * @param depth
     * @return Texture ID (handler for OpenGL)
     **/
    GLuint create(GLenum texType, GLenum internalFormat, int width, int height, GLenum format, GLenum pixelType, const GLvoid* data = NULL, int depth = 256);

    ///NEW
    GLuint resetTexture(GLenum texType, GLenum internalFormat, int width, int height, GLenum format, GLenum pixelType, const GLvoid* data = NULL, int depth = 256);

    /**
     * @brief Set Texture Parameters
     * @param wraps Wrap S
     * @param wrapt Wrap T
     * @param magfilter Mag Filter
     * @param minfilter Min Filter
     */
    void setTexParameters(GLenum wraps = GL_CLAMP, GLenum wrapt = GL_CLAMP, GLenum wrapr = GL_CLAMP, GLenum magfilter = GL_NEAREST, GLenum minfilter = GL_NEAREST);

    /**
     * @brief Set Texture Parameters
     * @param maxlevel Max Mipmap level
     * @param baselevel Mipmap Base Level
     * @param wraps Wrap S
     * @param wrapt Wrap T
     * @param magfilter Mag Filter
     * @param minfilter Min Filter
     */
    void setTexParametersMipMap(int levels, int baseLevel = 0, GLenum wraps = GL_CLAMP, GLenum wrapt = GL_CLAMP, GLenum magfilter = GL_NEAREST, GLenum minfilter = GL_NEAREST_MIPMAP_NEAREST);


    /**
     * Updates the data of the texture mantaining all other parameters.
     * @param data Pointer to data to fill the texture
    **/
    void update(const GLvoid* data);

	/**
     * Binds the texture to a unit given by the user.
	**/
    void bind(int texture_unit);

	/**
     * Binds the texture to the first free texture unit and returns the unit used.
	**/
    int bind(void);

    /**
     * @brief Binds the texture as an image texture (ex. for atomic counter).
     * Binds with READ and WRITE access.
     * @return Unit the texture was bound to, or -1 if no unit available
     */
    int bindImageRW(void);

    /**
     * @brief Binds the texture as an image texture (ex. for atomic counter).
     * Binds with READ and WRITE access.
     * @param unit Texture unit to bound
     * @return Unit the texture was bound to, or -1 if no unit available
     */
    void bindImageRW(int texture_unit);


    /**
     * @brief Binds the texture as an image texture (ex. for atomic counter).
     * Binds with READ and WRITE access.
     * @param format Elements format (ex. GL_R32UI)
     * @return Unit the texture was bound to, or -1 if no unit available
     */
    int bindImageFormatRW(GLenum format);

    /**
     * @brief Binds the texture as an image texture (ex. for atomic counter).
     * Binds with READ and WRITE access.
     * @param unit Texture unit to bound
     * @param format Elements format (ex. GL_R32UI)
     * @return Unit the texture was bound to, or -1 if no unit available
     */
    void bindImageFormatRW(int texture_unit, GLenum format);

    /**
     * @brief Unbinds this texture and frees tex unit
     */
    void unbind();

    /**
     * @brief texID Returns handle (texture ID)
     * @return Tex ID
     */
    GLuint texID (void) const {
        return tex_id;
    }

    /**
     * @brief unit
     * @return The bound texture unit
     */
    int textureUnit (void) const {
        return unit;
    }

private:

    // ID handler to texture
    GLuint tex_id;

    // type of texture as defined by OpenGL (ex. GL_TEXTURE_2D, GL_TEXTURE_3D ...)
    GLenum tex_type;

    // internal format as defined by OpenGL (ex. GL_RGBA, GL_RGBA32F ...)
    GLenum internal_format;

    // width in pixels
    int width;

    // height in pixels
    int height;

    // depth in pixels for 3D texture
    int depth;

    // format of texture channels (ex. GL_RGBA)
    GLenum format;

    // type of a pixel channel (ex. GL_FLOAT, GL_UNSIGNED_BYTE ...)
    GLenum pixel_type;

    //const GLvoid* data = NULL

    // number of lod levels (usually 1)
    int lod;

    // texture unit this texture is occupying (if any)
    int unit;

};


//classe texture
//método fill texture
//criar método pra setar os parâmetros dentro da classe texture e colocar um default (ver ssao noise texture)
//tirar o int unit do ssao. Jogar direto o bindtexture denntro do setuniform
//fbo usar texturemanager

#endif
