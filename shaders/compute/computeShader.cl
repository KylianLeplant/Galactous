

typedef struct {
    float x;
    float y;
    float z;
} Vec3;


float norm(Vec3 vect){
    return sqrt(pow(vect.x,2)+pow(vect.y,2)+ pow(vect.z,2));
}

__kernel void accelerations(__global Vec3* positions, __global const float* masses, __global Vec3* velocities,  __global const float* softening, __global const float* G, __global const Vec3* octreeCenters, __global const float* octreeWidths, __global const Vec3* octreeMassCenters, __global const float* octreeMasses, __global const unsigned* octreeParentIndexes, __global const unsigned* octreeNextSiblingIndexes, __global const unsigned* sizeOctree, __global const float* timeStep) { 

    int gid = get_global_id(0);
    Vec3 position = positions[gid];
    Vec3 velocity = velocities[gid];
    float mass = masses[gid];
    Vec3 acceleration = {0.0f, 0.0f, 0.0f};
    unsigned idOctree = 0;
    char isFinished = false;
    unsigned nb = 0;

    
    while (idOctree < *sizeOctree && !isFinished) {
        nb++;
        
        Vec3 octreeCenter = octreeCenters[idOctree];
        float octreeWidth = octreeWidths[idOctree];
        Vec3 octreeMassCenter = octreeMassCenters[idOctree];
        float octreeMass = octreeMasses[idOctree];
        unsigned nextSiblingIndex = octreeNextSiblingIndexes[idOctree];
        unsigned parentIndex = octreeParentIndexes[idOctree];

        Vec3 r = {octreeMassCenter.x - position.x, octreeMassCenter.y - position.y, octreeMassCenter.z - position.z};
        float d = norm(r) + *softening;
        float s_d = octreeWidth / d;
        if (s_d < 1.0f) {
            float divCoeff = *G * octreeMass / (d * d * d);
            acceleration.x += r.x * divCoeff;
            acceleration.y += r.y * divCoeff;
            acceleration.z += r.z * divCoeff;

            //change idOctree to the next sibling of the octree
            while (idOctree < *sizeOctree && octreeNextSiblingIndexes[idOctree] == 0){
                
                if (idOctree == 0) {
                    isFinished = true;
                    break;
                }
                idOctree = octreeParentIndexes[idOctree];
            }
            idOctree = octreeNextSiblingIndexes[idOctree];
        }
        else{
            idOctree++;
        }
    }
    velocities[gid].x += acceleration.x * *timeStep;
    velocities[gid].y += acceleration.y * *timeStep;
    velocities[gid].z += acceleration.z * *timeStep;
    positions[gid].x += velocities[gid].x * *timeStep;
    positions[gid].y += velocities[gid].y * *timeStep;
    positions[gid].z += velocities[gid].z * *timeStep;

}
