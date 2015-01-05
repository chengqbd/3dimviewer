///////////////////////////////////////////////////////////////////////////////
// $Id$
//
// 3DimViewer
// Lightweight 3D DICOM viewer.
//
// Copyright 2008-2012 3Dim Laboratory s.r.o.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef Shade_H
#define Shade_H

namespace shader
{

const char Shade[] =
"#ifndef __GLSL_CG_DATA_TYPES\n"
"#define saturate(type) clamp(type,0.0,1.0) \n"
"#endif \n"
" \n"
"uniform sampler3D t3D;      // 3D texture containing data \n"
"uniform sampler1D LookUp;   // color and transparency look-up texture \n"
"uniform sampler2D Osr;      // Off-Screen render texture with back-facing polygon data \n"
"uniform sampler2D OsrWC;    // back-facing world coords \n"
"uniform sampler2D Noise;    // Noise offset used to remove aliasing [replacable by internal GLSL noise generator]\n"
"uniform sampler2D Depth;\n"
" \n"
"varying vec3 worldCoords; \n"
" \n"
"uniform vec2 inputAdjustment;  \n"
"uniform vec2 imageAdjustment; \n"
"uniform vec3 sVector;\n"
"uniform vec3 tResolution; \n"
"uniform float StopCondition; \n"
"uniform vec2 wSize;             // Viewport size  \n"
"uniform vec4 pl;                // plane that cuts the volume \n"
"uniform float textureSampling; \n"
"\n"
"uniform sampler3D tSkip3D;      // skipping 3D texture \n"
"uniform vec3 skipTexSize; \n"
"uniform vec3 tSkipResolution; \n"
" \n"
" \n"
"// Compute position against the cutting plane \n"
"float cuttingPlane(vec3 pos) \n"
"{ \n"
    "return (pl.x * pos.x + pl.y * pos.y + pl.z * pos.z + pl.w);  \n"
"} \n"
" \n"
"// Compute plane intersection \n"
"float planeCut(vec3 pos, float value, float defvalue) \n"
"{  \n"
    "return (cuttingPlane(pos) > 0.0 ? value : defvalue); \n"
"} \n"
" \n"
"// Use the skipping volume to evaluate the given position \n"
"int isMeaningful(vec3 pos) \n"
"{\n"
    "// is the voxel otside of the cutting plane\n"
    "if( cuttingPlane(pos) < 0.0 ) \n"
    "{ \n"
        "return 0; \n"
    "} \n"
"     \n"
    "// sample a voxel at initial position \n"
    "vec4 voxel = texture3D(tSkip3D, pos * skipTexSize); \n"
"    \n"
    "// minimum and maximum voxel value  \n"
    "float vmax = voxel.g;\n"
    "vmax = saturate((vmax * inputAdjustment.x + 0.001) + inputAdjustment.y);\n"
"    \n"
    "// is the position empty (Hu < -200)?\n"
    "return (vmax < 0.15) ? 0 : 1; \n"
"} \n"
" \n"
"// Compute length of the step in the 3D texture \n"
"float stepLength(vec3 path) \n"
"{ \n"
    "// All converted to the first quadrant \n"
    "vec3 aPath = abs(path); \n"
"\n"
    "// to be explained later \n"
    "// basically computes line/plane intersection \n"
    "vec3 t = tResolution.xyz / (aPath+1e-20); \n"
    "float d2x = length(aPath*t.x); // intersection with a plane perpendicular to the X axis \n"
    "float d2y = length(aPath*t.y); // intersection with a plane perpendicular to the Y axis \n"
    "float d2z = length(aPath*t.z); // intersection with a plane perpendicular to the Z axis \n"
"\n"
    "// return the closest intersection - that is, the correct one \n"
    "return min(d2x,min(d2y,d2z)); \n"
"} \n"
"\n"
"// Compute ray depth in depth buffer \n"
"float computeDepth(vec3 backWC, vec3 frontWC, float rayTermination) \n"
"{   \n"
    "vec3 terminationWC = frontWC + ((backWC - frontWC) * vec3(rayTermination)); \n"
    "vec4 iproj = gl_ModelViewProjectionMatrix * vec4(terminationWC, 1); \n"
    "iproj.z /= iproj.w; \n"
    "return (iproj.z + 1.0) * 0.5; \n"
"} \n"
" \n"
" \n"
"void main() \n"
"{ \n"
    "// sample FirstPass texture with back-facing tex. coords (RGB) and back-facing Z-Buffer value (A) \n"
    "vec4 OSR = texture2D(Osr,(gl_FragCoord.xy)*wSize); \n"
"    \n"
    "// back and front coordinates \n"
    "vec3 backWC = texture2D(OsrWC, (gl_FragCoord.xy) * wSize).xyz; \n"
    "vec3 frontWC = worldCoords; \n"
"    \n"
    "// set initial color \n"
    "vec3 color = vec3(0.0), shade; \n"
"\n"
    "// set initial translucency \n"
    "float trans = 1.0; \n"
"\n"
    "// maximum normal length \n"
    "float maxNormalLength = 1.0 / length(vec3(1,1,1)); \n"
"\n"
    "// set ray direction \n"
    "vec3 RayPath = OSR.rgb - gl_TexCoord[0].stp; \n"
"    \n"
    "// normalized direction \n"
    "vec3 nPath = normalize(RayPath);\n"
"     \n"
    "// light position comes from OpenGL (hack!) \n"
    "vec3 vLight = nPath; \n"
"    \n"
    "// viewer is at the end of the ray \n"
    "vec3 vCamera = nPath; \n"
"    \n"
    "// adjust length of the step (vector)   \n"
    "vec3 CoarsePath = nPath * tSkipResolution.x; \n"
"    \n"
    "// adjust length of the step (vector)   \n"
"//    vec3 FinePath = nPath * stepLength(RayPath); \n"
    "vec3 FinePath = nPath * tResolution.x; \n"
"    \n"
    "// distance to travel by the ray \n"
    "float tDistance = length(RayPath); \n"
"    \n"
    "// length of a coarse step (scalar) \n"
    "float tCoarseStep = length(CoarsePath); \n"
"    \n"
    "// length of a fine step (scalar) \n"
    "float tFineStep = length(FinePath); \n"
"\n"
	"// calculate the number of fine steps in a single coarse step\n"
	"int NumOfFineSteps = int(tCoarseStep / tFineStep);\n"
"    \n"
    "// where to start in the texture (scalar) \n"
    "float tPositionStart = texture2D(Noise, (gl_FragCoord.xy - vec2(0.5)) * wSize).r * tFineStep; \n"
"    \n"
    "// where to start in the texture (vector) \n"
    "vec3 vtPositionStart = gl_TexCoord[0].stp + nPath * tPositionStart; \n"
"      \n"
    "// is the initial voxel meaningful? \n"
    "int voxelFlagStart = isMeaningful(vtPositionStart); \n"
"    \n"
    "// outer loop \n"
    "for( int i = 0; i < 256; i++ ) \n"
    "{\n"
        "// stop when (almost) no more color is being added \n"
        "// check if the ray is still vithin the textured volume \n"
        "if( trans <= StopCondition || tPositionStart > tDistance )\n"
            "break;\n"
"        \n"
        "// next position \n"
        "vec3 vtPositionEnd = vtPositionStart + CoarsePath; \n"
        "float tPositionEnd = tPositionStart + tCoarseStep; \n"
"        \n"
        "// is the end position meaningful? \n"
        "int voxelFlagEnd = isMeaningful(vtPositionEnd); \n"
"\n"
        "// should this part of the volume be passed\n"
        "if( (voxelFlagStart + voxelFlagEnd) != 0 )\n"
		"{\n"
            "// inner loop\n"
            "vec3 vtPosition = vtPositionStart; \n"
            "float tPosition = tPositionStart; \n"
            "for( int j = 0; j < NumOfFineSteps; j++ ) \n"
            "{\n"
                "// stop when (almost) no more color is being added \n"
                "if( trans <= StopCondition ) \n"
                    "break;\n"
"                \n"
                "// z-buffer check\n"
                "if( texture2D(Depth, (gl_FragCoord.xy) * wSize).r < computeDepth(backWC, frontWC, tPosition / tDistance) )\n"
                    "break;\n"
"             \n"
                "// sample voxel at the current position \n"
                "float voxel = planeCut(vtPosition, saturate((texture3D(t3D, vtPosition).r * inputAdjustment.x + 0.001) + inputAdjustment.y), 0.0);\n"
"\n"
                "// sample Look-Up texture \n"
                "vec4 LUT = texture1D(LookUp,voxel); \n"
"                                \n"
                "if( LUT.a < 0.99 ) \n"
                "{ \n"
                    "// sample surrounding voxels \n"
                    "const float minvoxel = 0.7; \n"
                    "vec3 aPosition = vtPosition + vec3(0.0, 0.0, sVector[2]); \n"
                    "float N00P = planeCut(aPosition, 1.0, minvoxel) * texture3D(t3D, aPosition).r; \n"
                    "vec3 bPosition = vtPosition + vec3(0.0, 0.0, -sVector[2]); \n"
                    "float N00M = planeCut(bPosition, 1.0, minvoxel) * texture3D(t3D, bPosition).r; \n"
                    "vec3 cPosition = vtPosition + vec3(0.0, sVector[1], 0.0); \n"
                    "float N0P0 = planeCut(cPosition, 1.0, minvoxel) * texture3D(t3D, cPosition).r; \n"
                    "vec3 dPosition = vtPosition + vec3(0.0, -sVector[1], 0.0); \n"
                    "float N0M0 = planeCut(dPosition, 1.0, minvoxel) * texture3D(t3D, dPosition).r; \n"
                    "vec3 ePosition = vtPosition + vec3(sVector[0], 0.0, 0.0); \n"
                    "float NP00 = planeCut(ePosition, 1.0, minvoxel) * texture3D(t3D, ePosition).r; \n"
                    "vec3 fPosition = vtPosition + vec3(-sVector[0], 0.0, 0.0); \n"
                    "float NM00 = planeCut(fPosition, 1.0, minvoxel) * texture3D(t3D, fPosition).r; \n"
"                    \n"
                    "// estimate a normal vector \n"
                    "vec3 normal = vec3(NP00 - NM00,\n"
                                       "N0P0 - N0M0,\n"
                                       "N00P - N00M); \n"
"                    \n"
                    "// gradient magnitude\n"
                    "float alphaDiv = length(normal) * maxNormalLength;\n"
"                    \n"
                    "// Add current voxel color to accumulation variable\n"
                    "// and multiply by transparency \n"
                    "// (voxel aplha * accumulated transparency) \n"
                    "if( alphaDiv > 0.005 ) \n"
                    "{\n"
                        "// normal vector \n"
                        "normal = normalize(normal); \n"
"                         \n"
                        "// Shade is computed by Lambertian shading model \n"
                        "float DiffuseColor = saturate(dot(normal, vLight)); \n"
"                         \n"
                        "// Lambertian term + Ambient term \n"
                        "shade = (0.75 * DiffuseColor + 0.25) * LUT.rgb;\n"
"                        \n"
                        "// accumulate color\n"
                        "float alpha = saturate(1.0 - LUT.a); \n"
                        "alpha *= textureSampling; \n"
                        "color += saturate(shade) * alpha * trans;\n"
"                        \n"
                        "// accumulate translucency \n"
                        "trans *= 1.0 - alpha;\n"
                    "}\n"
                "}\n"
"                \n"
                "// move ray to the next position - both the scalar and the vector \n"
                "vtPosition += FinePath;\n"
                "tPosition += tFineStep;\n"
            "} \n"
        "}        \n"
"        \n"
        "// move to the next position \n"
        "vtPositionStart = vtPositionEnd;\n"
        "tPositionStart = tPositionEnd;\n"
        "voxelFlagStart = voxelFlagEnd;        \n"
    "} \n"
"    \n"
    "// if not enought color is accumulated than fragment is discarded  \n"
    "if( (tPositionStart > tDistance) && (all(lessThan(color, vec3(0.01)))) ) \n"
    "{ \n"
        "gl_FragData[0] = vec4(0.0, 0.0, 0.0, 0.0); \n"
        "gl_FragData[1] = vec4(1.0, 0.0, 0.0, 0.0); \n"
    "} \n"
    "else \n"
    "{ \n"
        "gl_FragData[0] = vec4(saturate((color * imageAdjustment.y) + imageAdjustment.x), 0.0);\n"
        "gl_FragData[1] = vec4(0.0, (1.0 - trans), 0.0, 0.0);\n"
    "}\n"
"}\n";

} // namespace shader

#endif // Shade_H

