#ifndef QDP_JITF_GAUSS_H
#define QDP_JITF_GAUSS_H

#include "qmp.h"

namespace QDP {

template<class T>
CUfunction
function_gaussian_build(OLattice<T>& dest ,OLattice<T>& r1 ,OLattice<T>& r2 )
{
  //  std::cout << __PRETTY_FUNCTION__ << ": entering\n";

  CUfunction func;

  std::string fname("ptxgaussian.ptx");
  Jit function(fname.c_str(),"func");

  int r_idx = function.getRegIdx();
  ParamLeaf param_leaf(function, r_idx , Jit::LatticeLayout::COAL );
  function.addParamMemberArray( param_leaf.r_idx );

  // Destination
  typedef typename LeafFunctor<OLattice<T>, ParamLeaf>::Type_t  FuncRet_t;
  FuncRet_t dest_jit(forEach(dest, param_leaf, TreeCombine()));
  FuncRet_t r1_jit(forEach(r1, param_leaf, TreeCombine()));
  FuncRet_t r2_jit(forEach(r2, param_leaf, TreeCombine()));

  fill_gaussian( dest_jit.elem(0) , r1_jit.elem(0) , r2_jit.elem(0) );

  if (Layout::primaryNode())
    function.write();
      
  QMP_barrier();

  CUresult ret;
  CUmodule cuModule;
  ret = cuModuleLoad(&cuModule, fname.c_str());
  if (ret) QDP_error_exit("Error loading CUDA module '%s'",fname.c_str());

  ret = cuModuleGetFunction(&func, cuModule, "func");
  if (ret) { std::cout << "Error getting function\n"; exit(1); }

  //std::cout << __PRETTY_FUNCTION__ << ": exiting\n";

  return func;
}

template<class T>
void 
function_gaussian_exec(CUfunction function, OLattice<T>& dest,OLattice<T>& r1,OLattice<T>& r2, const Subset& s )
{
  //std::cout << __PRETTY_FUNCTION__ << ": entering\n";

  AddressLeaf addr_leaf;

  int junk_0 = forEach(dest, addr_leaf, NullCombine());
  int junk_1 = forEach(r1, addr_leaf, NullCombine());
  int junk_2 = forEach(r2, addr_leaf, NullCombine());

  // lo <= idx < hi
  int lo = 0;
  int hi = Layout::sitesOnNode();
  void * subset_member = QDPCache::Instance().getDevicePtr( s.getIdMemberTable() );

  std::vector<void*> addr;

  addr.push_back( &lo );
  //std::cout << "addr lo = " << addr[0] << " lo=" << lo << "\n";

  addr.push_back( &hi );
  //std::cout << "addr hi = " << addr[1] << " hi=" << hi << "\n";

  addr.push_back( &subset_member );
  //std::cout << "addr subset_member = " << addr[3] << " " << subset_member << "\n";

  int addr_dest=addr.size();
  for(int i=0; i < addr_leaf.addr.size(); ++i) {
    addr.push_back( &addr_leaf.addr[i] );
    //std::cout << "addr = " << addr_leaf.addr[i] << "\n";
  }

  static int threadsPerBlock = 0;

  if (!threadsPerBlock) {
    // Auto tuning
    threadsPerBlock = jit_autotuning(function,lo,hi,&addr[0]);
  } else {
    //QDP_info_primary("Previous auto-tuning result = %d",threadsPerBlock);
  }

  //QDP_info("Launching kernel with %d threads",hi-lo);

  kernel_geom_t now = getGeom( hi-lo , threadsPerBlock );

  CudaLaunchKernel(function,   now.Nblock_x,now.Nblock_y,1,    threadsPerBlock,1,1,    0, 0, &addr[0] , 0);
}



}

#endif
