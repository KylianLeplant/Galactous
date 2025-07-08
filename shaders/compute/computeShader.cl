typedef struct {
    float3 massCenter;
    float3 center;
    float width;
    float mass;
    unsigned nextSiblingIndex;
    unsigned parentIndex;
    unsigned index;
} GPUOctree;

__kernel void add_arrays(__global const float* a, __global const float* b, __global float* result) {
    int gid = get_global_id(0);
    result[gid] = a[gid] + b[gid];
}

float norm(float3 vect){
    return sqrt(pow(vect.x,2)+pow(vect.y,2)+ pow(vect.z,2));
}

__kernel void accelerations(__global const float3* positions, __global const float* masses, __global float3* accelerations, __global const GPUOctree* octreeList, const unsigned sizeOctree, const float softening, const float G, const float theta) {
    int gid = get_global_id(0);
    float3 position = positions[gid];
    float mass = masses[gid];
    float3 acceleration = (float3)(0.0f, 0.0f, 0.0f);
    unsigned idOctree = 0;
    while (idOctree < sizeOctree) {
        __global const GPUOctree* octree = &octreeList[idOctree];
        float3 r = octree->massCenter - position;
        float d = norm(r) + softening;
        float s_d = octree->width / d;
        if (s_d < theta){
            acceleration += r * (G * octree->mass / pow(d,3));
            //change idOctree to the next sibling of the octree
            while (idOctree < sizeOctree && octreeList[idOctree].nextSiblingIndex == 0){
                idOctree = octreeList[idOctree].parentIndex;
            }
            idOctree = octreeList[idOctree].nextSiblingIndex;
        }
        else{
            idOctree++;
        }
    }
    accelerations[gid] = acceleration;
}
