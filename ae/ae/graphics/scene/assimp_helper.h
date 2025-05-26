#ifndef AE_ASSIMP_HELPER_H
#define AE_ASSIMP_HELPER_H

#include "../core/texture.h"
#include "pose_animation.h"
#include "model.h"
#include "skeleton.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <spdlog/spdlog.h>

#include <filesystem>
#include <memory>
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

    std::shared_ptr<MeshNode> processRootNode() { return processNode(ai_scene->mRootNode); }
    std::shared_ptr<MeshNode> processNode(const aiNode *ai_node);
    std::shared_ptr<Mesh> processMesh(const aiMesh *ai_mesh);
    std::shared_ptr<Material> processMaterial(const aiMaterial *ai_material);
    std::shared_ptr<Texture> loadMaterialTexture(const aiMaterial *ai_material,
                                                 aiTextureType type,
                                                 const std::shared_ptr<Texture> &default_texture);
    Color loadMaterialColor(const aiMaterial *ai_material,
                            const char *p_key,
                            unsigned int type,
                            unsigned int index);

    void setVertexBoneData(Vertex &vertex, int32_t bone_id, float weight);
    void extractBoneWeightForVertices(const aiMesh *mesh, std::vector<Vertex> &vertices);

    std::shared_ptr<Skeleton> buildSkeleton();
    std::shared_ptr<PoseAnimation> processAnimation(const aiAnimation *ai_animation);

    static mat4 сonvertMatrixToGLM(const aiMatrix4x4 &from);
    static vec2 сonvertVec2ToGLM(const aiVector2D &v);
    static vec3 сonvertVec3ToGLM(const aiVector3D &v);
    static quat сonvertQuatToGLM(const aiQuaternion &q);

    Assimp::Importer importer;
    const aiScene *ai_scene;
    std::filesystem::path path;
    std::unordered_map<std::string, std::shared_ptr<Texture>> loaded_textures;
    Assets *assets;

    std::unordered_map<std::string, int32_t> bone_map;
};

} // namespace ae

#endif // AE_ASSIMP_HELPER_H
