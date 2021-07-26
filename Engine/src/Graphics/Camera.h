#ifndef CAMERA_H
#define CAMERA_H

#include "Math/MathTypes.h"

typedef struct Camera
{
    m4 Projection;
} Camera;

#define Camera_(camera, projection)		\
    {						\
	m4_copy(projection, camera.Projection);	\
    }

#endif
