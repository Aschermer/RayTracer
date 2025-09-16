
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <math.h>

#include "defines.h"
#include "vec3.h"

global vec3 SkyDir = vec3(0, 1, 0);
global vec3 SkyColor = vec3(0, 191, 255);
global vec3 GroundColor = vec3(131, 100, 62);

global vec3 SunDir = vec3(-0.7, 0.5, -1);
global vec3 SunColor = vec3(2550, 2280, 1320);

typedef union{
    struct{
        uint8 b;
        uint8 g;
        uint8 r;
        uint8 a;
    };
    uint8 d[4];
    uint32 pixel;
} windows_pixel;

typedef struct ray{
    vec3 Orig;
    vec3 Dir;
    
    vec3 at(double t) const {
        return Orig + t*Dir;
    }
} ray;

typedef struct{
    bool DidHit;
    double Dist;
    vec3 Normal;
} hit;

typedef struct{
    vec3 p[3];
} triangle;

internal windows_pixel RGBtoBGR
(vec3 color)
{
    windows_pixel out;
    out.b = (uint8)color.b & 0xFF;
    out.g = (uint8)color.g & 0xFF;
    out.r = (uint8)color.r & 0xFF;
    out.a = 255;
    return out;
}

internal hit SphereHitCheck
(vec3 Center, float Radius, ray Ray)
{
    hit Hit;
    vec3 oc = Center - Ray.Orig;
    float a = dot(Ray.Dir, Ray.Dir);
    float b = 2.0 * dot(Ray.Dir, oc);
    float c = dot(oc, oc) - Radius * Radius;
    int Discriminant = b*b - 4*a*c;
    Hit.DidHit = (Discriminant >= 0);
    if(Hit.DidHit)
    {
        Hit.Dist = (-b - sqrt(Discriminant)) / 2.0*a;
        Hit.Normal = Ray.at(Hit.Dist) - Center;
    }
    return Hit;
}

internal int TriHitCheck
(ray Ray, triangle Tri, vec3 Normal)
{
    triangle nTri = {};
    nTri.p[0] = normalize(Tri.p[0]);
    nTri.p[1] = normalize(Tri.p[1]);
    nTri.p[2] = normalize(Tri.p[2]);
    
    nTri.p[0] /= fabs(nTri.p[0].z);
    nTri.p[1] /= fabs(nTri.p[1].z);
    nTri.p[2] /= fabs(nTri.p[2].z);
    
    ray nRay = Ray;
    nRay.Dir = normalize(nRay.Dir);
    nRay.Dir /= fabs(nRay.Dir.z);
    
    double TriArea = fabs((nTri.p[1].x - nTri.p[0].x)*(nTri.p[2].y - nTri.p[0].y) - (nTri.p[2].x - nTri.p[1].x)*(nTri.p[1].y - nTri.p[0].y));
    
    double Area1 = 0;
    
    return 0;
}

internal vec3 CastRay
(ray Ray)
{
    int Collisions = 0;
    Ray.Dir = normalize(Ray.Dir);
    
    vec3 PixelColor = vec3();
    
    hit Hit = {};
    Hit = SphereHitCheck(vec3(0, 0, -2), 0.5, Ray);
    
    if(Hit.DidHit)
    {
        Collisions++;
        ray SecondRay;
        SecondRay.Orig = Ray.at(Hit.Dist);
        SecondRay.Dir = Hit.Normal;
        PixelColor = CastRay(SecondRay);
    }
    
    
    /*
    triangle Tri = {};
    Tri.p[0] = vec3(-0.5, 0, -2);
    Tri.p[1] = vec3(0, 0.5, -2);
    Tri.p[2] = vec3(0.5, -0.5, -2);
    int Result = TriHitCheck(Ray, Tri, vec3(0, 0, 0));
    
    if(Result)
    {
        Collisions = 1;
        PixelColor = vec3(255, 0, 0);
    }
    */
    
    if(Collisions == 0)
    {
        float RaySkyDot = dot(Ray.Dir, SkyDir);
        if(RaySkyDot > 0)
        {
            PixelColor = sqrt(RaySkyDot) * SkyColor + (1-sqrt(RaySkyDot)) * vec3(255, 255, 255);
        }
        else if(RaySkyDot > -0.1)
        {
            PixelColor = fabs(RaySkyDot) / 0.1 * GroundColor + (1 - fabs(RaySkyDot) / 0.1) * vec3(255, 255, 255);
        }
        else
        {
            PixelColor = GroundColor;
        }
        
        float RaySunDot = dot(Ray.Dir, normalize(SunDir));
        if(RaySunDot > 0.99)
        {
            RaySunDot = (RaySunDot - 0.99) * 100;
            PixelColor = RaySunDot * RaySunDot * RaySunDot * SunColor + (1-RaySunDot * RaySunDot * RaySunDot) * PixelColor;
        }
        PixelColor.r = fmin(255, PixelColor.r);
        PixelColor.g = fmin(255, PixelColor.g);
        PixelColor.b = fmin(255, PixelColor.b);
    }
    
    return PixelColor;
}

internal void RenderScreen
(screen_buffer *Buffer)
{
    float Fov = 90.0f;
    float FocalLength = 1.0f;
    
    
    
    int ImageWidth = Buffer->Width;
    int ImageHeight = Buffer->Height;
    float AspectRatio = (float)ImageWidth / (float)ImageHeight;
    
    float ViewWidth = 2 * tanf(Fov / 2) * FocalLength;
    float ViewHeight = ViewWidth / AspectRatio;
    
    vec3 CameraPos = vec3(0, 0, 0);
    
    vec3 ViewportU = vec3(ViewWidth, 0, 0);
    vec3 ViewportV = vec3(0, -ViewHeight, 0);
    
    vec3 PixelDeltaU = ViewportU / ImageWidth;
    vec3 PixelDeltaV = ViewportV / ImageHeight;
    
    
    vec3 ViewUpperLeft = CameraPos - vec3(0, 0, FocalLength) - ViewportU/2 - ViewportV/2;
    vec3 PixelUpperLeft = ViewUpperLeft + 0.5f * (PixelDeltaU + PixelDeltaV);
    
    windows_pixel *Pixel = (windows_pixel *)Buffer->Memory;
    for(int Y = 0; Y < ImageHeight; Y++)
    {
        for(int X = 0; X < ImageWidth; X++)
        {
            vec3 PixelCenter = PixelUpperLeft + (PixelDeltaU * X) + (PixelDeltaV * Y);
            
            ray Ray;
            Ray.Orig = CameraPos;
            Ray.Dir = PixelCenter - CameraPos;
            Ray.Dir = normalize(Ray.Dir);
            
            vec3 PixelColor = CastRay(Ray);
            
            *Pixel = RGBtoBGR(PixelColor);
            Pixel++;
        }
    }
}
