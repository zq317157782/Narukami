# Narukami
lightweight PBRT-style path tracer

# Todo list
 - [x] element math 
 - [x] vector math
 - [x] matrix math
    - [x] SSE general inverse
 - [x] transform structure
 - [x] basic/SoA ray structure
 - [x] basic/SoA triangle structure (one vertex with two edge vector)
 - [x] intersect func for ray and SoA triangle(single ray with four triangle)
 - [ ] partial differential dpdu/dpdv for triangle
 - [x] basic rect structure 
 - [x] basic/SoA AABB structure(for QBVH) and intersect scheme
 - [x] basic spectrum structure
 - [x] basic triangle mesh structure 
    - [ ] read obj model 
 - [ ] basic path tracing framework
 - [ ] basic image i/o 
    - [x] write PNG
    - [ ] read PNG
 - [ ] basic film structure
 - [ ] sampling methods
    - [x] stratified sampling method
    - [x] Halton sampling method
    - [ ] Hammersley method 
    - [ ] Sobel sampling method
    - [ ] R2 sampling method
 
# Reference
[Physically Based Rendering](https://www.pbrt.org/)

[Embree](https://embree.github.io/)

[Mitsuba renderer](https://www.mitsuba-renderer.org/)
 
