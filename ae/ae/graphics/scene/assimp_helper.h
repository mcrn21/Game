#ifndef AE_ASSIMP_HELPER_H
#define AE_ASSIMP_HELPER_H

#include "../../system/memory.h"
#include "../core/texture.h"
#include "model.h"
#include "pose_animation.h"
#include "skeleton.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <spdlog/spdlog.h>

#include <filesystem>
#include <unordered_map>

namespace ae {

class Assets;

struct AssimpHelper
{
    AssimpHelper(const std::filesystem::path &path, Assets *assets = nullptr)
        : ai_scene{nullptr}
        , path{path}
        , assets{assets}
    {}
    ~AssimpHelper() = default;

    bool load(Model *model = nullptr);

    SharedPtr<MeshNode> processRootNode() { return processNode(ai_scene->mRootNode); }
    SharedPtr<MeshNode> processNode(const aiNode *ai_node);
    SharedPtr<Mesh> processMesh(const aiMesh *ai_mesh);
    SharedPtr<Material> processMaterial(const aiMaterial *ai_material);
    SharedPtr<Texture> loadMaterialTexture(const aiMaterial *ai_material,
                                           aiTextureType type,
                                           const SharedPtr<Texture> &default_texture);
    Color loadMaterialColor(const aiMaterial *ai_material,
                            const char *p_key,
                            unsigned int type,
                            unsigned int index);

    void setVertexBoneData(Vertex &vertex, int32_t bone_id, float weight);
    void extractBoneWeightForVertices(const aiMesh *mesh, std::vector<Vertex> &vertices);

    SharedPtr<Skeleton> buildSkeleton();
    SharedPtr<PoseAnimation> processAnimation(const aiAnimation *ai_animation);

    static mat4 сonvertMatrixToGLM(const aiMatrix4x4 &from);
    static vec2 сonvertVec2ToGLM(const aiVector2D &v);
    static vec3 сonvertVec3ToGLM(const aiVector3D &v);
    static quat сonvertQuatToGLM(const aiQuaternion &q);

    Assimp::Importer importer;
    const aiScene *ai_scene;
    std::filesystem::path path;
    std::unordered_map<std::string, SharedPtr<Texture>> loaded_textures;
    Assets *assets;

    std::unordered_map<std::string, int32_t> bone_map;
};

} // namespace ae

#endif // AE_ASSIMP_HELPER_H
