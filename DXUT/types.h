#ifndef DXUT_TYPES_H
#define DXUT_TYPES_H

// atalhos para tipos da linguagem C++
using uchar = unsigned char;
using ushort = unsigned short;
using ulong = unsigned long;
using llong = long long;
using ullong = unsigned long long;
using uint = unsigned int;

// formatos de geometria
enum GeometryTypes
{
	UNKNOWN_T,
	BOX_T,
	CYLINDER_T,
	SPHERE_T,
	GEOSPHERE_T,
	GRID_T,
	QUAD_T
};
#endif