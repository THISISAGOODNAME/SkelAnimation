#ifndef _H_ANIMATIONBAKER_
#define _H_ANIMATIONBAKER_

#include "Animation/Clip.h"
#include "Animation/Skeleton.h"
#include "AnimTexture.h"

void BakeAnimationToTexture(Skeleton& skel, Clip& clip, AnimTexture& tex);

#endif
