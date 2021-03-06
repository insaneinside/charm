
/****************************************************
             Measures the degree of pipeling in the network interface.
             Each processor sends K messages and the destination
             responds with 1 message. Depending on the message size
             different bottlenecks in the pipeline will affect the
             rate of message passing. For short message it could be
             NIC processing and for large messages it will be network
             bandwidth.

   - Sameer Kumar 03/08/05
**********************************************************/


#include <stdlib.h>
#include <converse.h>

//Number of iterations for each message size
enum {nCycles = 100};
//Try message sizes from 16 to 262144
enum { maxMsgSize = 1 << 18 };

//Variable declarations
CpvDeclare(int,msgSize);
CpvDeclare(int,cycleNum);

CpvDeclare(int,exitHandler);
CpvDeclare(int,node0Handler);
CpvDeclare(int,node1Handler);
CpvStaticDeclare(double,startTime);
CpvStaticDeclare(double,endTime);

CpvStaticDeclare(int, kFactor);
CpvStaticDeclare(int, nRecvd);

#define K_FACTOR 16   //Node 0 sends K_FACTOR messages to Node 1 
                       //and gets an ack for them. The benchmark 
                       //measures bandwidth for this burst of K_FACTOR messages

//Compute idle time too. Accuracy of idle time depends on the machine layer
CpvDeclare(double, IdleStartTime);
CpvDeclare(double, IdleTime);

//Register Idle time handlers
void ApplIdleStart(void *, double start)
{
    CpvAccess(IdleStartTime)= start; //CmiWallTimer();
    return;
}

void ApplIdleEnd(void *, double cur)
{
  if(CpvAccess(IdleStartTime) < 0)
      return;
  
  CpvAccess(IdleTime) += cur /*CmiWallTimer()*/-CpvAccess(IdleStartTime);
  CpvAccess(IdleStartTime)=-1;
  return;
}

void startOperation()
{
    //CmiBarrier();  //On some machines with a two way benchmark a 
                     //barrier may be necessary to prevent processors 
                     //from going out of sync

    CpvAccess(cycleNum) = 0;
    CpvAccess(msgSize) = (CpvAccess(msgSize)-CmiMsgHeaderSizeBytes)*2 + 
        CmiMsgHeaderSizeBytes;

    char *msg = (char *)CmiAlloc(CpvAccess(msgSize));
    *((int *)(msg+CmiMsgHeaderSizeBytes)) = CpvAccess(msgSize);
    
    CmiSetHandler(msg,CpvAccess(node0Handler));
    CmiSyncSendAndFree(CmiMyPe(), CpvAccess(msgSize), msg);
    
    CpvAccess(startTime) = CmiWallTimer();
    CpvAccess(IdleTime) = 0.0;
}

//Finished operation, so time it
void operationFinished(char *msg)
{
    CmiFree(msg);
    double cycle_time = 
        (1e6*(CpvAccess(endTime)-CpvAccess(startTime)))/
        (1.0*nCycles*(CpvAccess(kFactor)+1));

    double compute_time = cycle_time - 
        (1e6*(CpvAccess(IdleTime)))/(1.0*nCycles*(CpvAccess(kFactor)+1));
    
    CmiPrintf("[%d] %d \t %5.3lfus \t %5.3lfus\n", CmiMyPe(),
              CpvAccess(msgSize) - CmiMsgHeaderSizeBytes, 
              cycle_time, compute_time);
    
    if (CpvAccess(msgSize) < maxMsgSize)
        startOperation();
    else if(CmiMyPe() == 0){
        void *sendmsg = CmiAlloc(CmiMsgHeaderSizeBytes);
        CmiSetHandler(sendmsg,CpvAccess(exitHandler));
        CmiSyncBroadcastAllAndFree(CmiMsgHeaderSizeBytes,sendmsg);
    }
}

CmiHandler exitHandlerFunc(char *msg)
{
    CmiFree(msg);
    CsdExitScheduler();
    return 0;
}

//The handler that sends out K_FACTOR messages
CmiHandler node0HandlerFunc(char *msg)
{
    CpvAccess(cycleNum) ++;
    
    if (CpvAccess(cycleNum) == nCycles) {
        CpvAccess(endTime) = CmiWallTimer();
        operationFinished(msg);
    }
    else {
        CmiSetHandler(msg,CpvAccess(node1Handler));
        *((int *)(msg+CmiMsgHeaderSizeBytes)) = CpvAccess(msgSize);
        int dest = CmiNumPes() - CmiMyPe() - 1;
        
        for(int count = 0; count < CpvAccess(kFactor); count++) {
            CmiSyncSend(dest,CpvAccess(msgSize),msg);
        }
        CmiFree(msg);
    }
    
    return 0;
}

//Receive K_FACTOR messages and send an ack back
CmiHandler node1HandlerFunc(char *msg)
{
    CpvAccess(nRecvd) ++;
    if(CpvAccess(nRecvd) != CpvAccess(kFactor)) {
        CmiFree(msg);    
        return 0;
    }
    
    CpvAccess(nRecvd) = 0;
    
    int size = *((int *)(msg+CmiMsgHeaderSizeBytes));
    CmiSetHandler(msg,CpvAccess(node0Handler));
    
    int dest = CmiNumPes() - CmiMyPe() - 1;
    CmiSyncSendAndFree(dest,size,msg);
    return 0;
}

CmiStartFn mymain(int argc, char **argv)
{
    int twoway = 0;

    CpvInitialize(int,msgSize);
    CpvInitialize(int,cycleNum);
    
    CpvInitialize(int, kFactor);
    CpvInitialize(int, nRecvd);
    
    CpvAccess(msgSize)= CmiMsgHeaderSizeBytes + 8;    
    CpvAccess(kFactor) = K_FACTOR;
    CpvAccess(nRecvd) = 0;

    CpvInitialize(int,exitHandler);
    CpvAccess(exitHandler) = CmiRegisterHandler((CmiHandler) exitHandlerFunc);
    CpvInitialize(int,node0Handler);
    CpvAccess(node0Handler) = CmiRegisterHandler((CmiHandler) node0HandlerFunc);
    CpvInitialize(int,node1Handler);
    CpvAccess(node1Handler) = CmiRegisterHandler((CmiHandler) node1HandlerFunc);
    
    CpvInitialize(double,startTime);
    CpvInitialize(double,endTime);
    
    if(argc > 1)
        twoway = atoi(argv[1]);
    
    if(argc > 2)
        CpvAccess(kFactor) = atoi(argv[2]);

    int otherPe = CmiMyPe() ^ 1;
    
    CcdCallOnConditionKeep(CcdPROCESSOR_BEGIN_IDLE, ApplIdleStart, NULL);
    CcdCallOnConditionKeep(CcdPROCESSOR_END_IDLE, ApplIdleEnd, NULL);
    
    if(CmiMyPe() == 0) {
        if(!twoway)
            CmiPrintf("Starting Multiping with oneway traffic, kFactor = %d\n", 
                      CpvAccess(kFactor));
        else
            CmiPrintf("Starting Multiping with twoway traffic, kFactor = %d\n", 
                      CpvAccess(kFactor));
    }

    if ((CmiMyPe() < CmiNumPes()/2) || twoway)
        startOperation();
    
    return 0;
}

int main(int argc,char *argv[])
{
    ConverseInit(argc,argv,(CmiStartFn)mymain,0,0);
    return 0;
}
