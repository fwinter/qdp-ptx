#ifndef QDP_JIT_
#define QDP_JIT_

#include<iostream>
#include<sstream>
#include<fstream>
#include<map>
#include<vector>
#include<string>
#include<functional>
#include<stdlib.h>

namespace QDP {

  class Jit {
  public:
    enum LatticeLayout { COAL=0,SCAL=1 };
    enum { RegTypeShift = 24 };
    enum RegType { f32=0,f64=1,u16=2,u32=3,u64=4,s16=5,s32=6,s64=7,u8=8,b16=9,b32=10,b64=11,pred=12 };
    enum CmpOp { eq, ne, lt, le, gt, ge, lo, ls, hi, hs , equ, neu, ltu, leu, gtu, geu, num, nan };
    enum StateSpace { GLOBAL=0,SHARED=1 };

    struct IndexRet {
      int r_newidx;
      int r_pred_in_buf;
      int r_rcvbuf;
    };

    // Specify the PTX register type to use when doing logical operations.
    // C++ bool is 1 bytes. PTX can't operate on 8 bits types. Must cast each access.
    //typedef unsigned int PTX_Bool_C_equiv;
    //static const Jit::RegType PTX_Bool = u32;
    //static const std::string PTX_Bool_OP;//{"b32"};   // and.b32  PTX can only do logical on bittypes

    Jit(const std::string& _filename , const std::string& _funcname );

    template<class T>
    void regStoring( WordJIT<T>* wj ) {
      vecStoring.push_back( wj );
    }

    void addCondBranchPredToLabel(int pred,const std::string& lab);
    void addBranchToLabel(const std::string& lab);
    void insert_label(const std::string& lab);

    void asm_mov(int dest,int src);
    void asm_cond_mov(int pred,int dest,int src);
    template<class T>
    void asm_mov_literal(int dest,T lit) {
      oss_prg << "mov." << regptx[getRegType(dest)] << " " << getName(dest) << "," << std::scientific << lit << ";\n";
    }
    int getTID();
    //int getF32(int in);
    void asm_selp(int dest,int r0,int r1,int pred);
    void asm_float_as_int(int dest,int src);
    void asm_st(int base,int offset,int src);
    void asm_cond_st(int pred,int base,int offset,int src);
    void asm_ld(int dest,int base,int offset);
    void asm_add(int dest,int lhs,int rhs);
    void asm_pow(int dest,int s1,int s2);
    void asm_and(int dest,int lhs,int rhs);
    void asm_or(int dest,int lhs,int rhs);
    void asm_sub(int dest,int lhs,int rhs);
    void asm_mul(int dest,int lhs,int rhs);
    void asm_bitand(int dest,int lhs,int rhs);
    void asm_bitor(int dest,int lhs,int rhs);
    void asm_div(int dest,int lhs,int rhs);
    void asm_fma(int dest,int lhs,int rhs,int add);
    void internal_asinf_kernel(int dest,int a);
    void asm_shl(int dest,int src,int bits);
    void asm_shr(int dest,int src,int bits);
    void asm_neg(int dest,int src);
    void asm_abs(int dest,int src);
    void asm_not(int dest,int src);
    void asm_cvt(int dest,int src);
    void asm_pred_to_01(int dest,int pred);
    void asm_01_to_pred(int pred,int src);
    void asm_cmp(CmpOp op,int pred,int lhs,int rhs);
    void asm_cmp_type(CmpOp op,int pred,int lhs,int rhs,RegType cmp_as);
    void asm_cos(int dest,int src);
    void asm_acos(int dest,int src);
    void asm_sin(int dest,int src);
    void asm_ex2(int dest,int src);
    void asm_lg2(int dest,int src);
    void asm_sqrt(int dest,int src);
    void internal_asinf_kernel(int a);

    int addGlobalMemory( size_t s , int r_idx , int idx_multiplier );

    std::string getName(int id) const;
    int getRegs(RegType type,int count);
    void dumpVarDefType(RegType type);
    void dumpVarDef();
    void dumpParam();
    int addParam(RegType type);
    int addParamLatticeBaseAddr(int r_idx,int idx_multiplier);
    int addSharedMemLatticeBaseAddr(int r_idx,int idx_multiplier);
    int addParamScalarBaseAddr();
    int addParamIndexFieldAndOption();
    int addParamMemberArray(int r_index);
    //IndexRet addParamIndexFieldRcvBuf(int wordSize);
    IndexRet addParamIndexFieldRcvBuf(int r_idx,int wordSize);
    void addCondBranch_if(IndexRet i);
    void addCondBranch_else();
    void addCondBranch_fi();
    void addCondBranchPred_if(int pred);
    void addCondBranchPred_else();
    void addCondBranchPred_fi();
    int getThreadIdMultiplied(int r_idx,int wordSize);
    int getSDATA();
    void set_state_space( int reg , StateSpace st );
    void write();
    int getRegIdx();
    int getRegIdxNoIndex();
    int getRegBlockIdx();
    int getRegHi();
    int getBlockDimX();
    void asm_bar_sync(int barrier);
    void setPrettyFunction(const std::string& p);

  private:
    int addParamImmediate(std::ostream& oss,RegType type);
    int getRegId(int id) const;
    RegType getRegType(int id) const;

    //
    mutable std::string filename,funcname;
    mutable int r_threadId_s32;
    mutable int r_threadId_s32_no_index;
    mutable int r_tid;
    mutable int r_ctaid_s32;
    mutable int r_lo;
    mutable int r_hi;
    mutable int r_ntidx;
    mutable int r_sdata = -1;
   
    //    mutable std::map<int,int> threadIdMultiplied;
    mutable std::map<int,std::map<int,int> > mapRegMul;
    mutable int nparam;
    mutable std::map<RegType,int> nreg;
    mutable std::map<RegType,int> regsize;
    mutable std::map<RegType,std::string> regprefix;
    mutable std::map<RegType,std::string> regptx;
    mutable std::vector<std::string> param;
    mutable std::vector<RegType> paramtype;
    mutable std::ostringstream oss_prg;
    mutable std::ostringstream oss_idx;
    mutable std::ostringstream oss_tidcalc;
    mutable std::ostringstream oss_tidmulti;
    mutable std::ostringstream oss_baseaddr;
    mutable std::ostringstream oss_vardef;
    mutable std::ostringstream oss_param;
    mutable std::string param_prefix;
    mutable std::map< int , std::map< int , std::string > > mapCVT;
    mutable std::map< RegType , std::string > mapDivRnd;
    mutable std::map< RegType , std::string > mapSqrtRnd;
    mutable std::map< CmpOp , std::string > mapCmpOp;
    mutable std::map< RegType , std::map< RegType , std::string > > mapIntMul;
    mutable std::map< RegType , RegType > mapBitType;
    mutable std::map< int , StateSpace >  mapStateSpace;
    mutable std::map< StateSpace , std::string >  mapStateSpace2String;
    mutable int n_globalMem = 0;

    std::vector< BASEWordJIT* > vecStoring;
    bool usesSharedMem = false;
    std::string prettyFunction;
  };


  template <class T> struct JitRegType {};

  template <> struct JitRegType<float>        { static const Jit::RegType Val_t = Jit::f32; };
  template <> struct JitRegType<double>       { static const Jit::RegType Val_t = Jit::f64; };
  template <> struct JitRegType<int>          { static const Jit::RegType Val_t = Jit::s32; };
  template <> struct JitRegType<unsigned int> { static const Jit::RegType Val_t = Jit::u32; };
  template <> struct JitRegType<bool>         { static const Jit::RegType Val_t = Jit::pred; };

#if 0
  template<int BoolSize> struct BoolReg {};
  template<> struct BoolReg<1> { static const Jit::RegType Val_t = Jit::u8; };
  template<> struct BoolReg<2> { static const Jit::RegType Val_t = Jit::u16; };
  template<> struct BoolReg<4> { static const Jit::RegType Val_t = Jit::u32; };
  template <> struct JitRegType<bool>         { static const Jit::RegType Val_t = BoolReg< sizeof(bool) >::Val_t; }; // 
#endif

}


#endif
