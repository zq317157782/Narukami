/*
MIT License

Copyright (c) 2019 ZhuQian

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once
/*
narukami.h
*/

//marco for namespace
#define NARUKAMI_BEGIN \
    namespace narukami \
    {
#define NARUKAMI_END }

//define DEBUG marco
#ifdef NDEBUG
#undef NARUKAMI_DEBUG
#else
#define NARUKAMI_DEBUG
#endif

#define NARUKAMI_STAT_ENABLED 1

#include <assert.h>
#include <ostream>
#include <iostream>
#include "core/platform.h"

NARUKAMI_BEGIN
struct Ray;
class Quaternion;
class Interaction;
class SurfaceInteraction;
class Scene;
class Light;
class AreaLight;
class LightMaterial;
class VisibilityTester;
class MemoryArena;


template <typename T>
using shared = std::shared_ptr<T>;

template <typename T>
inline shared<T> make_shared(const T &t)
{
    T *ptr = new T(t);
    return shared<T>(ptr);
}
NARUKAMI_END