#ifndef CORE_H
#define CORE_H

#define ImVec2(x, y) ((ImVec2) {x, y})
#define ImVec3(x, y, z) ((ImVec3) {x, y, z})
#define ImVec4(x, y, z, w) ((ImVec4) {x, y, z, w})
#define ImVec2_Assign(v1, v2) v1.x=v2.x; v1.y=v2.y
#define ImVec2_Equals(v1, v2) ((v1.x == v2.x && v1.y == v2.y) ? 1 : 0)

#define ImVec4_Assign(v1, v2) v1[0]=v2[0]; v1[1]=v2[1]; v1[2]=v2[2]; v1[3]=v2[3]

#endif
