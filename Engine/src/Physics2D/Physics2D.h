#ifndef PHYSICS_2D
#define PHYSICS_2D

#include <Math/MathTypes.h>
#include <Math/V2.h>

typedef struct RigidBody2D
{
    v3 Position;
    v3 Rotation;
} RigidBody2D;

typedef struct AABB
{
    v2 Min;
    v2 Max;
} AABB;

typedef struct RectangleShape
{
    v2 Min;
    v2 Max;
    v3 Center;
    v3 Diagonal;
    RigidBody2D Body;
} RectangleShape;

force_inline void
rectangle_shape_(RectangleShape rectangleShape, v3 min, v3 max)
{
    v2_sub(max, min, rectangleShape.Diagonal);
}

force_inline void
rectangle_shape_get_min(RectangleShape rectangleShape, v3 min)
{
    v2_sub(rectangleShape.Center, rectangleShape.Body.Position, min);
}

typedef struct CircleShape
{
    v3 Position;
    f32 Radius;
} CircleShape;

force_inline i32
aabb_intersect(AABB a, AABB b)
{
    if (a.Max[0] < b.Min[0])
	return 0;

    return 1;
}



typedef struct RigidBody
{
    v3 Position;
    v3 Rotation;

    v2 Velocity;
    f32 AngularVelocity;

    v2 Force;
    f32 Mass;
} RigidBody;

typedef struct Simulation
{
    RigidBody* Bodies;
} Simulation;

force_inline void
physics2d_on_update(f32 timestep)
{

}

#endif
