#ifndef TEXTURE_H
#define TEXTURE_H

//#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <iostream>

#include <GL/glew.h>
#include <vector>

#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCIIF

class TextureLoader {
public:

    TextureLoader(GLenum TextureTarget, const char* fileName)
    {
        m_textureTarget = TextureTarget;
        m_fileName = fileName;
    }

    bool Load()
    {

        int width, height, nrComponents;
        unsigned char *data = stbi_load(m_fileName, &width, &height, &nrComponents, 0);
        if (data)
        {
            GLenum format;
            if (nrComponents == 1)
                format = GL_RED;
            else if (nrComponents == 3)
                format = GL_RGB;
            else if (nrComponents == 4)
                format = GL_RGBA;

            glGenTextures(1, &m_textureObj);
            glBindTexture(m_textureTarget, m_textureObj);
            glTexImage2D(m_textureTarget, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glTexParameterf(m_textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameterf(m_textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            //glBindTexture(m_textureTarget, 0);

            stbi_image_free(data);
        }
        else
        {
            std::cout << "Texture failed to load at path: " << m_fileName << std::endl;
            stbi_image_free(data);
            return false;
        }

        return true;
    }

    void Bind(GLenum TextureUnit)
    {
        glActiveTexture(TextureUnit);
        glBindTexture(m_textureTarget, m_textureObj);
    }



    static GLuint LoadTexture( const char* path )
    {
        GLuint textureID;
        glGenTextures(1, &textureID);

        int width, height, nrComponents;
        unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
        if (data)
        {
            GLenum format;
            if (nrComponents == 1)
                format = GL_RED;
            else if (nrComponents == 3)
                format = GL_RGB;
            else if (nrComponents == 4)
                format = GL_RGBA;

            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);
        }
        else
        {
            std::cout << "Texture failed to load at path: " << path << std::endl;
            stbi_image_free(data);
        }

        return textureID;
    }


    static GLuint LoadCubeMap( vector<const char*> faces )
    {
        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        int width, height, nrComponents;

        for( uint i = 0; i < faces.size(); i++)
        {
            unsigned char *data = stbi_load(faces[i], &width, &height, &nrComponents, 0);
            if (data)
            {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

                stbi_image_free(data);
            }
            else
            {
                std::cout << "Cubemap texture failed to load at path " << faces[i] << std::endl;
                stbi_image_free(data);
            }
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        return textureID;
    }


    static GLuint loadDDS(const char* imagepath)
    {
        unsigned char header[124];

        FILE *fp;

        /* try to open the file */
        fp = fopen(imagepath, "rb");
        if (fp == NULL){
            printf("%s could not be opened. Are you in the right directory ? Don't forget to read the FAQ !\n", imagepath); getchar();
            return 0;
        }

        /* verify the type of file */
        char filecode[4];
        fread(filecode, 1, 4, fp);
        if (strncmp(filecode, "DDS ", 4) != 0) {
            fclose(fp);
            return 0;
        }

        /* get the surface desc */
        fread(&header, 124, 1, fp);

        unsigned int height      = *(unsigned int*)&(header[8 ]);
        unsigned int width	     = *(unsigned int*)&(header[12]);
        unsigned int linearSize	 = *(unsigned int*)&(header[16]);
        unsigned int mipMapCount = *(unsigned int*)&(header[24]);
        unsigned int fourCC      = *(unsigned int*)&(header[80]);


        unsigned char * buffer;
        unsigned int bufsize;
        /* how big is it going to be including all mipmaps? */
        bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize;
        buffer = (unsigned char*)malloc(bufsize * sizeof(unsigned char));
        fread(buffer, 1, bufsize, fp);
        /* close the file pointer */
        fclose(fp);

        unsigned int components  = (fourCC == FOURCC_DXT1) ? 3 : 4;
        unsigned int format;
        switch(fourCC)
        {
        case FOURCC_DXT1:
            format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
            break;
        case FOURCC_DXT3:
            format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
            break;
        case FOURCC_DXT5:
            format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
            break;
        default:
            free(buffer);
            return 0;
        }

        // Create one OpenGL texture
        GLuint textureID;
        glGenTextures(1, &textureID);

        // "Bind" the newly created texture : all future texture functions will modify this texture
        glBindTexture(GL_TEXTURE_2D, textureID);
        glPixelStorei(GL_UNPACK_ALIGNMENT,1);

        unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
        unsigned int offset = 0;

        /* load the mipmaps */
        for (unsigned int level = 0; level < mipMapCount && (width || height); ++level)
        {
            unsigned int size = ((width+3)/4)*((height+3)/4)*blockSize;
            glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height,
                0, size, buffer + offset);

            offset += size;
            width  /= 2;
            height /= 2;

            // Deal with Non-Power-Of-Two textures.
            if(width < 1) width = 1;
            if(height < 1) height = 1;

        }

        free(buffer);

        return textureID;
    }

private:
    const char* m_fileName;
    GLenum m_textureTarget;
    GLuint m_textureObj;

};
#endif // TEXTURE_H
