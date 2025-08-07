

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

__kernel void testGPUOctree(__global const GPUOctree* octreeList) {
    int gid = get_global_id(0);
        printf("Index: %u, Center: (%f, %f, %f), Width: %f, MassCenter: (%f, %f, %f), Mass: %f, NextSiblingIndex: %u, ParentIndex: %u\n",
               octreeList[gid].index,
               octreeList[gid].center.x, octreeList[gid].center.y, octreeList[gid].center.z,
               octreeList[gid].width,
               octreeList[gid].massCenter.x, octreeList[gid].massCenter.y, octreeList[gid].massCenter.z,
               octreeList[gid].mass,
               octreeList[gid].nextSiblingIndex,
               octreeList[gid].parentIndex);
}

float norm(Vec3 vect){
    return sqrt(pow(vect.x,2)+pow(vect.y,2)+ pow(vect.z,2));
}

__kernel void accelerations(__global const Vec3* positions, __global const float* masses, __global Vec3* accelerations,  __global const float* softening, __global const float* G, __global const Vec3* octreeCenters, __global const float* octreeWidths, __global const Vec3* octreeMassCenters, __global const float* octreeMasses, __global const unsigned* octreeParentIndexes, __global const unsigned* octreeNextSiblingIndexes, __global const unsigned* sizeOctree) { //, __global const unsigned* octreeIndexes
    //printf("hello");
    int gid = get_global_id(0);
    //printf("Index: %X \ncenter=%X,%X,%X\nwidth: %X\nmassCenter: %X,%X,%X\nmass: %X\nnextSiblingIndex: %X\nparentIndex: %X\n", octreeList[gid].index, octreeList[gid].center.x, octreeList[gid].center.y, octreeList[gid].center.z, octreeList[gid].width, octreeList[gid].massCenter.x, octreeList[gid].massCenter.y, octreeList[gid].massCenter.z, octreeList[gid].mass, octreeList[gid].nextSiblingIndex, octreeList[gid].parentIndex);
    //printf("sizeOctree = %d\n", *sizeOctree);
    Vec3 position = positions[gid];
    float mass = masses[gid];
    Vec3 acceleration = {0.0f, 0.0f, 0.0f};
    unsigned idOctree = 0;
    //printf("comparaison : idOctree = %d  sizeOctree : %d\n", idOctree, *sizeOctree);
    char isFinished = false;
    unsigned nb = 0;

    
    while (idOctree < *sizeOctree && !isFinished) {
        nb++;
        //printf("nb = %d\n", nb);
        
        Vec3 octreeCenter = octreeCenters[idOctree];
        float octreeWidth = octreeWidths[idOctree];
        Vec3 octreeMassCenter = octreeMassCenters[idOctree];
        float octreeMass = octreeMasses[idOctree];
        unsigned nextSiblingIndex = octreeNextSiblingIndexes[idOctree];
        unsigned parentIndex = octreeParentIndexes[idOctree];
        //unsigned index = octreeIndexes[idOctree];
        //printf("idOctree : %d  parentIndex : %d  nextSiblingIndex : %d\n",idOctree, parentIndex, nextSiblingIndex);
        //printf("\ngid = %d\n", gid);
        //printf("AZYparticle : \n  mass : %f, pos : %f, %f, %f\n", mass, position.x, position.y, position.z);
        //printf("octree : \n  index : %d, width : %f, mass : %f, pos : %f, %f, %f\n  masscenter : %f, %f, %f\n  nextIndex : %d, parentIndex : %d\n", idOctree, octreeWidth, octreeMass, octreeCenter.x, octreeCenter.y, octreeCenter.z, octreeMassCenter.x, octreeMassCenter.y, octreeMassCenter.z, nextSiblingIndex, parentIndex);
        //printf("current acceleration : (%f, %f, %f)\n", acceleration.x, acceleration.y, acceleration.z);

        Vec3 r = {octreeMassCenter.x - position.x, octreeMassCenter.y - position.y, octreeMassCenter.z - position.z};
        //printf("octree : %f, %f, %f,  pos : %f, %f, %f\n", octreeMassCenter.x, octreeMassCenter.y, octreeMassCenter.z, position.x, position.y, position.z);
        float d = norm(r) + *softening;
        float s_d = octreeWidth / d;
        if (s_d < 1.0f) {
            float divCoeff = *G * octreeMass / (d * d * d);
            //printf("divCoeff = %f, G : %f, octreeMass : %f, d : %f\n", divCoeff, *G, octreeMass, d);
            acceleration.x += r.x * divCoeff;
            acceleration.y += r.y * divCoeff;
            acceleration.z += r.z * divCoeff;
            //printf("s/d : %f, divCoeff : %f, r = (%f, %f, %f), d = %f, acceleration = (%f, %f, %f)\n", s_d, divCoeff, r.x, r.y, r.z, d, acceleration.x, acceleration.y, acceleration.z);

            //change idOctree to the next sibling of the octree
            while (idOctree < *sizeOctree && octreeNextSiblingIndexes[idOctree] == 0){
                
                if (idOctree == 0) {
                    isFinished = true;
                    break;
                }
                //printf("CHANGE idOctree = %d  parent = %d\n", idOctree, octreeParentIndexes[idOctree]);
                idOctree = octreeParentIndexes[idOctree];
            }
            idOctree = octreeNextSiblingIndexes[idOctree];
        }
        else{
            idOctree++;
        }
    }
    accelerations[gid] = acceleration;
    if (norm(accelerations[gid]) > 100.0f) {
        printf("Final acceleration for particle %d: (%f, %f, %f)\n", gid, acceleration.x, acceleration.y, acceleration.z);
    }
    //printf("gid = %d, acceleration = (%f, %f, %f)\n", gid, acceleration.x, acceleration.y, acceleration.z);
}
