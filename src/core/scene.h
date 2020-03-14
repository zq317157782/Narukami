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
#include "core/narukami.h"
#include "core/geometry.h"
#include "core/mesh.h"
#include "core/accelerator.h"
#include "core/primitive.h"
#include "core/interaction.h"
#include <vector>
NARUKAMI_BEGIN
class Scene{
    private:
        Accelerator _accelerator;
    public:
        std::vector<Primitive> lights;
    public:
        Scene(const std::vector<Primitive>& primitives){
             _accelerator=Accelerator(primitives);
             for(auto& p: primitives)
             {
                 if(p.light_material!=nullptr)
                 {
                     lights.push_back(p);
                 }
             }
        }

        inline bool intersect(MemoryArena &arena,const Ray& ray,Interaction* interaction) const{
            return _accelerator.intersect(arena,ray,interaction);
        }

        inline bool intersect(const Ray& ray) const{
             return _accelerator.intersect(ray);
        }
};
NARUKAMI_END