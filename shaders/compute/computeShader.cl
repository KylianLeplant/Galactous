

typedef struct {
    float x;
    float y;
    float z;
} Vec3;

typedef struct {
    Vec3 center;
    float width;
    Vec3 massCenter;
    float mass;
    unsigned nextSiblingIndex;
    unsigned parentIndex;
    unsigned index;
} GPUOctree;

__kernel void testVec3(__global const Vec3* a, __global Vec3* b) {
    int gid = get_global_id(0);
    b[gid].x = a[gid].x;
    b[gid].y = a[gid].y;
    b[gid].z = a[gid].z;
}

__kernel void add_arrays(__global const float* a, __global const float* b, __global float* result) {
    int gid = get_global_id(0);
    result[gid] = a[gid] + b[gid];
}

float norm(Vec3 vect){
    return sqrt(pow(vect.x,2)+pow(vect.y,2)+ pow(vect.z,2));
}

__kernel void accelerations(__global const Vec3* positions, __global const float* masses, __global Vec3* accelerations, __global const GPUOctree* octreeList, __global const unsigned* sizeOctree, __global const float* softening, __global const float* G) {
    int gid = get_global_id(0);
    printf("sizeOctree = %d\n", *sizeOctree);
    Vec3 position = positions[gid];
    float mass = masses[gid];
    Vec3 acceleration = {0.0f, 0.0f, 0.0f};
    unsigned idOctree = 0;
    while (idOctree < *sizeOctree) {
        __global const GPUOctree* octree = &octreeList[idOctree];
        Vec3 r = {octree->massCenter.x - position.x, octree->massCenter.y - position.y, octree->massCenter.z - position.z};
        printf("octree : %f, %f, %f,  pos : %f, %f, %f\n", octree->massCenter.x, octree->massCenter.y, octree->massCenter.z, position.x, position.y, position.z);
        float d = norm(r) + *softening;
        float s_d = octree->width / d;
        if (s_d < 1.0f) {
            float divCoeff = *G *octree->mass / (d * d * d);
            acceleration.x += r.x * divCoeff;
            acceleration.y += r.y * divCoeff;
            acceleration.z += r.z * divCoeff;
            printf("gid = %d, octree index = %d, mass = %f, r = (%f, %f, %f), d = %f, acceleration = (%f, %f, %f)\n", gid, octree->index, octree->mass, r.x, r.y, r.z, d, acceleration.x, acceleration.y, acceleration.z);

            //change idOctree to the next sibling of the octree
            while (idOctree < *sizeOctree && octreeList[idOctree].nextSiblingIndex == 0){
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
