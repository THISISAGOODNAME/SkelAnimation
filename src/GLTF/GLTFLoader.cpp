/**
 * https://github.com/KhronosGroup/glTF-Tutorials/blob/master/gltfTutorial/gltfTutorial_007_Animations.md
 */

#include "GLTFLoader.h"
#include "Animation/Track.h"
#include "Math/Transform.h"
#include "RHI/Rendering/Mesh.h"
#include <iostream>

#pragma region GLTFHelpers
namespace GLTFHelpers
{
    Transform GetLocalTransform(cgltf_node& node) {
        Transform result;

        if (node.has_matrix) {
            mat4 mat(&node.matrix[0]);
            result = mat4ToTransform(mat);
        }

        if (node.has_translation) {
            result.position = vec3(node.translation[0], node.translation[1], node.translation[2]);
        }

        if (node.has_rotation) {
            result.rotation = quat(node.rotation[0], node.rotation[1], node.rotation[2], node.rotation[3]);
        }

        if (node.has_scale) {
            result.scale = vec3(node.scale[0], node.scale[1], node.scale[2]);
        }

        return result;
    }

    int GetNodeIndex(cgltf_node* target, cgltf_node* allNodes, unsigned int numNodes) {
        if (target == nullptr) {
            return -1;
        }
        for (int i = 0; i < numNodes; ++i) {
            if (target == &allNodes[i]) {
                return i;
            }
        }
        return -1;
    }

    void GetScalarValues(std::vector<float>& outScalars, unsigned int inComponentCount, const cgltf_accessor& inAccessor) {
        outScalars.resize(inAccessor.count * inComponentCount);
        for (cgltf_size i = 0; i < inAccessor.count; ++i) {
            cgltf_accessor_read_float(&inAccessor, i, &outScalars[i * inComponentCount], inComponentCount);
        }
    }

    template<typename T, int N>
    void TrackFromChannel(Track<T, N>& inOutTrack, const cgltf_animation_channel& inChannel) {
        cgltf_animation_sampler& sampler = *inChannel.sampler;

        Interpolation interpolation = Interpolation::Constant;
        if (inChannel.sampler->interpolation == cgltf_interpolation_type_linear) {
            interpolation = Interpolation::Linear;
        }
        else if (inChannel.sampler->interpolation == cgltf_interpolation_type_cubic_spline) {
            interpolation = Interpolation::Cubic;
        }
        bool isSamplerCubic = interpolation == Interpolation::Cubic;

        std::vector<float> timelineFloats;
        GetScalarValues(timelineFloats, 1, *sampler.input);

        if (timelineFloats.size() == 0) {
            std::cout << "Error, trying to parse empty track\n";
            return;
        }

        std::vector<float> valueFloats;
        GetScalarValues(valueFloats, N, *sampler.output);

        int numFrames = sampler.input->count; // Timeline sampler
        int numberOfValuesPerFrame = valueFloats.size() / timelineFloats.size();

        inOutTrack.SetInterpolation(interpolation);
        inOutTrack.Resize(numFrames);

        for (unsigned int i = 0; i < numFrames; ++i) {
            int baseIndex = i * numberOfValuesPerFrame;
            Frame<N>& frame = inOutTrack[i];
            int offset = 0;

            frame.mTime = timelineFloats[i];

            for (int component = 0; component < N; ++component) {
                frame.mIn[component] = isSamplerCubic ? valueFloats[baseIndex + offset++] : 0.0f;
            }

            for (int component = 0; component < N; ++component) {
                frame.mValue[component] = valueFloats[baseIndex + offset++];
            }

            for (int component = 0; component < N; ++component) {
                frame.mOut[component] = isSamplerCubic ? valueFloats[baseIndex + offset++] : 0.0f;
            }

            if (offset != numberOfValuesPerFrame) {
                std::cout << "Added up wrong number of components\n";
            }
        }
    }

    void MeshFromAttribute(Mesh& outMesh, cgltf_attribute& attribute, cgltf_skin* skin, cgltf_node* nodes, unsigned int nodeCount) {
        cgltf_attribute_type attribType = attribute.type;
        cgltf_accessor& accessor = *attribute.data;

        unsigned int componentCount = 0;
        if (accessor.type == cgltf_type_vec2) {
            componentCount = 2;
        }
        else if (accessor.type == cgltf_type_vec3) {
            componentCount = 3;
        }
        else if (accessor.type == cgltf_type_vec4) {
            componentCount = 4;
        }
        else {
            std::cout << "Unknown data type\n";
            return;
        }

        std::vector<float> values;
        GetScalarValues(values, componentCount, accessor);
        int acessorCount = accessor.count;

        std::vector<vec3>& positions = outMesh.GetPosition();
        std::vector<vec3>& normals = outMesh.GetNormal();
        std::vector<vec2>& texCoords = outMesh.GetTexCoord();
        std::vector<ivec4>& influences = outMesh.GetInfluences();
        std::vector<vec4>& weights = outMesh.GetWeights();

        for (unsigned int i = 0; i < acessorCount; ++i) {
            int index = i * componentCount;
            switch (attribType) {
                case cgltf_attribute_type_position:
                    positions.push_back(vec3(values[index + 0], values[index + 1], values[index + 2]));
                    break;
                case cgltf_attribute_type_normal:
                {
                    vec3 normal = vec3(values[index + 0], values[index + 1], values[index + 2]);
                    if (lenSq(normal) < 0.000001f) {
                        normal = vec3(0, 1, 0);
                    }
                    normals.push_back(normalized(normal));
                }
                    break;
                case cgltf_attribute_type_texcoord:
                    texCoords.push_back(vec2(values[index + 0], values[index + 1]));
                    break;
                case cgltf_attribute_type_joints:
                {
                    // These indices are skin relative. This function has no information about the
                    // skin that is being parsed. Add +0.5f to round, since we can't read ints
                    ivec4 joints(
                            (int)(values[index + 0] + 0.5f),
                            (int)(values[index + 1] + 0.5f),
                            (int)(values[index + 2] + 0.5f),
                            (int)(values[index + 3] + 0.5f)
                    );


                    joints.x = std::fmaxf(0, GetNodeIndex(skin->joints[joints.x], nodes, nodeCount));
				    joints.y = std::fmaxf(0, GetNodeIndex(skin->joints[joints.y], nodes, nodeCount));
				    joints.z = std::fmaxf(0, GetNodeIndex(skin->joints[joints.z], nodes, nodeCount));
				    joints.w = std::fmaxf(0, GetNodeIndex(skin->joints[joints.w], nodes, nodeCount));

				    influences.push_back(joints);
                }
                    break;
                case cgltf_attribute_type_weights:
                    weights.push_back(vec4(values[index + 0], values[index + 1], values[index + 2], values[index + 3]));
                    break;
            }
        }
    }

    void StaticMeshFromAttribute(Mesh& outMesh, cgltf_attribute& attribute) {
        cgltf_attribute_type attribType = attribute.type;
        cgltf_accessor& accessor = *attribute.data;

        int componentCount = 0;
        if (accessor.type == cgltf_type_vec2) {
            componentCount = 2;
        }
        else if (accessor.type == cgltf_type_vec3) {
            componentCount = 3;
        }
        else if (accessor.type == cgltf_type_vec4) {
            componentCount = 4;
        }
        else {
            std::cout << "Unknown data type\n";
            return;
        }

        std::vector<float> values;
        GetScalarValues(values, componentCount, accessor);
        int acessorCount = accessor.count;

        std::vector<vec3>& positions = outMesh.GetPosition();
        std::vector<vec3>& normals = outMesh.GetNormal();
        std::vector<vec2>& texCoords = outMesh.GetTexCoord();

        for (int i = 0; i < acessorCount; ++i) {
            int index = i * componentCount;
            switch (attribType) {
                case cgltf_attribute_type_position:
                    positions.push_back(vec3(values[index + 0], values[index + 1], values[index + 2]));
                    break;
                case cgltf_attribute_type_normal:
                {
                    vec3 normal = vec3(values[index + 0], values[index + 1], values[index + 2]);
                    if (lenSq(normal) < 0.000001f) {
                        normal = vec3(0, 1, 0);
                    }
                    normals.push_back(normalized(normal));
                }
                    break;
                case cgltf_attribute_type_texcoord:
                    texCoords.push_back(vec2(values[index + 0], values[index + 1]));
                    break;
            }
        }
    }
}
#pragma endregion GLTFHelpers

cgltf_data* LoadGLTFFile(const char* path) {
    cgltf_options options;
    memset(&options, 0, sizeof(cgltf_options));

    cgltf_data* data = NULL;
    cgltf_result result = cgltf_parse_file(&options, path, &data);

    if (result != cgltf_result_success) {
        std::cout << "Could not load input file: " << path << "\n";
        return nullptr;
    }

    result = cgltf_load_buffers(&options, data, path);
    if (result != cgltf_result_success) {
        cgltf_free(data);
        std::cout << "Could not load buffers for: " << path << "\n";
        return nullptr;
    }

    result = cgltf_validate(data);
    if (result != cgltf_result_success) {
        cgltf_free(data);
        std::cout << "Invalid gltf file: " << path << "\n";
        return nullptr;
    }

    return data;
}

void FreeGLTFFile(cgltf_data* data) {
    if (data == nullptr) {
        std::cout << "WARNING: Can't free null data\n";
    }
    else {
        cgltf_free(data);
    }
}

Pose LoadRestPose(cgltf_data* data) {
    int boneCount = data->nodes_count;
    Pose result(boneCount);

    for (int i = 0; i < boneCount; ++i) {
        cgltf_node* node = &(data->nodes[i]);

        Transform transform = GLTFHelpers::GetLocalTransform(data->nodes[i]);
        result.SetLocalTransform(i, transform);

        int parent = GLTFHelpers::GetNodeIndex(node->parent, data->nodes, boneCount);
        result.SetParent(i, parent);
    }

    return result;
}

std::vector<std::string> LoadJointNames(cgltf_data* data) {
    int boneCount = data->nodes_count;
    std::vector<std::string> result(boneCount, "Not Set");

    for (unsigned int i = 0; i < boneCount; ++i) {
        cgltf_node* node = &(data->nodes[i]);

        if (node->name == nullptr) {
            result[i] = "EMPTY NODE";
        }
        else {
            result[i] = node->name;
        }
    }

    return result;
}

std::vector<Clip> LoadAnimationClips(cgltf_data* data) {
    int numClips = data->animations_count;
    int numNodes = data->nodes_count;

    std::vector<Clip> result;
    result.resize(numClips);

    for (int i = 0; i < numClips; ++i) {
        result[i].SetName(data->animations[i].name);

        int numChannels = data->animations[i].channels_count;
        for (int j = 0; j < numChannels; ++j) {
            cgltf_animation_channel& channel = data->animations[i].channels[j];
            cgltf_node* target = channel.target_node;
            int nodeId = GLTFHelpers::GetNodeIndex(target, data->nodes, numNodes);

            if (channel.target_path == cgltf_animation_path_type_translation) {
                VectorTrack& track = result[i][nodeId].GetPositionTrack();
                GLTFHelpers::TrackFromChannel<vec3, 3>(track, channel);
            }
            else if (channel.target_path == cgltf_animation_path_type_scale) {
                VectorTrack& track = result[i][nodeId].GetScaleTrack();
                GLTFHelpers::TrackFromChannel<vec3, 3>(track, channel);
            }
            else if (channel.target_path == cgltf_animation_path_type_rotation) {
                QuaternionTrack& track = result[i][nodeId].GetRotationTrack();
                GLTFHelpers::TrackFromChannel<quat, 4>(track, channel);
            }
        }
        result[i].RecalculateDuration();
    }

    return result;
}

Pose LoadBindPose(cgltf_data* data) {
    // First, we find the bind pose of the model, but in world space
    Pose restPose = LoadRestPose(data);
    int numBones = restPose.Size();
    std::vector<Transform> worldBindPose(numBones);
    for (int i = 0; i < numBones; ++i) {
        worldBindPose[i] = restPose.GetGlobalTransform(i);
    }

    // Next, we loop trough every skinned mesh in the gltf file
    int numSkins = data->skins_count;
    for (int i = 0; i < numSkins; ++i) {
        cgltf_skin* skin = &(data->skins[i]);
        std::vector<float> invBindAccessor;
        GLTFHelpers::GetScalarValues(invBindAccessor, 16, *skin->inverse_bind_matrices);

        // For each joint in the skin
        int numJoints = skin->joints_count;
        for (int j = 0; j < numJoints; ++j) {
            // Read the ivnerse bind matrix of the joint
            float* matrix = &(invBindAccessor[j * 16]);
            mat4 invBindMatrix = mat4(matrix);
            // invert, convert to transform
            mat4 bindMatrix = inverse(invBindMatrix);
            Transform bindTransform = mat4ToTransform(bindMatrix);
            // Set that transform in the worldBindPose.
            cgltf_node* jointNode = skin->joints[j];
            int jointIndex = GLTFHelpers::GetNodeIndex(jointNode, data->nodes, numBones);
            worldBindPose[jointIndex] = bindTransform;
        } // end for each joint
    } // end for each skin

    // Convert the world bind pose to a regular bind pose
    Pose bindPose = restPose;
    for (int i = 0; i < numBones; ++i) {
        Transform current = worldBindPose[i];
        int p = bindPose.GetParent(i);
        if (p >= 0) { // Bring into parent space
            Transform parent = worldBindPose[p];
            current = combine(inverse(parent), current);
        }
        bindPose.SetLocalTransform(i, current);
    }

    return bindPose;
}

Skeleton LoadSkeleton(cgltf_data* data) {
    return Skeleton(
            LoadRestPose(data),
            LoadBindPose(data),
            LoadJointNames(data)
    );
}

std::vector<Mesh> LoadMeshes(cgltf_data* data) {
    std::vector<Mesh> result;
    cgltf_node* nodes = data->nodes;
    int nodeCount = data->nodes_count;

    for (int i = 0; i < nodeCount; ++i) {
        cgltf_node* node = &nodes[i];
        if (node->mesh == 0 || node->skin == 0) {
            continue;
        }

        int numPrims = node->mesh->primitives_count;
        for (int j = 0; j < numPrims; ++j) {
            result.push_back(Mesh());
            Mesh& mesh = result[result.size() - 1];

            cgltf_primitive* primitive = &node->mesh->primitives[j];

            int numAttributes = primitive->attributes_count;
            for (unsigned int k = 0; k < numAttributes; ++k) {
                cgltf_attribute* attribute = &primitive->attributes[k];
                GLTFHelpers::MeshFromAttribute(mesh, *attribute, node->skin, nodes, nodeCount);
            }

            if (primitive->indices != 0) {
                int indexCount = primitive->indices->count;
                std::vector<unsigned int>& indices = mesh.GetIndices();
                indices.resize(indexCount);

                for (int k = 0; k < indexCount; ++k) {
                    indices[k] = cgltf_accessor_read_index(primitive->indices, k);
                }
            }

            mesh.UpdateOpenGLBuffers();
        }
    }

    return result;
}

std::vector<Mesh> LoadStaticMeshes(cgltf_data* data) {
    std::vector<Mesh> result;
    cgltf_node* nodes = data->nodes;
    int nodeCount = data->nodes_count;

    for (int i = 0; i < nodeCount; ++i) {
        cgltf_node* node = &nodes[i];
        if (node->mesh == 0) {
            continue;
        }

        int numPrims = node->mesh->primitives_count;
        for (int j = 0; j < numPrims; ++j) {
            result.push_back(Mesh());
            Mesh& mesh = result[result.size() - 1];

            cgltf_primitive* primitive = &node->mesh->primitives[j];

            int numAttributes = primitive->attributes_count;
            for (int k = 0; k < numAttributes; ++k) {
                cgltf_attribute* attribute = &primitive->attributes[k];
                GLTFHelpers::StaticMeshFromAttribute(mesh, *attribute);
            }

            if (primitive->indices != 0) {
                int indexCount = primitive->indices->count;
                std::vector<unsigned int>& indices = mesh.GetIndices();
                indices.resize(indexCount);

                for (int k = 0; k < indexCount; ++k) {
                    indices[k] = (unsigned int)cgltf_accessor_read_index(primitive->indices, k);
                }
            }

            mesh.UpdateOpenGLBuffers();
        }
    }

    return result;
}
