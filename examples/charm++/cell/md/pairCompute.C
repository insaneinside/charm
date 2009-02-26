#include "pairCompute.h"
#include "main.h"


PairCompute::PairCompute(int numParticlesPerPatch) {

  numParticles = numParticlesPerPatch;
  particleX[0] = (float*)(malloc_aligned(numParticles * sizeof(float), 128));
  particleX[1] = (float*)(malloc_aligned(numParticles * sizeof(float), 128));
  particleY[0] = (float*)(malloc_aligned(numParticles * sizeof(float), 128));
  particleY[1] = (float*)(malloc_aligned(numParticles * sizeof(float), 128));
  particleZ[0] = (float*)(malloc_aligned(numParticles * sizeof(float), 128));
  particleZ[1] = (float*)(malloc_aligned(numParticles * sizeof(float), 128));
  particleQ[0] = (float*)(malloc_aligned(numParticles * sizeof(float), 128));
  particleQ[1] = (float*)(malloc_aligned(numParticles * sizeof(float), 128));
  forceX[0] = (float*)(malloc_aligned(numParticles * sizeof(float), 128));
  forceX[1] = (float*)(malloc_aligned(numParticles * sizeof(float), 128));
  forceY[0] = (float*)(malloc_aligned(numParticles * sizeof(float), 128));
  forceY[1] = (float*)(malloc_aligned(numParticles * sizeof(float), 128));
  forceZ[0] = (float*)(malloc_aligned(numParticles * sizeof(float), 128));
  forceZ[1] = (float*)(malloc_aligned(numParticles * sizeof(float), 128));
  patchDataCount = 0;

  // Check in with the main chare so it "knows" this object is ready for the simulation to start
  mainProxy.proxyCheckIn();
}


PairCompute::PairCompute(CkMigrateMessage* msg) {
  CkAbort("PairCompute::PairCompute(CkMigrateMessage* msg) not implemented yet");
}


PairCompute::~PairCompute() {
  if (particleX[0] != NULL) { free_aligned(particleX[0]); particleX[0] = NULL; }
  if (particleX[1] != NULL) { free_aligned(particleX[1]); particleX[1] = NULL; }
  if (particleY[0] != NULL) { free_aligned(particleY[0]); particleY[0] = NULL; }
  if (particleY[1] != NULL) { free_aligned(particleY[1]); particleY[1] = NULL; }
  if (particleZ[0] != NULL) { free_aligned(particleZ[0]); particleZ[0] = NULL; }
  if (particleZ[1] != NULL) { free_aligned(particleZ[1]); particleZ[1] = NULL; }
  if (particleQ[0] != NULL) { free_aligned(particleQ[0]); particleQ[0] = NULL; }
  if (particleQ[1] != NULL) { free_aligned(particleQ[1]); particleQ[1] = NULL; }
  if (forceX[0] != NULL) { free_aligned(forceX[0]); forceX[0] = NULL; }
  if (forceX[1] != NULL) { free_aligned(forceX[1]); forceX[1] = NULL; }
  if (forceY[0] != NULL) { free_aligned(forceY[0]); forceY[0] = NULL; }
  if (forceY[1] != NULL) { free_aligned(forceY[1]); forceY[1] = NULL; }
  if (forceZ[0] != NULL) { free_aligned(forceZ[0]); forceZ[0] = NULL; }
  if (forceZ[1] != NULL) { free_aligned(forceZ[1]); forceZ[1] = NULL; }
  numParticles = 0;
}


void PairCompute::patchData(int numParticles, float* particleX, float* particleY, float* particleZ, float* particleQ, int fromPatch) {

  // Copy the data from the parameters
  memcpy(this->particleX[fromPatch], particleX, numParticles * sizeof(float));
  memcpy(this->particleY[fromPatch], particleY, numParticles * sizeof(float));
  memcpy(this->particleZ[fromPatch], particleZ, numParticles * sizeof(float));
  memcpy(this->particleQ[fromPatch], particleQ, numParticles * sizeof(float));

  // Increment the patch count and initiate the calculation of both patches have
  //   sent their data to this compute
  patchDataCount++;
  if (patchDataCount >= 2) {
    thisProxy(thisIndex.x, thisIndex.y).doCalc();  // Send message to self to do calculation
    patchDataCount = 0;
  }
}


void PairCompute::doCalc_callback() {

  // Calculate the index of patch 0 and send force data back to it
  int p0Index = thisIndex.x;
  int p0IndexX = PATCH_I_TO_X(p0Index);
  int p0IndexY = PATCH_I_TO_Y(p0Index);
  int p0IndexZ = PATCH_I_TO_Z(p0Index);
  patchArrayProxy(p0IndexX, p0IndexY, p0IndexZ).forceCheckIn(numParticles, forceX[0], forceY[0], forceZ[0]);

  // Calculate the index of patch 1 and send force data back to it
  int p1Index = thisIndex.y;
  int p1IndexX = PATCH_I_TO_X(p1Index);
  int p1IndexY = PATCH_I_TO_Y(p1Index);
  int p1IndexZ = PATCH_I_TO_Z(p1Index);
  patchArrayProxy(p1IndexX, p1IndexY, p1IndexZ).forceCheckIn(numParticles, forceX[1], forceY[1], forceZ[1]);
}


#include "pairCompute.def.h"