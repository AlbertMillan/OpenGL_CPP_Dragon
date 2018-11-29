#ifndef MODEL_H
#define MODEL_H

//#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "App.h"
#include "ShaderUniformSetters.h"
#include "ATB.h"
#include "Classes/ogldev_math_3d.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

using namespace std;

unsigned int TextureFromFile(const char *path, const string &directory, bool gamma = false);

class Model
{
public:
    Model();

    ~Model();

    bool LoadMesh(const string& filename);

    void Render( Shader* shader );

    uint NumBones() const
    {
        return m_NumBones;
    }

    void BoneTransform( float TimeInSeconds, vector<Matrix4f>& Transforms);

    void AddToATB(TwBar* myBar);

    struct Texture
    {
        unsigned int id;
        string type;
        string path;
    };


private:
    #define INVALID_MATERIAL 0xFFFFFFFF
    #define NUM_BONES_PER_VERTEX 4

    struct BoneInfo
    {
        Matrix4f BoneOffset;
        Matrix4f FinalTransformation;

        BoneInfo()
        {
            BoneOffset.SetZero();
            FinalTransformation.SetZero();
        }
    };

    struct VertexBoneData
    {
        uint IDs[NUM_BONES_PER_VERTEX];
        float Weights[NUM_BONES_PER_VERTEX];

        VertexBoneData()
        {
            Reset();
        };

        void Reset()
        {
            ZERO_MEM(IDs);
            ZERO_MEM(Weights);
        }

        void AddBoneData(uint BoneID, float Weight);
    };



    struct MeshEntry {
        MeshEntry()
        {
            NumIndices    = 0;
            BaseVertex    = 0;
            BaseIndex     = 0;
            MaterialIndex = INVALID_MATERIAL;
        }

        unsigned int NumIndices;
        unsigned int BaseVertex;
        unsigned int BaseIndex;
        unsigned int MaterialIndex;
    };

    void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
    void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
    void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
    uint FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
    uint FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
    uint FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
    const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const string NodeName);
    void ReadNodeHierarchy(float AnimationTime, const aiNode* pNode, const Matrix4f& ParentTransform);
    bool InitFromScene(const aiScene* pScene, const string& Filename);
    void InitMesh(uint MeshIndex,
                  const aiMesh* paiMesh,
                  vector<Vector3f>& Positions,
                  vector<Vector3f>& Normals,
                  vector<Vector2f>& TexCoords,
                  vector<VertexBoneData>& Bones,
                  vector<unsigned int>& Indices);
    void LoadBones(uint MeshIndex, const aiMesh* paiMesh, vector<VertexBoneData>& Bones);
    bool InitMaterials(const aiScene* pScene, const string& Filename);
    vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);
    void Clear();


    enum VB_TYPES {
        INDEX_BUFFER,
        POS_VB,
        NORMAL_VB,
        TEXCOORD_VB,
        BONE_VB,
        NUM_VBs
    };

    GLuint m_VAO;
    GLuint m_Buffers[NUM_VBs];

    vector<MeshEntry> m_Entries;
    vector<Texture> Textures;

    map<string,uint> m_BoneMapping;     // maps a bone name to its index
    uint m_NumBones;
    vector<BoneInfo> m_BoneInfo;
    Matrix4f m_GlobalInverseTransform;

    const aiScene* scene;
    Assimp::Importer importer;


};
#endif





















    /* Model Data */
//    vector<Texture> textures_loaded;
//    vector<Mesh> meshes;
//    string directory;
//    bool gammaCorrection;
//
//    /* Functions */
//    // constructor, expects a filepath to a 3D model
//    Model(string const &path, bool gamma = false) : gammaCorrection(gamma)
//    {
//        loadModel(path);
//    }
//
//    // draws the model, and thus all its meshes
//    void Draw ( Shader *shader )
//    {
//        for( unsigned int i = 0; i < meshes.size(); i++ )
//        {
//            meshes[i].draw(shader);
//        }
//    }
//
//    /*###########################################################################################*/
//    void BoneTransform(float TimeInSeconds, vector<Matrix4f>& Transforms)
//    {
//        Matrix4f Identity;
//        Identity.InitIdentity();
//        float TicksPerSecond = 0.0f;
//        //printf(" %d", scene->mAnimations[0]->mTicksPerSecond);
//
//        if (scene->mAnimations[0]->mTicksPerSecond != 0.0f)
//        {
//            TicksPerSecond = scene->mAnimations[0]->mTicksPerSecond;
//        }
//        else
//        {
//            TicksPerSecond = 25.0f;
//        }
//
//        //float TicksPerSecond = (float)(scene->mAnimations[0]->mTicksPerSecond != 0.0f ? scene->mAnimations[0]->mTicksPerSecond:25.0f);
//        float TimeInTicks = TimeInSeconds * TicksPerSecond;
//        float AnimationTime = fmod(TimeInTicks, (float)scene->mAnimations[0]->mDuration);
//
//        ReadNodeHierarchy(AnimationTime, scene->mRootNode, Identity);
//
//        Transforms.resize(m_NumBones);
//
//        for( uint i = 0; i < m_NumBones; i++ )
//        {
//            Transforms[i] = m_BoneInfo[i].FinalTransformation;
//        }
//    }
//    /*###########################################################################################*/
//
//private:
//    /* Data */
//    Matrix4f m_GlobalInverseTransform;
//    const aiScene* scene;
//    Assimp::Importer importer;
//    map<string,uint> m_BoneMapping;
//    uint m_NumBones = 0;
//    vector<BoneInfo> m_BoneInfo;
//
//    /* Functions */
//    // loads model with supported ASSIMP extensions from file and stores the resulting mehses in the meshes vector.
//    void loadModel (string const &path)
//    {
//        //const aiScene* scene = aiImportFile( file_name, aiProcess_Triangulate | aiProcess_PreTransformVertices );
//        scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
//
//        //bool Ret = false;
//        if (scene)
//        {
//            // Extract, inverse and store transformation matrix of the root of the hierarchy.
//            m_GlobalInverseTransform = scene->mRootNode->mTransformation;
//            m_GlobalInverseTransform.Inverse();
//            //Ret = InitFromScene(scene, path);
//        }
//
//        else
//        {
//            fprintf(stderr, "ERROR: reading mesh %s\n", path);
//            exit(0);
//        }
//
//        printf("  %i materials\n", scene->mNumMaterials);
//        printf("  %i meshes\n", scene->mNumMeshes);
//        printf("  %i textures\n", scene->mNumTextures);
//
//        // retrieve the directory path of the filepath
//        directory = path.substr(0, path.find_last_of('/'));
//
//        // process ASSIMP's root node recursively
//        processNode(scene->mRootNode, scene);
//    }
//
//    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
//    void processNode(aiNode *node, const aiScene *scene)
//    {
//        // process each mesh located at the current node
//        for(unsigned int i = 0; i < node->mNumMeshes; i++)
//        {
//            // the node object only contains indices to index the actual objects in the scene.
//            // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
//            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
//            printf("    %i vertices in mesh\n", mesh->mNumVertices);
//            printf("    %i bones in mesh\n", mesh->mNumBones);
//            printf("    %i faces in mesh\n", mesh->mNumFaces);
//            //printf("    %i bones in mesh\n", mesh->mBones);
//            meshes.push_back(processMesh(mesh, scene));
//        }
//        // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
//        for(unsigned int i = 0; i < node->mNumChildren; i++)
//        {
//            processNode(node->mChildren[i], scene);
//        }
//
//    }
//
//    // ERROR - MODELDATA VS VERTEX, TEXTURE, ETC.
//    Mesh processMesh(aiMesh *mesh, const aiScene *myScene)
//    {
//        // data to fill
//        vector<Vertex> vertices;
//        vector<unsigned int> indices;
//        vector<Texture> textures;
//        vector<VertexBoneData> bones;
//
//        bones.resize(mesh->mNumVertices);
//        // Iterate through each of the mesh's vertices
//        for( unsigned int i = 0; i < mesh->mNumVertices; i++ )
//        {
//            Vertex vertex;
//            glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
//            // positions
//            vector.x = mesh->mVertices[i].x;
//            vector.y = mesh->mVertices[i].y;
//            vector.z = mesh->mVertices[i].z;
//            vertex.Position = vector;
//            // normals
//            vector.x = mesh->mNormals[i].x;
//            vector.y = mesh->mNormals[i].y;
//            vector.z = mesh->mNormals[i].z;
//            vertex.Normal = vector;
//            // texture coordinates
//            if(mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
//            {
//                glm::vec2 vec;
//                // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
//                // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
//                vec.x = mesh->mTextureCoords[0][i].x;
//                vec.y = mesh->mTextureCoords[0][i].y;
//                vertex.TexCoords = vec;
//            }
//            else
//            {
//                vertex.TexCoords = glm::vec2(0.0f, 0.0f);
//            }
//            // tangent
////            vector.x = mesh->mTangents[i].x;
////            vector.y = mesh->mTangents[i].y;
////            vector.z = mesh->mTangents[i].z;
////            vertex.Tangent = vector;
////            // bitangent
////            vector.x = mesh->mBitangents[i].x;
////            vector.y = mesh->mBitangents[i].y;
////            vector.z = mesh->mBitangents[i].z;
////            vertex.Bitangent = vector;
//            vertices.push_back(vertex);
//        }
//
//        for (uint i = 0 ; i < mesh->mNumBones; i++)
//        {
//            uint BoneIndex = 0;
//            string BoneName(mesh->mBones[i]->mName.data);
//
//            // Create new bone if name does not exist.
//            if (m_BoneMapping.find(BoneName) == m_BoneMapping.end() )
//            {
//                // Allocate an index for a new bone.
//                BoneIndex = m_NumBones;
//                m_NumBones++;
//                BoneInfo bi;
//                m_BoneInfo.push_back(bi);
//                m_BoneInfo[BoneIndex].BoneOffset = mesh->mBones[i]->mOffsetMatrix;
//                m_BoneMapping[BoneName] = BoneIndex;        // value boneName has key BoneIndex
//            }
//            else
//            {
//                BoneIndex = m_BoneMapping[BoneName];
//            }
//// Quiza hay que revisar esto para que funcione.
//            for (uint j = 0; j < mesh->mBones[i]->mNumWeights; j++)
//            {
//                uint VertexID = /* mesh->mNumVertices */0 + mesh->mBones[i]->mWeights[j].mVertexId;
//                float Weight = mesh->mBones[i]->mWeights[j].mWeight;
//                // Ojo, Saber si esto corresponde a 1 vt o a varios...
//                // vertex.Bones[VertexID]->AddBoneData(BoneIndex, Weight);
//                //printf("%d\n", j);
//                bones[VertexID].AddBoneData(BoneIndex, Weight);
//            }
//        }
//
//        // Iterate through each of the mesh's faces. Retrieve corresponding vertex indices.
//        for( unsigned int i = 0; i < mesh->mNumFaces; i++ )
//        {
//            aiFace face = mesh->mFaces[i];
//            // retrieve all indices of the face and store them in the indices vector
//            for(unsigned int j = 0; j < face.mNumIndices; j++)
//                indices.push_back(face.mIndices[j]);
//        }
//
//        // process materials
//        aiMaterial* material = myScene->mMaterials[mesh->mMaterialIndex];
//        // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
//        // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
//        // Same applies to other texture as the following list summarizes:
//        // diffuse: texture_diffuseN
//        // specular: texture_specularN
//        // normal: texture_normalN
//
//        // 1. diffuse maps
//        vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
//        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
//        // 2. specular maps
//        vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
//        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
//        // 3. normal maps
//        std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
//        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
//        // 4. height maps
//        std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
//        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
//
//        // return a mesh object created from the extracted mesh data
//        return Mesh(vertices, indices, textures, bones);
//    }
//
//    // checks all material textures of a given type and loads the textures if they're not loaded yet.
//    // the required info is returned as a Texture struct.
//    vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
//    {
//        vector<Texture> textures;
//        for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
//        {
//            aiString str;
//            mat->GetTexture(type, i, &str);
//            // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
//            bool skip = false;
//            for(unsigned int j = 0; j < textures_loaded.size(); j++)
//            {
//                if(std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
//                {
//                    textures.push_back(textures_loaded[j]);
//                    skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
//                    break;
//                }
//            }
//            if(!skip)
//            {   // if texture hasn't been loaded already, load it
//                Texture texture;
//                texture.id = TextureFromFile(str.C_Str(), this->directory);
//                texture.type = typeName;
//                texture.path = str.C_Str();
//                textures.push_back(texture);
//                textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
//            }
//        }
//        return textures;
//    }
//
//    /*###########################################################################################*/
//    void ReadNodeHierarchy (float AnimationTime, const aiNode* rNode, const Matrix4f& ParentTransform)
//    {
//        string NodeName(rNode->mName.data);
//
//        const aiAnimation* pAnimation = scene->mAnimations[0];
//
//        Matrix4f NodeTransformation(rNode->mTransformation);
//
//        const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);
//
//        if ( pNodeAnim )
//        {
//            // Interpolate scaling and generate scaling transformation matrix
//            aiVector3D Scaling;
//            CalcInterpolatedScaling( Scaling, AnimationTime, pNodeAnim );
//            Matrix4f ScalingM;
//            ScalingM.InitScaleTransform(Scaling.x, Scaling.y, Scaling.z);
//
//            // Interpolate rotation and generate rotation transformation matrix
//            aiQuaternion RotationQ;
//            CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);
//            Matrix4f RotationM = Matrix4f(RotationQ.GetMatrix());
//
//            // Interpolate translation and generate translation transformation matrix
//            aiVector3D Translation;
//            CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
//            Matrix4f TranslationM;
//            TranslationM.InitTranslationTransform(Translation.x, Translation.y, Translation.z);
//
//            // Combine the above transformations
//            NodeTransformation = TranslationM * RotationM * ScalingM;
//        }
//
//        Matrix4f GlobalTransformation = ParentTransform * NodeTransformation;
//
//        // Check if there is only 1 bone???
//        if( m_BoneMapping.find(NodeName) != m_BoneMapping.end() )
//        {
//            uint BoneIndex = m_BoneMapping[NodeName];
//            m_BoneInfo[BoneIndex].FinalTransformation = m_GlobalInverseTransform * GlobalTransformation * m_BoneInfo[BoneIndex].BoneOffset;
//        }
//
//        for(uint i = 0; i < rNode->mNumChildren; i++ )
//        {
//            ReadNodeHierarchy(AnimationTime, rNode->mChildren[i], GlobalTransformation);
//        }
//    }
//
//    void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
//    {
//        if( pNodeAnim->mNumScalingKeys == 1)
//        {
//            Out = pNodeAnim->mScalingKeys[0].mValue;
//            return;
//        }
//
//        uint ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
//        uint NextScalingIndex = (ScalingIndex + 1);
//        assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
//        float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
//        float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
//        assert(Factor >= 0.0f && Factor <= 1.0f);
//        const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
//        const aiVector3D& End   = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
//        aiVector3D Delta = End - Start;
//        Out = Start + Factor * Delta;
//    }
//
//    uint FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
//    {
//        assert(pNodeAnim->mNumScalingKeys > 0);
//
//        for (uint i = 0 ; i < pNodeAnim->mNumScalingKeys - 1 ; i++) {
//            if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) {
//                return i;
//            }
//        }
//
//        assert(0);
//
//        return 0;
//    }
//
//    void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
//    {
//        // we need at least two values to interpolate...
//        if (pNodeAnim->mNumRotationKeys == 1) {
//            Out = pNodeAnim->mRotationKeys[0].mValue;
//            return;
//        }
//
//        uint RotationIndex = FindRotation(AnimationTime, pNodeAnim);
//        uint NextRotationIndex = (RotationIndex + 1);
//        assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
//        float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
//        float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
//        assert(Factor >= 0.0f && Factor <= 1.0f);
//        const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
//        const aiQuaternion& EndRotationQ   = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
//        aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
//        Out = Out.Normalize();
//    }
//
//    uint FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
//    {
//        assert(pNodeAnim->mNumRotationKeys > 0);
//
//        for (uint i = 0 ; i < pNodeAnim->mNumRotationKeys - 1 ; i++) {
//            if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
//                return i;
//            }
//        }
//
//        assert(0);
//
//        return 0;
//    }
//
//    void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
//    {
//        if (pNodeAnim->mNumPositionKeys == 1) {
//            Out = pNodeAnim->mPositionKeys[0].mValue;
//            return;
//        }
//
//        uint PositionIndex = FindPosition(AnimationTime, pNodeAnim);
//        uint NextPositionIndex = (PositionIndex + 1);
//        assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
//        float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
//        float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
//        assert(Factor >= 0.0f && Factor <= 1.0f);
//        const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
//        const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
//        aiVector3D Delta = End - Start;
//        Out = Start + Factor * Delta;
//    }
//
//    uint FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
//    {
//        for (uint i = 0 ; i < pNodeAnim->mNumPositionKeys - 1 ; i++) {
//            if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
//                return i;
//            }
//        }
//
//        assert(0);
//
//        return 0;
//    }
//
//    const aiNodeAnim* FindNodeAnim( const aiAnimation* pAnimation, const string NodeName )
//    {
//        for( uint i = 0; i < pAnimation->mNumChannels; i++ )
//        {
//            const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];
//
//            if( string(pNodeAnim->mNodeName.data) == NodeName )
//            {
//                return pNodeAnim;
//            }
//        }
//
//        return NULL;
//    }
//    /*###########################################################################################*/
//};
//
//unsigned int TextureFromFile(const char *path, const string &directory, bool gamma)
//{
//    string filename = string(path);
//    //filename = directory + '/' + filename;
//
//    unsigned int textureID;
//    glGenTextures(1, &textureID);
//
//    int width, height, nrComponents;
//    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
//    if (data)
//    {
//        GLenum format;
//        if (nrComponents == 1)
//            format = GL_RED;
//        else if (nrComponents == 3)
//            format = GL_RGB;
//        else if (nrComponents == 4)
//            format = GL_RGBA;
//
//        glBindTexture(GL_TEXTURE_2D, textureID);
//        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
//        glGenerateMipmap(GL_TEXTURE_2D);
//
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//        stbi_image_free(data);
//    }
//    else
//    {
//        std::cout << "Texture failed to load at path: " << path << std::endl;
//        stbi_image_free(data);
//    }
//
//    return textureID;
