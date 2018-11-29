#include "Model.h"
#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define POSITION_LOCATION    0
#define NORMAL_LOCATION      1
#define TEX_COORD_LOCATION   2
#define BONE_ID_LOCATION     3
#define BONE_WEIGHT_LOCATION 4


using namespace std;

void Model::VertexBoneData::AddBoneData(uint BoneID, float Weight)
{
    for (uint i = 0 ; i < ARRAY_SIZE_IN_ELEMENTS(IDs) ; i++) {
        if (Weights[i] == 0.0) {
            IDs[i]     = BoneID;
            Weights[i] = Weight;
            return;
        }
    }
    // should never get here - more bones than we have space for
    assert(0);
}


Model::Model()
{
    m_VAO = 0;
    ZERO_MEM(m_Buffers);
    m_NumBones = 0;
    scene = NULL;
}


Model::~Model()
{
    Clear();
}

void Model::Clear()
{
//    for( uint i = 0; i < Textures.size(); i++ )
//    {
//        SAFE_DELETE(Textures[i]);
//    }

    if (m_Buffers[0] != 0) {
        glDeleteBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);
    }

    if (m_VAO != 0) {
        glDeleteVertexArrays(1, &m_VAO);
        m_VAO = 0;
    }
}


bool Model::LoadMesh(const string& filename)
{
    // Release the previously loaded mesh (if any)
    Clear();

    // Create the VAO
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    //directory = filename.substr(0, filename.find_last_of('/'));

    // Create the buffers for the vertices attributes
    glGenBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);

    bool Ret = false;

    scene = importer.ReadFile(filename.c_str(), ASSIMP_LOAD_FLAGS);
    //scene =importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    if( scene )
    {
        printf("  %i materials\n", scene->mNumMaterials);
        printf("  %i meshes\n", scene->mNumMeshes);
        printf("  %i textures\n", scene->mNumTextures);

        m_GlobalInverseTransform = scene->mRootNode->mTransformation;
        m_GlobalInverseTransform.Inverse();
        // Mesh Initializer Method
        Ret = InitFromScene(scene, filename);
    }
    else {
        printf("Error parsing '%s': '%s'\n", filename.c_str(), importer.GetErrorString());
    }

    // Make sure the VAO is not changed from the outside
    glBindVertexArray(0);

    return Ret;
}


bool Model::InitFromScene( const aiScene* scene, const string& filename )
{
    m_Entries.resize(scene->mNumMeshes);
    //Textures.resize(scene->mNumMaterials);

    vector<Vector3f> Positions;
    vector<Vector3f> Normals;
    vector<Vector2f> TexCoords;
    vector<VertexBoneData> Bones;
    vector<uint> Indices;

    uint NumVertices = 0;
    uint NumIndices = 0;

    // Count the number of vertices and indices. Loops through every mesh, and adds the values to a total counter
    for (uint i = 0 ; i < m_Entries.size() ; i++) {
        m_Entries[i].MaterialIndex = scene->mMeshes[i]->mMaterialIndex;
        m_Entries[i].NumIndices    = scene->mMeshes[i]->mNumFaces * 3;
        m_Entries[i].BaseVertex    = NumVertices;
        m_Entries[i].BaseIndex     = NumIndices;

        NumVertices += scene->mMeshes[i]->mNumVertices;
        NumIndices  += m_Entries[i].NumIndices;
    }


    // Reserve space in the vectors for the vertex attributes and indices
    Positions.reserve(NumVertices);
    Normals.reserve(NumVertices);
    TexCoords.reserve(NumVertices);
    Bones.resize(NumVertices);
    Indices.reserve(NumIndices);

    // Initialize the meshes in the scene one by one
    for (uint i = 0; i < m_Entries.size(); i++ )
    {
        const aiMesh* mesh = scene->mMeshes[i];
        printf("    %i vertices in mesh\n", mesh->mNumVertices);
        printf("    %i bones in mesh\n", mesh->mNumBones);
        printf("    %i faces in mesh\n", mesh->mNumFaces);
        InitMesh(i, mesh, Positions, Normals, TexCoords, Bones, Indices);
    }


    // Generate and populate the buffers with vertex attributes and the indices
  	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POS_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Positions[0]) * Positions.size(), &Positions[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(POSITION_LOCATION);
    glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[TEXCOORD_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexCoords[0]) * TexCoords.size(), &TexCoords[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(TEX_COORD_LOCATION);
    glVertexAttribPointer(TEX_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE, 0, 0);

   	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[NORMAL_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Normals[0]) * Normals.size(), &Normals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(NORMAL_LOCATION);
    glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

   	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[BONE_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Bones[0]) * Bones.size(), &Bones[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(BONE_ID_LOCATION);
    glVertexAttribIPointer(BONE_ID_LOCATION, 4, GL_INT, sizeof(VertexBoneData), (const GLvoid*)0);
    glEnableVertexAttribArray(BONE_WEIGHT_LOCATION);
    glVertexAttribPointer(BONE_WEIGHT_LOCATION, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (const GLvoid*)16);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[INDEX_BUFFER]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices[0]) * Indices.size(), &Indices[0], GL_STATIC_DRAW);

	return true;
}

vector<Model::Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
{
    vector<Texture> textures;
    vector<Texture> textures_loaded;
    for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
        bool skip = false;
        for(unsigned int j = 0; j < textures_loaded.size(); j++)
        {
            if(std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
            {
                textures.push_back(textures_loaded[j]);
                skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                break;
            }
        }
        if(!skip)
        {   // if texture hasn't been loaded already, load it
            Texture texture;
            texture.id = TextureLoader::LoadTexture(str.C_Str());
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
        }
    }
    return textures;
}



void Model::InitMesh(uint MeshIndex, const aiMesh* mesh, vector<Vector3f>& Positions, vector<Vector3f>& Normals,
                     vector<Vector2f>& TexCoords, vector<VertexBoneData>& Bones, vector<uint>& Indices)
{
    //const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

    // Populate the vertex attribute vectors
    for( uint i = 0; i < mesh->mNumVertices; i++ )
    {
        const aiVector3D* pPos      = &(mesh->mVertices[i]);
        const aiVector3D* pNormal   = &(mesh->mNormals[i]);

        Positions.push_back(Vector3f(pPos->x, pPos->y, pPos->z));
        Normals.push_back(Vector3f(pNormal->x, pNormal->y, pNormal->z));

        if( mesh->HasTextureCoords(0) )
        {
            const aiVector3D* pTexCoord = &(mesh->mTextureCoords[0][i]);
            TexCoords.push_back(Vector2f(pTexCoord->x, pTexCoord->y));
        }
    }

    LoadBones(MeshIndex, mesh, Bones);

    // Populate the index buffer
    for (uint i = 0 ; i < mesh->mNumFaces ; i++) {
        const aiFace& Face = mesh->mFaces[i];
        assert(Face.mNumIndices == 3);
        Indices.push_back(Face.mIndices[0]);
        Indices.push_back(Face.mIndices[1]);
        Indices.push_back(Face.mIndices[2]);
    }

    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
     //1. diffuse maps
    vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    Textures.insert(Textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    // 2. specular maps
    vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
    Textures.insert(Textures.end(), specularMaps.begin(), specularMaps.end());
    // 3. normal maps
    std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
    Textures.insert(Textures.end(), normalMaps.begin(), normalMaps.end());
    // 4. height maps
    std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
    Textures.insert(Textures.end(), heightMaps.begin(), heightMaps.end());
}

void Model::LoadBones( uint MeshIndex, const aiMesh* mesh, vector<VertexBoneData>& Bones  )
{
    for (uint i = 0 ; i < mesh->mNumBones; i++)
    {
        uint BoneIndex = 0;
        string BoneName(mesh->mBones[i]->mName.data);

        // Create new bone if name does not exist.
        if (m_BoneMapping.find(BoneName) == m_BoneMapping.end() )
        {
            // Allocate an index for a new bone.
            BoneIndex = m_NumBones;
            m_NumBones++;
            BoneInfo bi;
            m_BoneInfo.push_back(bi);
            m_BoneInfo[BoneIndex].BoneOffset = mesh->mBones[i]->mOffsetMatrix;
            m_BoneMapping[BoneName] = BoneIndex;        // value boneName has key BoneIndex
        }
        else
        {
            BoneIndex = m_BoneMapping[BoneName];
        }

        for (uint j = 0; j < mesh->mBones[i]->mNumWeights; j++)
        {
            uint VertexID = m_Entries[MeshIndex].BaseVertex + mesh->mBones[i]->mWeights[j].mVertexId;
            float Weight = mesh->mBones[i]->mWeights[j].mWeight;
            Bones[VertexID].AddBoneData(BoneIndex, Weight);
        }
    }
}


void Model::Render(Shader* shader)
{
    unsigned int diffuseNr  = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr   = 1;
    unsigned int heightNr   = 1;
    for(unsigned int i = 0; i < Textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
        // retrieve texture number (the N in diffuse_textureN)
        string number;
        string name = Textures[i].type;
        if(name == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if(name == "texture_specular")
			number = std::to_string(specularNr++); // transfer unsigned int to stream
        else if(name == "texture_normal")
			number = std::to_string(normalNr++); // transfer unsigned int to stream
         else if(name == "texture_height")
		    number = std::to_string(heightNr++); // transfer unsigned int to stream

                                                // now set the sampler to the correct texture unit
        glUniform1i(glGetUniformLocation(shader->ID, (name + number).c_str()), i);
        // and finally bind the texture
        glBindTexture(GL_TEXTURE_2D, Textures[i].id);
    }

    glBindVertexArray(m_VAO);
    glDrawElementsBaseVertex(GL_TRIANGLES,
                                 m_Entries[0].NumIndices,
                                 GL_UNSIGNED_INT,
                                 (void*)(sizeof(uint) * m_Entries[0].BaseIndex),
                                 m_Entries[0].BaseVertex);

    glBindVertexArray(0);
}


uint Model::FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    for (uint i = 0 ; i < pNodeAnim->mNumPositionKeys - 1 ; i++) {
        if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
            return i;
        }
    }

    assert(0);

    return 0;
}


uint Model::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    assert(pNodeAnim->mNumRotationKeys > 0);

    for (uint i = 0 ; i < pNodeAnim->mNumRotationKeys - 1 ; i++) {
        if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
            return i;
        }
    }

    assert(0);

    return 0;
}


uint Model::FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    assert(pNodeAnim->mNumScalingKeys > 0);

    for (uint i = 0 ; i < pNodeAnim->mNumScalingKeys - 1 ; i++) {
        if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) {
            return i;
        }
    }

    assert(0);

    return 0;
}


void Model::CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    if (pNodeAnim->mNumPositionKeys == 1) {
        Out = pNodeAnim->mPositionKeys[0].mValue;
        return;
    }

    uint PositionIndex = FindPosition(AnimationTime, pNodeAnim);
    uint NextPositionIndex = (PositionIndex + 1);
    assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
    float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
    float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
    assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
    const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
    aiVector3D Delta = End - Start;
    Out = Start + Factor * Delta;
}


void Model::CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	// we need at least two values to interpolate...
    if (pNodeAnim->mNumRotationKeys == 1) {
        Out = pNodeAnim->mRotationKeys[0].mValue;
        return;
    }

    uint RotationIndex = FindRotation(AnimationTime, pNodeAnim);
    uint NextRotationIndex = (RotationIndex + 1);
    assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
    float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
    float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
    assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
    const aiQuaternion& EndRotationQ   = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
    aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
    Out = Out.Normalize();
}


void Model::CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    if (pNodeAnim->mNumScalingKeys == 1) {
        Out = pNodeAnim->mScalingKeys[0].mValue;
        return;
    }

    uint ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
    uint NextScalingIndex = (ScalingIndex + 1);
    assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
    float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
    float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
    assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
    const aiVector3D& End   = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
    aiVector3D Delta = End - Start;
    Out = Start + Factor * Delta;
}


void Model::ReadNodeHierarchy(float AnimationTime, const aiNode* pNode, const Matrix4f& ParentTransform)
{
    string NodeName(pNode->mName.data);

    const aiAnimation* pAnimation = scene->mAnimations[0];

    Matrix4f NodeTransformation(pNode->mTransformation);

    const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);

    if (pNodeAnim) {
        // Interpolate scaling and generate scaling transformation matrix
        aiVector3D Scaling;
        CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
        Matrix4f ScalingM;
        ScalingM.InitScaleTransform(Scaling.x, Scaling.y, Scaling.z);

        // Interpolate rotation and generate rotation transformation matrix
        aiQuaternion RotationQ;
        CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);
        Matrix4f RotationM = Matrix4f(RotationQ.GetMatrix());

        // Interpolate translation and generate translation transformation matrix
        aiVector3D Translation;
        CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
        Matrix4f TranslationM;
        TranslationM.InitTranslationTransform(Translation.x, Translation.y, Translation.z);

        // Combine the above transformations
        NodeTransformation = TranslationM * RotationM * ScalingM;
    }

    Matrix4f GlobalTransformation = ParentTransform * NodeTransformation;

    if (m_BoneMapping.find(NodeName) != m_BoneMapping.end()) {
        uint BoneIndex = m_BoneMapping[NodeName];
        m_BoneInfo[BoneIndex].FinalTransformation = m_GlobalInverseTransform * GlobalTransformation * m_BoneInfo[BoneIndex].BoneOffset;
    }

    for (uint i = 0 ; i < pNode->mNumChildren ; i++) {
        ReadNodeHierarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation);
    }
}


void Model::BoneTransform(float TimeInSeconds, vector<Matrix4f>& Transforms)
{
    Matrix4f Identity;
    Identity.InitIdentity();
    float TicksPerSecond = 0.0f;
    //printf(" %d", scene->mAnimations[0]->mTicksPerSecond);

    if (scene->mAnimations[0]->mTicksPerSecond != 0.0f)
    {
        TicksPerSecond = scene->mAnimations[0]->mTicksPerSecond;
    }
    else
    {
        TicksPerSecond = 25.0f;
    }

    float TimeInTicks = TimeInSeconds * TicksPerSecond;
    float AnimationTime = fmod(TimeInTicks, (float)scene->mAnimations[0]->mDuration);

    ReadNodeHierarchy(AnimationTime, scene->mRootNode, Identity);

    Transforms.resize(m_NumBones);

    for( uint i = 0; i < m_NumBones; i++ )
    {
        Transforms[i] = m_BoneInfo[i].FinalTransformation;
    }
}


const aiNodeAnim* Model::FindNodeAnim(const aiAnimation* pAnimation, const string NodeName)
{
    for (uint i = 0 ; i < pAnimation->mNumChannels ; i++) {
        const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

        if (string(pNodeAnim->mNodeName.data) == NodeName) {
            return pNodeAnim;
        }
    }

    return NULL;
}


void Model::AddToATB(TwBar* myBar)
{
    //Vector3f data = Vector3f( scene->mMeshes[0]->mNumVertices, scene->mMeshes[0]->mNumFaces, scene->mMeshes[0]->mNumBones);
    //TwAddVarRO(myBar, "Vertices", TW_TYPE_VECTOR3I, &data, NULL);
    TwAddVarRO(myBar, "Vertices", TW_TYPE_INT32, &scene->mMeshes[0]->mNumVertices, NULL);
    TwAddVarRO(myBar, "Faces", TW_TYPE_INT32, &scene->mMeshes[0]->mNumFaces, NULL);
    TwAddVarRO(myBar, "Bones", TW_TYPE_INT32, &scene->mMeshes[0]->mNumBones, NULL);
    TwAddSeparator(myBar, "", NULL);
    //TwAddVarRO(myBar, "Ticks", TW_TYPE_DOUBLE, &scene->mAnimations[0]->mDuration, NULL);
    //TwAddVarRO(myBar, "Ticks/Second", TW_TYPE_DOUBLE, &scene->mAnimations[0]->mTicksPerSecond, NULL);

    //printf("%d\n ", scene->mAnimations[0]->mDuration);
    //printf("%d\n ", scene->mAnimations[0]->mTicksPerSecond);
}

