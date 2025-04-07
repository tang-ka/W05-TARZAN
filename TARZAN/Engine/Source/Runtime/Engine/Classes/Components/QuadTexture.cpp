
#include "QuadTexture.h"

uint32 quadTextureInices[] =
{
    0,1,2,
    1,3,2
};

FVertexTexture quadTextureVertices[] =
{
    {-1.0f,1.0f,0.0f,0.0f,0.0f},
    { 1.0f,1.0f,0.0f,1.0f,0.0f},
    {-1.0f,-1.0f,0.0f,0.0f,1.0f},
    { 1.0f,-1.0f,0.0f,1.0f,1.0f}
};

//uint32 quadTextureInices[] =
//{
//    0, 1, 2,
//    0, 2, 3
//};
//
//FVertexTexture quadTextureVertices[] =
//{
//    { -1.0f,  1.0f, 0.0f, 0.0f, 0.0f },
//    {  1.0f,  1.0f, 0.0f, 1.0f, 0.0f },
//    {  1.0f, -1.0f, 0.0f, 1.0f, 1.0f },
//    { -1.0f, -1.0f, 0.0f, 0.0f, 1.0f },
//};