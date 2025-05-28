#include "assimp_helper.h"
#include "../../assets/assets.h"

#include <glm/gtc/type_ptr.hpp>

namespace ae {

bool AssimpHelper::load(Model *model)
{
    if (path.empty())
        return false;

    ai_scene = importer.ReadFile(path.string(),
                                 aiProcess_Triangulate | aiProcess_GenSmoothNormals
                                     | aiProcess_FlipUVs | aiProcess_CalcTangentSpace
                                 /*| aiProcess_JoinIdenticalVertices*/ /* | aiProcess_OptimizeMeshes
                                     | aiProcess_SortByPType*/
                                 /*| aiProcess_PreTransformVertices*/);

    if (!ai_scene || ai_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !ai_scene->mRootNode) {
        spdlog::error("Assimp: {}", importer.GetErrorString());
        return false;
    }

    if (model) {
        auto skeleton = buildSkeleton();
        auto root_node = processRootNode();
        model->setRootNode(root_node);
        if (skeleton->getBoneCount() != 0)
            model->setSkeleton(skeleton);

        std::vector<SharedPtr<PoseAnimation>> animations;
        for (int32_t i = 0; i < ai_scene->mNumAnimations; ++i)
            animations.push_back(processAnimation(ai_scene->mAnimations[i]));
        model->setAnimations(animations);
    }

    return true;
}

SharedPtr<MeshNode> AssimpHelper::processNode(const aiNode *ai_node)
{
    if (!ai_node)
        return nullptr;

    std::vector<SharedPtr<MeshNode>> child_nodes;
    std::vector<SharedPtr<Mesh>> meshes;

    // std::string node_name = ai_node->mName.C_Str();
    // spdlog::debug("Node: {}", node_name);

    // Meshes
    for (int32_t i = 0; i < ai_node->mNumMeshes; ++i) {
        auto mesh = processMesh(ai_scene->mMeshes[ai_node->mMeshes[i]]);
        if (mesh)
            meshes.push_back(mesh);
    }

    // Child nodes
    for (int32_t i = 0; i < ai_node->mNumChildren; i++) {
        auto child_mesh_node = processNode(ai_node->mChildren[i]);
        if (child_mesh_node)
            child_nodes.push_back(child_mesh_node);
    }

    auto mesh_node = SharedPtr<MeshNode>::create();
    mesh_node->setChildren(child_nodes);
    mesh_node->setMeshes(meshes);
    mesh_node->setTransform(сonvertMatrixToGLM(ai_node->mTransformation));

    return mesh_node;
}

SharedPtr<Mesh> AssimpHelper::processMesh(const aiMesh *ai_mesh)
{
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    for (int32_t i = 0; i < ai_mesh->mNumVertices; ++i) {
        Vertex vertex;
        vec3 vector;

        vertex.position = сonvertVec3ToGLM(ai_mesh->mVertices[i]);

        if (ai_mesh->HasNormals())
            vertex.normal = сonvertVec3ToGLM(ai_mesh->mNormals[i]);

        // Texture coordinates
        if (ai_mesh->mTextureCoords[0]) {
            vertex.tex_coords = сonvertVec3ToGLM(ai_mesh->mTextureCoords[0][i]);

            // vector.x = mesh->mTangents[i].x;
            // vector.y = mesh->mTangents[i].y;
            // vector.z = mesh->mTangents[i].z;
            // vertex.tangent = vector;

            // vector.x = mesh->mBitangents[i].x;
            // vector.y = mesh->mBitangents[i].y;
            // vector.z = mesh->mBitangents[i].z;
            // vertex.bitangent = vector;
        } else
            vertex.tex_coords = vec2{0.0f};

        // Vertex colors
        if (ai_mesh->HasVertexColors(0)) {
            vec4 color{1.0f};
            color.r = ai_mesh->mColors[0][i].r;
            color.g = ai_mesh->mColors[0][i].g;
            color.b = ai_mesh->mColors[0][i].b;
            color.a = ai_mesh->mColors[0][i].a;

            vertex.color = color;
        } else
            vertex.color = vec4{1.0f};

        vertices.push_back(vertex);
    }

    // Indices
    for (int32_t i = 0; i < ai_mesh->mNumFaces; ++i) {
        aiFace face = ai_mesh->mFaces[i];
        for (int32_t j = 0; j < face.mNumIndices; ++j)
            indices.push_back(face.mIndices[j]);
    }

    // Extract bones
    extractBoneWeightForVertices(ai_mesh, vertices);

    auto mesh = SharedPtr<Mesh>::create();

    aiMaterial *ai_material = ai_scene->mMaterials[ai_mesh->mMaterialIndex];
    auto material = processMaterial(ai_material);

    mesh->create(vertices, indices, material);

    return mesh;
}

SharedPtr<Material> AssimpHelper::processMaterial(const aiMaterial *ai_material)
{
    auto material = SharedPtr<Material>::create();
    material->diffuse_texture = loadMaterialTexture(ai_material,
                                                    aiTextureType_DIFFUSE,
                                                    Texture::getDefaultDiffuseTexture());
    material->specular_texture = loadMaterialTexture(ai_material,
                                                     aiTextureType_SPECULAR,
                                                     Texture::getDefaultSpecularTexture());
    material->color = loadMaterialColor(ai_material, AI_MATKEY_COLOR_DIFFUSE);
    return material;
}

SharedPtr<Texture> AssimpHelper::loadMaterialTexture(const aiMaterial *ai_material,
                                                     aiTextureType type,
                                                     const SharedPtr<Texture> &default_texture)
{
    for (int32_t i = 0; i < ai_material->GetTextureCount(type); ++i) {
        aiString str;
        ai_material->GetTexture(type, i, &str);
        std::string texture_name{str.data, str.length};

        SharedPtr<Texture> texture;

        if (texture_name.starts_with('*')) {
            const aiTexture *ai_texture = ai_scene->GetEmbeddedTexture(texture_name.data());

            if (ai_texture) {
                texture = SharedPtr<Texture>::create();
                texture->loadFromMemory(reinterpret_cast<uint8_t *>(ai_texture->pcData),
                                        ai_texture->mHeight == 0
                                            ? ai_texture->mWidth
                                            : ai_texture->mWidth * ai_texture->mHeight);

                return texture;
            }
        }

        std::string texture_asset_name{
            (path.parent_path() / std::filesystem::path{texture_name}).string()};
        std::filesystem::path texture_path{path.parent_path().string() + "/" + texture_name};

        if (assets) {
            if (assets->has<Texture>(texture_asset_name))
                return assets->get<Texture>(texture_asset_name);

            texture = SharedPtr<Texture>::create();
            if (texture->loadFromFile(texture_path))
                assets->add(texture_asset_name, texture);

            return texture;
        } else {
            auto found_texture = loaded_textures.find(texture_asset_name);
            if (found_texture != loaded_textures.end())
                return found_texture->second;

            texture = SharedPtr<Texture>::create();
            if (texture->loadFromFile(texture_path))
                loaded_textures.emplace(texture_asset_name, texture);

            return texture;
        }
    }

    return default_texture;
}

Color AssimpHelper::loadMaterialColor(const aiMaterial *ai_material,
                                      const char *p_key,
                                      unsigned int type,
                                      unsigned int index)
{
    aiColor4D color;
    if (AI_SUCCESS != aiGetMaterialColor(ai_material, p_key, type, index, &color))
        return Color::white;
    return Color{color.r, color.g, color.b, color.a};
}

void AssimpHelper::setVertexBoneData(Vertex &vertex, int32_t bone_id, float weight)
{
    for (int32_t i = 0; i < 4; ++i) {
        if (vertex.bone_ids[i] < 0) {
            vertex.weights[i] = weight;
            vertex.bone_ids[i] = bone_id;
            break;
        }
    }
}

void AssimpHelper::extractBoneWeightForVertices(const aiMesh *mesh, std::vector<Vertex> &vertices)
{
    for (int32_t bone_index = 0; bone_index < mesh->mNumBones; ++bone_index) {
        int32_t bone_id = -1;
        std::string bone_name = mesh->mBones[bone_index]->mName.C_Str();

        if (!bone_map.contains(bone_name))
            continue;

        bone_id = bone_map[bone_name];

        auto weights = mesh->mBones[bone_index]->mWeights;
        int32_t num_weights = mesh->mBones[bone_index]->mNumWeights;

        for (int32_t weight_index = 0; weight_index < num_weights; ++weight_index) {
            int32_t vertex_id = weights[weight_index].mVertexId;
            float weight = weights[weight_index].mWeight;
            setVertexBoneData(vertices[vertex_id], bone_id, weight);
        }
    }
}

SharedPtr<Skeleton> AssimpHelper::buildSkeleton()
{
    bone_map.clear();

    auto skeleton = SharedPtr<Skeleton>::create();
    std::unordered_map<std::string, mat4> bone_offsets;

    for (int32_t i = 0; i < ai_scene->mNumMeshes; ++i) {
        aiMesh *mesh = ai_scene->mMeshes[i];

        for (int32_t j = 0; j < mesh->mNumBones; ++j) {
            aiBone *bone = mesh->mBones[j];
            std::string name = bone->mName.C_Str();
            // mat4 offset = сonvertMatrixToGLM(bone->mOffsetMatrix);
            mat4 offset = glm::transpose(glm::make_mat4(&bone->mOffsetMatrix.a1));
            bone_offsets[name] = offset;
        }
    }

    std::function<void(aiNode *, int32_t)> process_node = [&](aiNode *node, int32_t parent_index) {
        std::string node_name = node->mName.C_Str();

        int32_t node_index = skeleton->addNode(node_name, сonvertMatrixToGLM(node->mTransformation));

        if (bone_offsets.contains(node_name)) {
            int32_t bone_index = skeleton->setBone(node_index, bone_offsets[node_name]);
            bone_map.emplace(node_name, bone_index);
        }

        if (parent_index != -1)
            skeleton->setParent(node_index, parent_index);
        else
            skeleton->setRootIndex(node_index);

        for (unsigned int i = 0; i < node->mNumChildren; ++i)
            process_node(node->mChildren[i], node_index);
    };

    process_node(ai_scene->mRootNode, -1);

    return skeleton;
}

SharedPtr<PoseAnimation> AssimpHelper::processAnimation(const aiAnimation *ai_animation)
{
    auto animation = SharedPtr<PoseAnimation>::create();

    animation->setName(ai_animation->mName.C_Str());
    animation->setDuration(ai_animation->mDuration);
    animation->setTicksPerSecond(ai_animation->mTicksPerSecond);

    for (int32_t i = 0; i < ai_animation->mNumChannels; i++) {
        auto channel = ai_animation->mChannels[i];
        std::string bone_name = channel->mNodeName.data;

        PoseAnimation::Bone bone;

        for (int32_t i = 0; i < channel->mNumPositionKeys; ++i) {
            PoseAnimation::KeyPosition data;
            data.position = сonvertVec3ToGLM(channel->mPositionKeys[i].mValue);
            data.time = channel->mPositionKeys[i].mTime;
            bone.positions.push_back(data);
        }

        for (int32_t i = 0; i < channel->mNumRotationKeys; ++i) {
            PoseAnimation::KeyRotation data;
            data.rotation = glm::normalize(сonvertQuatToGLM(channel->mRotationKeys[i].mValue));
            data.time = channel->mRotationKeys[i].mTime;
            bone.rotations.push_back(data);
        }

        for (int keyIndex = 0; keyIndex < channel->mNumScalingKeys; ++keyIndex) {
            PoseAnimation::KeyScale data;
            data.scale = сonvertVec3ToGLM(channel->mScalingKeys[keyIndex].mValue);
            data.time = channel->mScalingKeys[keyIndex].mTime;
            bone.scales.push_back(data);
        }

        animation->addBone(bone_name, bone);
    }

    return animation;
}

mat4 AssimpHelper::сonvertMatrixToGLM(const aiMatrix4x4 &from)
{
    mat4 to;
    to[0][0] = from.a1;
    to[1][0] = from.a2;
    to[2][0] = from.a3;
    to[3][0] = from.a4;
    to[0][1] = from.b1;
    to[1][1] = from.b2;
    to[2][1] = from.b3;
    to[3][1] = from.b4;
    to[0][2] = from.c1;
    to[1][2] = from.c2;
    to[2][2] = from.c3;
    to[3][2] = from.c4;
    to[0][3] = from.d1;
    to[1][3] = from.d2;
    to[2][3] = from.d3;
    to[3][3] = from.d4;
    return to;
}

vec2 AssimpHelper::сonvertVec2ToGLM(const aiVector2D &v)
{
    return vec2{v.x, v.y};
}

vec3 AssimpHelper::сonvertVec3ToGLM(const aiVector3D &v)
{
    return vec3{v.x, v.y, v.z};
}

quat AssimpHelper::сonvertQuatToGLM(const aiQuaternion &q)
{
    return quat{q.w, q.x, q.y, q.z};
}

} // namespace ae
