#ifndef _H_REARRANGEBONES_
#define _H_REARRANGEBONES_

#include "Skeleton.h"
#include "RHI/Rendering/Mesh.h" // Circular dependency, bad!
#include "Clip.h"
#include <map>

typedef std::map<int, int> BoneMap;

BoneMap RearrangeSkeleton(Skeleton& skeleton);
void RearrangeMesh(Mesh& mesh, BoneMap& boneMap);
void RearrangeClip(Clip& clip, BoneMap& boneMap);
void RearrangeFastclip(FastClip& clip, BoneMap& boneMap);

#endif
