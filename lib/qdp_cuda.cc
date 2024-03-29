// -*- c++ -*-


#include <iostream>

#include "qdp_config_internal.h" 

#include "qdp.h"
// #include "qdp_init.h"
// #include "qdp_deviceparams.h"
// #include "qdp_cuda.h"
// #include "cuda.h"
#include <map>
#include <string>

using namespace std;



namespace QDP {

  CUstream * QDPcudastreams;
  CUevent * QDPevCopied;

  CUdevice cuDevice;
  CUcontext cuContext;

  bool deviceSet = false;


  std::map<CUresult,std::string> mapCuErrorString= {
    {CUDA_SUCCESS,"CUDA_SUCCESS"},
    {CUDA_ERROR_INVALID_VALUE,"CUDA_ERROR_INVALID_VALUE"},
    {CUDA_ERROR_OUT_OF_MEMORY,"CUDA_ERROR_OUT_OF_MEMORY"},
    {CUDA_ERROR_NOT_INITIALIZED,"CUDA_ERROR_NOT_INITIALIZED"},
    {CUDA_ERROR_DEINITIALIZED,"CUDA_ERROR_DEINITIALIZED"},
    {CUDA_ERROR_PROFILER_DISABLED,"CUDA_ERROR_PROFILER_DISABLED"},
    {CUDA_ERROR_PROFILER_NOT_INITIALIZED,"CUDA_ERROR_PROFILER_NOT_INITIALIZED"},
    {CUDA_ERROR_PROFILER_ALREADY_STARTED,"CUDA_ERROR_PROFILER_ALREADY_STARTED"},
    {CUDA_ERROR_PROFILER_ALREADY_STOPPED,"CUDA_ERROR_PROFILER_ALREADY_STOPPED"},
    {CUDA_ERROR_NO_DEVICE,"CUDA_ERROR_NO_DEVICE"},
    {CUDA_ERROR_INVALID_DEVICE,"CUDA_ERROR_INVALID_DEVICE"},
    {CUDA_ERROR_INVALID_IMAGE,"CUDA_ERROR_INVALID_IMAGE"},
    {CUDA_ERROR_INVALID_CONTEXT,"CUDA_ERROR_INVALID_CONTEXT"},
    {CUDA_ERROR_CONTEXT_ALREADY_CURRENT,"CUDA_ERROR_CONTEXT_ALREADY_CURRENT"},
    {CUDA_ERROR_MAP_FAILED,"CUDA_ERROR_MAP_FAILED"},
    {CUDA_ERROR_UNMAP_FAILED,"CUDA_ERROR_UNMAP_FAILED"},
    {CUDA_ERROR_ARRAY_IS_MAPPED,"CUDA_ERROR_ARRAY_IS_MAPPED"},
    {CUDA_ERROR_ALREADY_MAPPED,"CUDA_ERROR_ALREADY_MAPPED"},
    {CUDA_ERROR_NO_BINARY_FOR_GPU,"CUDA_ERROR_NO_BINARY_FOR_GPU"},
    {CUDA_ERROR_ALREADY_ACQUIRED,"CUDA_ERROR_ALREADY_ACQUIRED"},
    {CUDA_ERROR_NOT_MAPPED,"CUDA_ERROR_NOT_MAPPED"},
    {CUDA_ERROR_NOT_MAPPED_AS_ARRAY,"CUDA_ERROR_NOT_MAPPED_AS_ARRAY"},
    {CUDA_ERROR_NOT_MAPPED_AS_POINTER,"CUDA_ERROR_NOT_MAPPED_AS_POINTER"},
    {CUDA_ERROR_ECC_UNCORRECTABLE,"CUDA_ERROR_ECC_UNCORRECTABLE"},
    {CUDA_ERROR_UNSUPPORTED_LIMIT,"CUDA_ERROR_UNSUPPORTED_LIMIT"},
    {CUDA_ERROR_CONTEXT_ALREADY_IN_USE,"CUDA_ERROR_CONTEXT_ALREADY_IN_USE"},
    {CUDA_ERROR_INVALID_SOURCE,"CUDA_ERROR_INVALID_SOURCE"},
    {CUDA_ERROR_FILE_NOT_FOUND,"CUDA_ERROR_FILE_NOT_FOUND"},
    {CUDA_ERROR_SHARED_OBJECT_SYMBOL_NOT_FOUND,"CUDA_ERROR_SHARED_OBJECT_SYMBOL_NOT_FOUND"},
    {CUDA_ERROR_SHARED_OBJECT_INIT_FAILED,"CUDA_ERROR_SHARED_OBJECT_INIT_FAILED"},
    {CUDA_ERROR_OPERATING_SYSTEM,"CUDA_ERROR_OPERATING_SYSTEM"},
    {CUDA_ERROR_INVALID_HANDLE,"CUDA_ERROR_INVALID_HANDLE"},
    {CUDA_ERROR_NOT_FOUND,"CUDA_ERROR_NOT_FOUND"},
    {CUDA_ERROR_NOT_READY,"CUDA_ERROR_NOT_READY"},
    {CUDA_ERROR_LAUNCH_FAILED,"CUDA_ERROR_LAUNCH_FAILED"},
    {CUDA_ERROR_LAUNCH_OUT_OF_RESOURCES,"CUDA_ERROR_LAUNCH_OUT_OF_RESOURCES"},
    {CUDA_ERROR_LAUNCH_TIMEOUT,"CUDA_ERROR_LAUNCH_TIMEOUT"},
    {CUDA_ERROR_LAUNCH_INCOMPATIBLE_TEXTURING,"CUDA_ERROR_LAUNCH_INCOMPATIBLE_TEXTURING"},
    {CUDA_ERROR_PEER_ACCESS_ALREADY_ENABLED,"CUDA_ERROR_PEER_ACCESS_ALREADY_ENABLED"},
    {CUDA_ERROR_PEER_ACCESS_NOT_ENABLED,"CUDA_ERROR_PEER_ACCESS_NOT_ENABLED"},
    {CUDA_ERROR_PRIMARY_CONTEXT_ACTIVE,"CUDA_ERROR_PRIMARY_CONTEXT_ACTIVE"},
    {CUDA_ERROR_CONTEXT_IS_DESTROYED,"CUDA_ERROR_CONTEXT_IS_DESTROYED"},
    {CUDA_ERROR_ASSERT,"CUDA_ERROR_ASSERT"},
    {CUDA_ERROR_TOO_MANY_PEERS,"CUDA_ERROR_TOO_MANY_PEERS"},
    {CUDA_ERROR_HOST_MEMORY_ALREADY_REGISTERED,"CUDA_ERROR_HOST_MEMORY_ALREADY_REGISTERED"},
    {CUDA_ERROR_HOST_MEMORY_NOT_REGISTERED,"CUDA_ERROR_HOST_MEMORY_NOT_REGISTERED"},
    {CUDA_ERROR_UNKNOWN,"CUDA_ERROR_UNKNOWN"}};



  void CudaLaunchKernel( CUfunction f, 
			 unsigned int  gridDimX, unsigned int  gridDimY, unsigned int  gridDimZ, 
			 unsigned int  blockDimX, unsigned int  blockDimY, unsigned int  blockDimZ, 
			 unsigned int  sharedMemBytes, CUstream hStream, void** kernelParams, void** extra )
  {
#ifdef GPU_DEBUG_DEEP
    QDP_debug_deep("CudaLaunchKernel ... ");
#endif

    CudaSyncTransferStream();
    // This call is async
    cuLaunchKernel(f, gridDimX, gridDimY, gridDimZ, 
		   blockDimX, blockDimY, blockDimZ, 
		   sharedMemBytes, hStream, kernelParams, extra);

    QDPCache::Instance().releasePrevLockSet();
    QDPCache::Instance().beginNewLockSet();

#ifdef GPU_DEBUG_DEEP
    QDPCache::Instance().printLockSets();
#endif

    if (DeviceParams::Instance().getSyncDevice()) {  
      QDP_info_primary("Pulling the brakes: device sync after kernel launch!");
      CudaDeviceSynchronize();
    }
  }

  void CudaRes(const std::string& s,CUresult ret) {
    if (ret != CUDA_SUCCESS) {
      if (mapCuErrorString.count(ret)) 
	std::cout << s << " Error: " << mapCuErrorString.at(ret) << "\n";
      else
	std::cout << s << " Error: (not known)\n";
      exit(1);
    }
  }

  //int CudaGetConfig(CUdevice_attribute what)
  int CudaGetConfig(int what)
  {
    if (!deviceSet)
      QDP_error_exit("CudaGetConfig called before device set");
    int data;
    CUresult ret;
    ret = cuDeviceGetAttribute( &data, (CUdevice_attribute)what , cuDevice );
    CudaRes("cuDeviceGetAttribute",ret);
    return data;
  }

  void CudaGetSM(int* maj,int* min) {
    CUresult ret;
    ret = cuDeviceComputeCapability( maj , min , cuDevice );
    CudaRes("cuDeviceComputeCapability",ret);
  }

  void CudaInit() {
    std::cout << "cuda init\n";
    cuInit(0);

    int deviceCount = 0;
    cuDeviceGetCount(&deviceCount);
    if (deviceCount == 0) { 
      std::cout << "There is no device supporting CUDA.\n"; 
      exit(1); 
    }
  }








  void * CudaGetKernelStream() {
    return (void*)&QDPcudastreams[KERNEL];
  }

  void CudaCreateStreams() {
    QDPcudastreams = new CUstream[2];
    for (int i=0; i<2; i++) {
      QDP_info_primary("JIT: Creating CUDA stream %d",i);
      cuStreamCreate(&QDPcudastreams[i],0);
    }
    QDP_info_primary("JIT: Creating CUDA event for transfers");
    QDPevCopied = new CUevent;
    cuEventCreate(QDPevCopied,CU_EVENT_BLOCKING_SYNC);
  }

  void CudaSyncKernelStream() {
    CUresult ret = cuStreamSynchronize(QDPcudastreams[KERNEL]);
    CudaRes("cuStreamSynchronize",ret);    
  }

  void CudaSyncTransferStream() {
    CUresult ret = cuStreamSynchronize(QDPcudastreams[TRANSFER]);
    CudaRes("cuStreamSynchronize",ret);    
  }

  void CudaRecordAndWaitEvent() {
    cuEventRecord( *QDPevCopied , QDPcudastreams[TRANSFER] );
    cuStreamWaitEvent( QDPcudastreams[KERNEL] , *QDPevCopied , 0);
  }

  void CudaSetDevice(int dev)
  {
    CUresult ret;
    std::cout << "trying to get device " << dev << "\n";
    ret = cuDeviceGet(&cuDevice, dev);
    CudaRes("",ret);
    std::cout << "trying to create a context \n";
    ret = cuCtxCreate(&cuContext, 0, cuDevice);
    CudaRes("",ret);

    deviceSet=true;
    DeviceParams::Instance().autoDetect();
  }

  void CudaGetDeviceCount(int * count)
  {
    cuDeviceGetCount( count );
  }


  bool CudaHostRegister(void * ptr , size_t size)
  {
    CUresult ret;
    int flags = 0;
    QDP_info_primary("CUDA host register ptr=%p (%u) size=%lu (%u)",ptr,(unsigned)((size_t)ptr%4096) ,(unsigned long)size,(unsigned)((size_t)size%4096));
    ret = cuMemHostRegister(ptr, size, flags);
    CudaRes("cuMemHostRegister",ret);
    return true;
  }

  
  void CudaHostUnregister(void * ptr )
  {
    CUresult ret;
    ret = cuMemHostUnregister(ptr);
    CudaRes("cuMemHostUnregister",ret);
  }
  

  bool CudaHostAlloc(void **mem , const size_t size, const int flags)
  {
    CUresult ret;
    ret = cuMemHostAlloc(mem,size,flags);
    CudaRes("cudaHostAlloc",ret);
    return ret == CUDA_SUCCESS;
  }


  void CudaHostFree(const void *mem)
  {
    CUresult ret;
    ret = cuMemFreeHost((void *)mem);
    CudaRes("cuMemFreeHost",ret);
  }



#if 0
  void CudaMemcpy( const void * dest , const void * src , size_t size)
  {
    CUresult ret;
#ifdef GPU_DEBUG_DEEP
    QDP_debug_deep("cudaMemcpy dest=%p src=%p size=%d" ,  dest , src , size );
#endif

    ret = cuMemcpy((CUdeviceptr)const_cast<void*>(dest),
		   (CUdeviceptr)const_cast<void*>(src),
		   size);

    CudaRes("cuMemcpy",ret);
  }

  void CudaMemcpyAsync( const void * dest , const void * src , size_t size )
  {
    CUresult ret;
#ifdef GPU_DEBUG_DEEP
    QDP_debug_deep("cudaMemcpy dest=%p src=%p size=%d" ,  dest , src , size );
#endif

    if (DeviceParams::Instance().getAsyncTransfers()) {
      ret = cuMemcpyAsync((CUdeviceptr)const_cast<void*>(dest),
			  (CUdeviceptr)const_cast<void*>(src),
			  size,QDPcudastreams[TRANSFER]);
    } else {
      std::cout << "using sync copy\n";
      ret = cuMemcpy((CUdeviceptr)const_cast<void*>(dest),
		     (CUdeviceptr)const_cast<void*>(src),
		     size);
    }

    CudaRes("cuMemcpyAsync",ret);
  }
#endif

  
  void CudaMemcpyH2DAsync( void * dest , const void * src , size_t size )
  {
    CUresult ret;
#ifdef GPU_DEBUG_DEEP
    QDP_debug_deep("CudaMemcpyH2DAsync dest=%p src=%p size=%d" ,  dest , src , size );
#endif

    if (DeviceParams::Instance().getAsyncTransfers()) {
      ret = cuMemcpyHtoDAsync((CUdeviceptr)const_cast<void*>(dest),
			      src,
			      size,QDPcudastreams[TRANSFER]);
    } else {
      std::cout << "using sync H2D copy\n";
      ret = cuMemcpyHtoD((CUdeviceptr)const_cast<void*>(dest),
			 src,
			 size);
    }

    CudaRes("cuMemcpyH2DAsync",ret);
  }

  void CudaMemcpyD2HAsync( void * dest , const void * src , size_t size )
  {
    CUresult ret;
#ifdef GPU_DEBUG_DEEP
    QDP_debug_deep("CudaMemcpyD2HAsync dest=%p src=%p size=%d" ,  dest , src , size );
#endif

    if (DeviceParams::Instance().getAsyncTransfers()) {
      ret = cuMemcpyDtoHAsync( dest,
			      (CUdeviceptr)const_cast<void*>(src),
			      size,QDPcudastreams[TRANSFER]);
    } else {
      std::cout << "using sync D2H copy\n";
      ret = cuMemcpyDtoH( const_cast<void*>(dest),
			 (CUdeviceptr)const_cast<void*>(src),
			 size);
    }

    CudaRes("cuMemcpyH2DAsync",ret);
  }


  void CudaMemcpyH2D( void * dest , const void * src , size_t size )
  {
    CUresult ret;
#ifdef GPU_DEBUG_DEEP
    QDP_debug_deep("CudaMemcpyH2D dest=%p src=%p size=%d" ,  dest , src , size );
#endif
    ret = cuMemcpyHtoD((CUdeviceptr)const_cast<void*>(dest), src, size);
    CudaRes("cuMemcpyH2D",ret);
  }

  void CudaMemcpyD2H( void * dest , const void * src , size_t size )
  {
    CUresult ret;
#ifdef GPU_DEBUG_DEEP
    QDP_debug_deep("CudaMemcpyD2H dest=%p src=%p size=%d" ,  dest , src , size );
#endif
    ret = cuMemcpyDtoH( dest, (CUdeviceptr)const_cast<void*>(src), size);
    CudaRes("cuMemcpyD2H",ret);
  }


  bool CudaMalloc(void **mem , size_t size )
  {
    CUresult ret;
    ret = cuMemAlloc( (CUdeviceptr*)mem,size);
#ifdef GPU_DEBUG_DEEP
    QDP_debug_deep( "CudaMalloc %p", *mem );
#endif
    CudaRes("cuMemAlloc",ret);
    return ret == CUDA_SUCCESS;
  }

  void CudaFree(const void *mem )
  {
#ifdef GPU_DEBUG_DEEP
    QDP_debug_deep( "CudaFree %p", mem );
#endif
    CUresult ret;
    ret = cuMemFree((CUdeviceptr)const_cast<void*>(mem));
    CudaRes("cuMemFree",ret);
  }

  void CudaThreadSynchronize()
  {
#ifdef GPU_DEBUG_DEEP
    QDP_debug_deep( "cudaThreadSynchronize" );
#endif
    cuCtxSynchronize();
  }

  void CudaDeviceSynchronize()
  {
#ifdef GPU_DEBUG_DEEP
    QDP_debug_deep( "cudaDeviceSynchronize" );
#endif
    CUresult ret = cuCtxSynchronize();
    CudaRes("cuCtxSynchronize",ret);
  }

}


