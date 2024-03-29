#include "qdp.h"

namespace QDP {

  const Jit::RegType JitRegType<float>::Val_t;
  const Jit::RegType JitRegType<double>::Val_t;
  const Jit::RegType JitRegType<int>::Val_t;
  const Jit::RegType JitRegType<unsigned int>::Val_t;
  const Jit::RegType JitRegType<bool>::Val_t;


  Jit::Jit(const std::string& _filename , const std::string& _funcname ) :
    filename(_filename) , funcname(_funcname)
  {
    nparam=0;
    nreg[f32]=0;
    nreg[f64]=0;
    nreg[u16]=0;
    nreg[u32]=0;
    nreg[u64]=0;
    nreg[s16]=0;
    nreg[s32]=0;
    nreg[s64]=0;
    nreg[u8]=0;
    nreg[b16]=0;
    nreg[b32]=0;
    nreg[b64]=0;
    nreg[pred]=0;
    regprefix[f32]="f";
    regprefix[f64]="d";
    regprefix[u16]="h";
    regprefix[u32]="u";
    regprefix[u64]="w";
    regprefix[s16]="q";
    regprefix[s32]="i";
    regprefix[s64]="l";
    regprefix[u8]="s";
    regprefix[b16]="x";
    regprefix[b32]="y";
    regprefix[b64]="z";
    regprefix[pred]="p";
    regptx[f32]="f32";
    regptx[f64]="f64";
    regptx[u16]="u16";
    regptx[u32]="u32";
    regptx[u64]="u64";
    regptx[s16]="s16";
    regptx[s32]="s32";
    regptx[s64]="s64";
    regptx[u8]="u8";
    regptx[b16]="b16";
    regptx[b32]="b32";
    regptx[b64]="b64";
    regptx[pred]="pred";

    int r_ctaid = getRegs( u16 , 1 );
    int r_ntid = getRegs( u16 , 1 );
    int r_mul = getRegs( u32 , 2 );
    int r_threadId_u32 = getRegs( u32 , 1 );
    r_threadId_s32 = getRegs( s32 , 1 );
    r_tid = getRegs( s32 , 1 );
    r_ntidx = getRegs( s32 , 1 );
    r_ctaid_s32 = getRegs( s32 , 1 );
    r_threadId_s32_no_index = r_threadId_s32;

    oss_tidcalc <<  "mov.u16 " << getName(r_ctaid) << ",%ctaid.x;\n";
    oss_tidcalc <<  "cvt.s32.u16 " << getName(r_ctaid_s32) << "," << getName(r_ctaid) << ";\n";
    oss_tidcalc <<  "mov.u16 " << getName(r_ntid) << ",%ntid.x;\n";
    oss_tidcalc <<  "cvt.s32.u16 " << getName(r_ntidx) << "," << getName(r_ntid) << ";\n";
    oss_tidcalc <<  "mul.wide.u16 " << getName(r_mul) << "," << getName(r_ntid) << "," << getName(r_ctaid) << ";\n";
    oss_tidcalc <<  "cvt.u32.u16 " << getName(r_mul + 1) << ",%tid.x;\n";
    oss_tidcalc <<  "cvt.s32.u32 " << getName(r_tid) << "," << getName(r_mul + 1) << ";\n";
    oss_tidcalc <<  "add.u32 " << getName(r_threadId_u32) << "," << getName(r_mul + 1)  << "," << getName(r_mul) << ";\n";

    //
    // Check for thread Id boundaries LO <= idx < HI
    //

    r_lo = addParamImmediate(oss_tidcalc,Jit::s32);
    r_hi = addParamImmediate(oss_tidcalc,Jit::s32);

    oss_tidcalc <<  "cvt.s32.u32 " << getName(r_threadId_s32) << "," << getName(r_threadId_u32) << ";\n";
    oss_tidcalc <<  "add.s32 " << getName(r_threadId_s32) << "," << getName(r_threadId_s32) << "," << getName(r_lo) << ";\n";

    int r_out_of_range = getRegs( Jit::pred , 1 );
    oss_tidcalc <<  "setp.ge.s32 " << getName(r_out_of_range) << "," << getName(r_threadId_s32) << "," << getName(r_hi) << ";\n";
    oss_tidcalc <<  "@" << getName(r_out_of_range) << " exit;\n";

    // mapCVT[to][from]
    mapCVT[s32][f32]="rni."; // float-to-int
    mapCVT[s32][f64]="rni."; // float-to-int

    mapCVT[f32][f64]="rn.";  // loss of precision

    mapCVT[f32][s32]="rn.";  // int-to-float
    mapCVT[f64][s32]="rn.";  // int-to-float
    mapCVT[f32][u32]="rn.";  // int-to-float
    mapCVT[f64][u32]="rn.";  // int-to-float
    mapCVT[f32][s64]="rn.";  // int-to-float
    mapCVT[f64][s64]="rn.";  // int-to-float
    mapCVT[f32][u64]="rn.";  // int-to-float
    mapCVT[f64][u64]="rn.";  // int-to-float


    mapCmpOp[Jit::eq]="eq";
    mapCmpOp[Jit::ne]="ne";
    mapCmpOp[Jit::lt]="lt";
    mapCmpOp[Jit::le]="le";
    mapCmpOp[Jit::gt]="gt";
    mapCmpOp[Jit::ge]="ge";
    mapCmpOp[Jit::lo]="lo";
    mapCmpOp[Jit::ls]="ls";
    mapCmpOp[Jit::hi]="hi";
    mapCmpOp[Jit::hs ]="hs";
    mapCmpOp[Jit::equ]="equ";
    mapCmpOp[Jit::neu]="neu";
    mapCmpOp[Jit::ltu]="ltu";
    mapCmpOp[Jit::leu]="leu";
    mapCmpOp[Jit::gtu]="gtu";
    mapCmpOp[Jit::geu]="geu";
    mapCmpOp[Jit::num]="num";
    mapCmpOp[Jit::nan]="nan";

    // [to] [from] 
    mapIntMul[Jit::s64][Jit::s32]="wide.";
    mapIntMul[Jit::s64][Jit::s64]="lo.";
    mapIntMul[Jit::s32][Jit::s32]="lo.";

    mapIntMul[Jit::u64][Jit::u32]="wide.";
    mapIntMul[Jit::u64][Jit::u64]="lo.";
    mapIntMul[Jit::u32][Jit::u32]="lo.";

    
    mapBitType[f32]=b32;
    mapBitType[f64]=b64;
    mapBitType[u16]=b16;
    mapBitType[u32]=b32;
    mapBitType[u64]=b64;
    mapBitType[s16]=b16;
    mapBitType[s32]=b32;
    mapBitType[s64]=b64;
    mapBitType[b32]=b32;

    mapStateSpace2String[ Jit::GLOBAL ] = "global.";
    mapStateSpace2String[ Jit::SHARED ] = "shared.";

    if (DeviceParams::Instance().getDivRnd()) {
      mapDivRnd[f32]="rn."; // rn
      mapDivRnd[f64]="rn.";
      mapSqrtRnd[f32]="rn."; // rn
      mapSqrtRnd[f64]="rn.";
    } else {
      mapDivRnd[f32]="approx."; // rn
      mapDivRnd[f64]="approx.";
      mapSqrtRnd[f32]="approx."; // rn
      mapSqrtRnd[f64]="approx.";
    }
  }

  void Jit::asm_bar_sync(int barrier)
  {
    oss_prg << "bar.sync " << barrier << ";\n";
  }

  int Jit::getBlockDimX() {
    return r_ntidx;
  }

  int Jit::getTID() {
    return r_tid;
  }

  void Jit::set_state_space( int reg , StateSpace st )
  {
    mapStateSpace[reg]=st;
  }

  int Jit::addParamImmediate(std::ostream& oss,RegType type){
    if (paramtype.size() != nparam) {
      std::cout << "error paramtype.size() != nparam\n";
      exit(1);
    }
    paramtype.push_back(type);
    std::ostringstream tmp;
    tmp << ".param ." << regptx[type] << " param" << nparam;
    param.push_back(tmp.str());
    int r_ret = getRegs( type , 1 );
    oss << "ld.param." << regptx[type] << " " << getName(r_ret) << ",[param" << nparam << "];\n";
    nparam++;
    return r_ret;
  }

  int Jit::getRegHi() {
    return r_hi;
  }

  int Jit::getRegId(int id) const {
    return id & ((1 << RegTypeShift)-1);
  }

  Jit::RegType Jit::getRegType(int id) const {
    return (RegType)(id >> RegTypeShift);
  }


  void Jit::asm_mov(int dest,int src)
  {
    if ( getRegType(dest) != getRegType(src) ) {
      std::cout << "JIT::asm_mov trying to mov types " << getRegType(dest) << " " << getRegType(src) << "\n";
      exit(1);
    }
    oss_prg << "mov." << regptx[getRegType(dest)] << " " << getName(dest) << "," << getName(src) << ";\n";
  }

  void Jit::asm_cond_mov(int pred,int dest,int src)
  {
    if ( getRegType(dest) != getRegType(src) || getRegType(pred) != Jit::pred ) {
      std::cout << "JIT::asm_cond_mov type mismatch " << getRegType(dest) << " " << getRegType(pred) << " " << getRegType(src) << "\n";
      exit(1);
    }
    oss_prg << "@" << getName(pred) << " mov." << regptx[getRegType(dest)] << " " << getName(dest) << "," << getName(src) << ";\n";
  }

  void Jit::asm_selp(int dest,int r0,int r1,int pred)
  {
    if ( getRegType(dest) != getRegType(r0) || getRegType(dest) != getRegType(r1) || getRegType(pred) != Jit::pred ) {
      std::cout << "JIT::asm_selp type mismatch " << getRegType(dest) << " " << getRegType(r0) << " " << getRegType(r1) << " " << getRegType(pred) << "\n";
      exit(1);
    }
    oss_prg << "selp." << regptx[getRegType(dest)] << " " << getName(dest) << "," << getName(r0) << "," << getName(r1) << "," << getName(pred) << ";\n";
  }



  void Jit::asm_st(int base,int offset,int src)
  {
    if (mapStateSpace.count(base) < 1)
      QDP_error_exit("Jit::asm_st: No state space information");
    oss_prg << "st." << mapStateSpace2String.at( mapStateSpace.at(base) ) << regptx[getRegType(src)] << " [" << getName(base) << "+" << offset << "]," << getName(src) << ";\n";
  }

  void Jit::asm_cond_st(int pred,int base,int offset,int src)
  {
    if ( getRegType(pred) != Jit::pred )
      QDP_error_exit("Jit::asm_cond_st not pred type");
    if (mapStateSpace.count(base) < 1)
      QDP_error_exit("Jit::asm_st: No state space information");
    oss_prg << "@" << getName(pred) << " st." << mapStateSpace2String.at( mapStateSpace.at(base) ) << regptx[getRegType(src)] << " [" << getName(base) << "+" << offset << "]," << getName(src) << ";\n";
  }

  void Jit::asm_ld(int dest,int base,int offset)
  {
    if (mapStateSpace.count(base) < 1)
      QDP_error_exit("Jit::asm_ld: No state space information dest_type=%d offset=%d",getRegType(dest),offset);
    oss_prg << "ld." << mapStateSpace2String.at( mapStateSpace.at(base) ) << regptx[getRegType(dest)] << " " << getName(dest) << ",[" << getName(base) << "+" << offset << "];\n";
  }

  void Jit::asm_add(int dest,int lhs,int rhs)
  {
    if ( getRegType(dest) != getRegType(rhs) || getRegType(dest) != getRegType(lhs) ) {
      std::cout << "JIT::asm_add: trying to add different types " << getRegType(dest) << " " << getRegType(lhs) << " " << getRegType(rhs) << "\n";
      exit(1);
    }
    oss_prg << "add." << regptx[getRegType(dest)] << " " << getName(dest) << "," << getName(lhs) << "," << getName(rhs) << ";\n";
  }

  void Jit::asm_float_as_int(int dest,int src)
  {
    if ( getRegType(dest) != Jit::u32 || getRegType(src) != Jit::f32 ) {
      std::cout << "JIT::asm_float_as_int: type mismatch " << getRegType(dest) << " " << getRegType(src) << "\n";
      exit(1);
    }
    oss_prg << "mov.b32 " << getName(dest) << "," << getName(src) << ";\n";
  }


  void Jit::internal_asinf_kernel(int dest,int a) {
    int a2 = getRegs(getRegType(dest),1);
    int t = getRegs(getRegType(dest),1);
    int r0 = getRegs(getRegType(dest),1);
    int r1 = getRegs(getRegType(dest),1);
    int r2 = getRegs(getRegType(dest),1);
    int r3 = getRegs(getRegType(dest),1);
    int r4 = getRegs(getRegType(dest),1);
    asm_mov_literal(r0,(float)5.175137819e-002f);    
    asm_mov_literal(r1,(float)1.816697683e-002f);
    asm_mov_literal(r2,(float)4.675685871e-002f);
    asm_mov_literal(r3,(float)7.484657646e-002f);
    asm_mov_literal(r4,(float)1.666701424e-001f);
    asm_mul(a2,a,a);
    asm_mov(t,r0);
    asm_fma(t,t,a2,r1);
    asm_fma(t,t,a2,r2);
    asm_fma(t,t,a2,r3);
    asm_fma(t,t,a2,r4);
    asm_mul(t,t,a2);
    asm_fma(a,t,a,a);
    asm_mov(dest,a);
  }

  // Code take from CUDA math function:
#if 0
  float __internal_asinf_kernel(float a) {
    float a2, t;
    a2 = a * a;
    t =                         5.175137819e-002f;
    t = __internal_fmad (t, a2, 1.816697683e-002f);
    t = __internal_fmad (t, a2, 4.675685871e-002f);
    t = __internal_fmad (t, a2, 7.484657646e-002f);
    t = __internal_fmad (t, a2, 1.666701424e-001f);
    t = t * a2;
    a = __internal_fmad (t, a, a);
    return a;
  }
#endif


  void Jit::asm_acos(int dest,int a)
  {
    if ( getRegType(dest) != getRegType(a) || getRegType(dest) != Jit::f32 ) {
      std::cout << "JIT::asm_acos: type mismatch " << getRegType(dest) << " " << getRegType(a) << "\n";
      exit(1);
    }
    int t0 = getRegs(getRegType(dest),1);
    int t1 = getRegs(getRegType(dest),1);
    int t2 = getRegs(getRegType(dest),1);
    int r0 = getRegs( Jit::s32 , 1 );
    int r1 = getRegs(getRegType(dest),1);
    int r2 = getRegs(getRegType(dest),1);
    int r05 = getRegs(getRegType(dest),1);
    int r057 = getRegs(getRegType(dest),1);
    int rpi02 = getRegs(getRegType(dest),1);
    int rpi = getRegs(getRegType(dest),1);
    int ret0 = getRegs(getRegType(dest),1);
    int ret1 = getRegs(getRegType(dest),1);
    int ret2 = getRegs(getRegType(dest),1);
    int aint = getRegs( Jit::u32 , 1 );
    int pred = getRegs( Jit::pred , 1 );
    asm_mov_literal(r0,(int)0.0);
    asm_mov_literal(r1,(float)1.0);
    asm_mov_literal(r2,(float)2.0);
    asm_mov_literal(r05,(float)0.5);
    asm_mov_literal(r057,(float)0.57);
    asm_mov_literal(rpi,(float)3.14159);
    asm_mov_literal(rpi02,(float)3.14159/2.0);
    asm_abs(t0,a);
    asm_sub(t2,r1,t0);
    asm_mul(t2,r05,t2);
    asm_sqrt(t2,t2);
    asm_cmp(CmpOp::gt,pred, t0, r057 );
    asm_selp(t1,t2,t0,pred);
    internal_asinf_kernel(t1,t1);
    asm_mul(ret0,r2,t1);
    asm_sub(ret1,rpi02,t1);
    asm_selp(t1,ret0,ret1,pred);
    asm_float_as_int(aint,a);
    asm_cmp_type(CmpOp::lt,pred, aint, r0 , Jit::s32 );
    asm_sub(ret2,rpi,t1);
    asm_cond_mov(pred,t1,ret2);
    asm_mov(dest,t1);

    // Code take from CUDA math function:
#if 0
    float t0, t1, t2;

    t0 = fabsf(a);
    t2 = 1.0f - t0;
    t2 = 0.5f * t2;
    t2 = sqrtf(t2);
    t1 = t0 > 0.57f ? t2 : t0;
    t1 = __internal_asinf_kernel(t1);
    t1 = t0 > 0.57f ? 2.0f * t1 : CUDART_PIO2_F - t1;
    if (__float_as_int(a) < 0) {
      t1 = CUDART_PI_F - t1;
    }
    return t1;
#endif
  }


  void Jit::asm_pow(int dest,int s1,int s2)
  {
    if ( getRegType(dest) != getRegType(s1) || getRegType(dest) != getRegType(s2) ) {
      std::cout << "JIT::asm_pow: type mismatch " << getRegType(dest) << " " << getRegType(s1) << " " << getRegType(s2) << "\n";
      exit(1);
    }
    int mul = getRegs(getRegType(dest),1);
    int lg2 = getRegs(getRegType(dest),1);
    asm_lg2( lg2 , s1 );
    asm_mul( mul , lg2 , s2 );
    asm_ex2( dest , mul );
  }

  void Jit::asm_pred_to_01(int dest,int pred)
  {
    if ( getRegType(pred) != Jit::pred ) {
      std::cout << "JIT::asm_selp: type mismatch " << getRegType(dest) << " " << getRegType(pred) << "\n";
      exit(1);
    }
    oss_prg << "selp." << regptx[getRegType(dest)] << " " << getName(dest) << ",1,0," << getName(pred) << ";\n";
  }

  void Jit::asm_01_to_pred(int dest,int src)
  {
    if ( getRegType(dest) != Jit::pred ) {
      std::cout << "JIT::asm_01_to_pred: type mismatch " << getRegType(dest) << " " << getRegType(src) << "\n";
      exit(1);
    }
    oss_prg << "setp.ne." << regptx[getRegType(src)] << " " << getName(dest) << "," << getName(src) << ",0;\n";
  }

  void Jit::insert_label(const std::string& lab)
  {
    oss_prg << lab << ":\n";
  }


  void Jit::asm_cmp(CmpOp op,int pred,int lhs,int rhs)
  {
    if ( getRegType(pred) != Jit::pred || mapBitType.at(getRegType(lhs)) != mapBitType.at(getRegType(rhs)) ) {
      std::cout << "JIT::asm_cmp: type mismatch " << getRegType(pred) << " " << getRegType(lhs) << " " << getRegType(rhs) << "\n";
      exit(1);
    }
    oss_prg << "setp." << mapCmpOp.at(op) << "." << regptx[getRegType(lhs)] << " " << getName(pred) << "," << getName(lhs) << "," << getName(rhs) << ";\n";
  }

  void Jit::asm_cmp_type(CmpOp op,int pred,int lhs,int rhs,RegType cmp_as)
  {
    if ( getRegType(pred) != Jit::pred || mapBitType.at(getRegType(lhs)) != mapBitType.at(getRegType(rhs)) ) {
      std::cout << "JIT::asm_cmp_type: type mismatch " << getRegType(pred) << " " << getRegType(lhs) << " " << getRegType(rhs) << " " << cmp_as << "\n";
      exit(1);
    }
    oss_prg << "setp." << mapCmpOp.at(op) << "." << regptx[cmp_as] << " " << getName(pred) << "," << getName(lhs) << "," << getName(rhs) << ";\n";
  }

  void Jit::asm_and(int dest,int lhs,int rhs)
  {
    if ( getRegType(dest) != getRegType(rhs) || getRegType(dest) != getRegType(lhs) ) {
      std::cout << "JIT::asm_and: trying to add different types " << getRegType(dest) << " " << getRegType(lhs) << " " << getRegType(rhs) << "\n";
      exit(1);
    }
    oss_prg << "and." << regptx[ getRegType(dest) ]  << " " << getName(dest) << "," << getName(lhs) << "," << getName(rhs) << ";\n";
  }

  void Jit::asm_or(int dest,int lhs,int rhs)
  {
    if ( getRegType(dest) != getRegType(rhs) || getRegType(dest) != getRegType(lhs) ) {
      std::cout << "JIT::asm_or: trying to add different types " << getRegType(dest) << " " << getRegType(lhs) << " " << getRegType(rhs) << "\n";
      exit(1);
    }
    oss_prg << "or." << regptx[ getRegType(dest) ]  << " " << getName(dest) << "," << getName(lhs) << "," << getName(rhs) << ";\n";
  }

  void Jit::asm_not(int dest,int src)
  {
    if ( getRegType(dest) != getRegType(src) ) {
      std::cout << "JIT::asm_not: trying to add different types " 
		<< getRegType(dest) << " " 
		<< getRegType(src) << "\n";
      exit(1);
    }
    oss_prg << "not." << regptx[ getRegType(dest) ] << " " << getName(dest) << "," << getName(src) << ";\n";
  }

  void Jit::asm_sub(int dest,int lhs,int rhs)
  {
    if ( getRegType(dest) != getRegType(rhs) || getRegType(dest) != getRegType(lhs) ) {
      std::cout << "JIT::asm_sub: trying to add different types " << getRegType(dest) << " " << getRegType(lhs) << " " << getRegType(rhs) << "\n";
      exit(1);
    }
    oss_prg << "sub." << regptx[getRegType(dest)] << " " << getName(dest) << "," << getName(lhs) << "," << getName(rhs) << ";\n";
  }

  void Jit::asm_mul(int dest,int lhs,int rhs)
  {
    if (!(
      ( getRegType(dest) == Jit::u64 && getRegType(rhs) == Jit::u32 && getRegType(lhs) == Jit::u32 ) ||
      ( getRegType(dest) == Jit::s64 && getRegType(rhs) == Jit::s32 && getRegType(lhs) == Jit::s32 ) ||
      ( getRegType(dest) == getRegType(rhs) && getRegType(dest) == getRegType(lhs) ) )) {
      std::cout << "JIT::asm_mul: you are tryiong to multiply with a weird combination of types " << getRegType(dest) << " " << getRegType(lhs) << " " << getRegType(rhs) << "\n";
      exit(1);
    }
    oss_prg << "mul." << mapIntMul[getRegType(dest)][getRegType(lhs)] << regptx[getRegType(lhs)] << " " << getName(dest) << "," << getName(lhs) << "," << getName(rhs) << ";\n";
  }

  void Jit::asm_shl(int dest,int src,int bits)
  {
    if ( mapBitType.at(getRegType(dest)) != mapBitType.at(getRegType(src)) || getRegType(bits) != Jit::u32 ) {
      std::cout << "JIT::asm_shl: type mismatch " << getRegType(dest) << " " << getRegType(src) << " " << getRegType(bits) << "\n";
      exit(1);
    }
    oss_prg << "shl." << regptx.at(mapBitType.at(getRegType(src))) << " " << getName(dest) << "," << getName(src) << "," << getName(bits) << ";\n";
  }

  void Jit::asm_shr(int dest,int src,int bits)
  {
    if ( mapBitType.at(getRegType(dest)) != mapBitType.at(getRegType(src)) || getRegType(bits) != Jit::u32 ) {
      std::cout << "JIT::asm_shr: type mismatch " << getRegType(dest) << " " << getRegType(src) << " " << getRegType(bits) << "\n";
      exit(1);
    }
    oss_prg << "shr." << regptx.at(mapBitType.at(getRegType(src))) << " " << getName(dest) << "," << getName(src) << "," << getName(bits) << ";\n";
  }


  void Jit::asm_bitand(int dest,int lhs,int rhs)
  {
    if ( mapBitType.at(getRegType(dest)) != mapBitType.at(getRegType(lhs)) || 
	 mapBitType.at(getRegType(dest)) != mapBitType.at(getRegType(rhs)) ) {
      std::cout << "JIT::asm_bitand: type mismatch " << getRegType(dest) << " " << getRegType(lhs) << " " << getRegType(rhs) << "\n";
      exit(1);
    }
    oss_prg << "and." << regptx.at(mapBitType.at(getRegType(dest))) << " " << getName(dest) << "," << getName(lhs) << "," << getName(rhs) << ";\n";
  }


  void Jit::asm_bitor(int dest,int lhs,int rhs)
  {
    if ( mapBitType.at(getRegType(dest)) != mapBitType.at(getRegType(lhs)) || 
	 mapBitType.at(getRegType(dest)) != mapBitType.at(getRegType(rhs)) ) {
      std::cout << "JIT::asm_bitor: type mismatch " << getRegType(dest) << " " << getRegType(lhs) << " " << getRegType(rhs) << "\n";
      exit(1);
    }
    oss_prg << "or." << regptx.at(mapBitType.at(getRegType(dest))) << " " << getName(dest) << "," << getName(lhs) << "," << getName(rhs) << ";\n";
  }


  void Jit::asm_div(int dest,int lhs,int rhs)
  {
    if ( getRegType(dest) != getRegType(rhs) || getRegType(dest) != getRegType(lhs) ) {
      std::cout << "JIT::asm_div: trying to add different types " << getRegType(dest) << " " << getRegType(lhs) << " " << getRegType(rhs) << "\n";
      exit(1);
    }
    oss_prg << "div." << mapDivRnd[getRegType(dest)] << regptx[getRegType(dest)] << " " << getName(dest) << "," << getName(lhs) << "," << getName(rhs) << ";\n";
  }

  void Jit::asm_fma(int dest,int lhs,int rhs,int add)
  {
    if ( getRegType(dest) != getRegType(rhs) || getRegType(dest) != getRegType(lhs) || getRegType(dest) != getRegType(add) ) {
      std::cout << "JIT::asm_mul: trying to add different types " 
		<< getRegType(dest) << " " 
		<< getRegType(lhs) << " " 
		<< getRegType(rhs) << " "
		<< getRegType(add) << "\n";
      exit(1);
    }
    oss_prg << "fma.rn." << regptx[getRegType(dest)] << " " << getName(dest) << "," << getName(lhs) << "," << getName(rhs) << "," << getName(add) << ";\n";
  }

  void Jit::asm_neg(int dest,int src)
  {
    if ( getRegType(dest) != getRegType(src) ) {
      std::cout << "JIT::asm_mul: trying to add different types " 
		<< getRegType(dest) << " " 
		<< getRegType(src) << "\n";
      exit(1);
    }
    oss_prg << "neg." << regptx[getRegType(dest)] << " " << getName(dest) << "," << getName(src) << ";\n";
  }

  void Jit::asm_abs(int dest,int src)
  {
    if ( getRegType(dest) != getRegType(src) ) {
      std::cout << "JIT::asm_abs: trying to add different types " 
		<< getRegType(dest) << " " 
		<< getRegType(src) << "\n";
      exit(1);
    }
    oss_prg << "abs." << regptx[getRegType(dest)] << " " << getName(dest) << "," << getName(src) << ";\n";
  }

#if 0
  int Jit::getF32(int in) 
  {
    int ret = in;
    if (getRegType(in) != Jit::f32) {
      ret = getRegs( Jit::f32 , 1 );
      asm_cvt( ret , in );
    }
    return ret;
  }
#endif

  void Jit::asm_cos(int dest,int src)
  {
    if ( getRegType(dest) != getRegType(src) || getRegType(src) != Jit::f32 ) {
      std::cout << "JIT::asm_cos: type mismatch " 
		<< getRegType(dest) << " " 
		<< getRegType(src) << "\n";
      exit(1);
    }
    oss_prg << "cos.approx." << regptx[getRegType(dest)] << " " << getName(dest) << "," << getName(src) << ";\n";
  }

  void Jit::asm_sin(int dest,int src)
  {
    if ( getRegType(dest) != getRegType(src) || getRegType(src) != Jit::f32 ) {
      std::cout << "JIT::asm_sin: type mismatch " 
		<< getRegType(dest) << " " 
		<< getRegType(src) << "\n";
      exit(1);
    }
    oss_prg << "sin.approx." << regptx[getRegType(dest)] << " " << getName(dest) << "," << getName(src) << ";\n";
  }

  void Jit::asm_ex2(int dest,int src)
  {
    if ( getRegType(dest) != getRegType(src) || getRegType(src) != Jit::f32 ) {
      std::cout << "JIT::asm_ex2: type mismatch " 
		<< getRegType(dest) << " " 
		<< getRegType(src) << "\n";
      exit(1);
    }
    oss_prg << "ex2.approx." << regptx[getRegType(dest)] << " " << getName(dest) << "," << getName(src) << ";\n";
  }

  void Jit::asm_lg2(int dest,int src)
  {
    if ( getRegType(dest) != getRegType(src) || getRegType(src) != Jit::f32 ) {
      std::cout << "JIT::asm_lg2: type mismatch " 
		<< getRegType(dest) << " " 
		<< getRegType(src) << "\n";
      exit(1);
    }
    oss_prg << "lg2.approx." << regptx[getRegType(dest)] << " " << getName(dest) << "," << getName(src) << ";\n";
  }

  void Jit::asm_sqrt(int dest,int src)
  {
    if ( getRegType(dest) != getRegType(src) ) {
      std::cout << "JIT::asm_sqrt: type mismatch " 
		<< getRegType(dest) << " " 
		<< getRegType(src) << "\n";
      exit(1);
    }
    oss_prg << "sqrt." << mapSqrtRnd[getRegType(dest)] << regptx[getRegType(dest)] << " " << getName(dest) << "," << getName(src) << ";\n";
  }


  void Jit::asm_cvt(int dest,int src)
  {
    if ( getRegType(dest) == getRegType(src) ) {
      std::cout << "JIT::asm_cvt: trying to convert different types " 
		<< getRegType(dest) << " " 
		<< getRegType(src) << "\n";
      exit(1);
    }
    if (getRegType(dest) != Jit::pred  &&  getRegType(src) == Jit::pred) {
      int u32 = getRegs( Jit::u32 , 1 );
      asm_pred_to_01( u32 , src );
      oss_prg << "cvt." << mapCVT[getRegType(dest)][getRegType(u32)] << regptx[getRegType(dest)] << "." << regptx[getRegType(u32)] << " " << getName(dest) << "," << getName(u32) << ";\n";
    } else {
      oss_prg << "cvt." << mapCVT[getRegType(dest)][getRegType(src)] << regptx[getRegType(dest)] << "." << regptx[getRegType(src)] << " " << getName(dest) << "," << getName(src) << ";\n";
    }
  }


  std::string Jit::getName(int id) const 
  {
    RegType type = getRegType(id);
    int reg = getRegId(id);
    std::ostringstream tmp;
    tmp << regprefix[type];
    tmp << reg;
    return tmp.str();
  }

  int Jit::getRegs(RegType type,int count) 
  {
    int reg = nreg[type];
    nreg[type] += count;
    return  reg | (type << RegTypeShift);
  }

  void Jit::dumpVarDefType(RegType type) 
  {
    if (nreg[type] > 0)
      oss_vardef << ".reg ." << regptx[type] << " " << regprefix[type] << "<" << nreg[type] << ">;\n";
  }

  void Jit::dumpVarDef() 
  {
    dumpVarDefType(f32);
    dumpVarDefType(f64);
    dumpVarDefType(u16);
    dumpVarDefType(u32);
    dumpVarDefType(u64);
    dumpVarDefType(s16);
    dumpVarDefType(s32);
    dumpVarDefType(s64);
    dumpVarDefType(u8);
    dumpVarDefType(b16);
    dumpVarDefType(b32);
    dumpVarDefType(b64);
    dumpVarDefType(pred);
  }


  int Jit::addGlobalMemory( size_t s , int r_idx , int idx_multiplier )
  {
    oss_vardef << ".global.u8 glob" << n_globalMem << "[" << s << "];\n";
    int r_addr = getRegs( Jit::u64 , 1 );
    int r_ret = getRegs( Jit::u64 , 1 );
    oss_baseaddr << "mov.u64 " << getName(r_addr) << ",glob" << n_globalMem << ";\n";
    oss_baseaddr << "add.u64 " << getName(r_ret) << "," << getName(r_addr) << "," << getName( getThreadIdMultiplied(r_idx,idx_multiplier) ) << ";\n";
    mapStateSpace[r_ret] = Jit::GLOBAL;
    n_globalMem++;
    return r_ret;
  }


  void Jit::dumpParam()
  {
    for (int i = 0 ; i < param.size() ; i++ ) {
      oss_param << param[i];
      if (i<param.size()-1)
	oss_param << ",\n";
      else
	oss_param << "\n";
    }
  }

  int Jit::addParam(RegType type) 
  {
    if (paramtype.size() != nparam) {
      std::cout << "error paramtype.size() != nparam\n";
      exit(1);
    }
    paramtype.push_back(type);
    std::ostringstream tmp;
    tmp << ".param ." << regptx[type] << " param" << nparam;
    param.push_back(tmp.str());
    int r_ret = getRegs( type , 1 );
    oss_baseaddr << "ld.param." << regptx[type] << " " << getName(r_ret) << ",[param" << nparam << "];\n";
    nparam++;
    return r_ret;
  }

// cvt.u32,s32 u3,s2;         // convert because current index type is "s32"
// mul.wide.u32 w2,u2,4;      // (int*) mul with sizeof(int)
// ld.param.u64 w5,[param2];  // w5 = goffset[]
// add.u64 w6,w5,w2;          // w6 = &goffset[idx]
// ld.param.s32 s0,[w6];      // s0 = goffset[idx],   s0 is the new thread idx, i.e. 'u2'

  int Jit::getRegIdx()
  {
    return r_threadId_s32;
  }

  // Needed for global reduction kernel
  int Jit::getRegIdxNoIndex()
  {
    return r_threadId_s32_no_index;
  }

  int Jit::getRegBlockIdx()
  {
    return r_ctaid_s32;
  }

  int Jit::addParamMemberArray(int r_index)
  {
    //
    // Add a parameter for the member table (array of bool)
    //
    paramtype.push_back(u64);
    std::ostringstream tmp;
    tmp << ".param .u64 param" << nparam;
    param.push_back(tmp.str());
    int r_member_addr = getRegs( u64 , 1 );
    int r_member_offs = getRegs( u64 , 1 );
    int r_member = getRegs( u8 , 1 );
    int r_member_u32 = getRegs( u32 , 1 );
    int r_isMember = getRegs( pred , 1 );
    oss_baseaddr << "ld.param.u64 " << getName(r_member_addr) << ",[param" << nparam << "];  // member array\n";
    oss_baseaddr << "add.u64 " << getName(r_member_offs) << "," << getName(r_member_addr) << "," << getName( getThreadIdMultiplied(r_index,1) ) << ";\n";
    nparam++;

    oss_prg << "//\n// Is this site a member of the subset ?\n";
    oss_prg << "ld.global.u8 " << getName(r_member) << ",[" << getName(r_member_offs) << "];\n";
    oss_prg << "cvt.u32.u8 " << getName(r_member_u32) << "," << getName(r_member) << ";\n";
    oss_prg << "setp.ne.u32 " << getName(r_isMember) << "," << getName(r_member_u32) << ",0;\n";
    oss_prg <<  "@!" << getName(r_isMember) << " exit;\n";
    oss_prg << "//\n";

  }


  int Jit::addParamIndexFieldAndOption()
  {
    if (paramtype.size() != nparam) {
      std::cout << "error paramtype.size() != nparam\n";
      exit(1);
    }

    //
    // Adds a bool (as Jit::s32) to bypass this first index
    // Useful when the "inner" sites are actually the whole local volume
    //
    paramtype.push_back(s32);
    std::ostringstream tmp;
    tmp << ".param .s32 param" << nparam;
    param.push_back(tmp.str());
    int r_do_first_idx = getRegs( s32 , 1 );
    oss_idx << "ld.param.s32 " << getName(r_do_first_idx) << ",[param" << nparam << "];\n";
    nparam++;

    int pred_first_idx = getRegs( pred , 1 );
    oss_idx <<  "setp.ne.s32 " << getName(pred_first_idx) << "," << getName(r_do_first_idx) << ",0;\n";

    

    int idx_u32 = getRegs( u32 , 1 );
    int idx_u32_mul_4 = getRegs( u64 , 1 );
    oss_idx << "@" << getName(pred_first_idx) << "  " <<  "cvt.u32.s32 " << getName(idx_u32) << "," << getName(r_threadId_s32) << ";\n";
    oss_idx << "@" << getName(pred_first_idx) << "  " <<  "mul.wide.u32 " << getName(idx_u32_mul_4) << "," << getName(idx_u32) << ",4;\n";

    //
    // Now add the parameter to the index field (in case needed)
    //
    paramtype.push_back(u64);
    tmp.str("");
    tmp << ".param .u64 param" << nparam;
    param.push_back(tmp.str());
    int r_param = getRegs( u64 , 1 );
    oss_idx << "@" << getName(pred_first_idx) << "  " << "ld.param.u64 " << getName(r_param) << ",[param" << nparam << "];\n";    
    nparam++;

    int r_param_p_idx = getRegs( u64 , 1 );
    oss_idx << "@" << getName(pred_first_idx) << "  " << "add.u64 " << getName(r_param_p_idx) << "," << getName(r_param) << "," << getName(idx_u32_mul_4) << ";\n";

    r_threadId_s32_no_index = r_threadId_s32;
    r_threadId_s32 = getRegs( s32 , 1 );
    oss_idx << "@" << getName(pred_first_idx) << "  " << "ld.global.s32 " << getName(r_threadId_s32) << ",[" << getName(r_param_p_idx) << "];\n";

    //
    // In case we don't do the first index just copy the old index
    //
    oss_idx << "@!" << getName(pred_first_idx) << "  " << "mov.s32 " << getName(r_threadId_s32) << "," << getName(r_threadId_s32_no_index) << ";\n";

    return r_threadId_s32;
  }


  int Jit::getSDATA()
  {
    if (r_sdata == -1) {
      r_sdata = getRegs( u64 , 1 );
      oss_baseaddr << "mov.u64 " << getName(r_sdata) << ",sdata;  // shared memory\n";
    }
    return r_sdata;
  }


  int Jit::addSharedMemLatticeBaseAddr(int r_idx,int idx_multiplier)
  {
    usesSharedMem = true;
    int r_ret = getRegs( u64 , 1 );
    oss_baseaddr << "add.u64 " << getName(r_ret) << "," << getName( getSDATA() ) << "," << getName( getThreadIdMultiplied(r_idx,idx_multiplier) ) << ";\n";
    mapStateSpace[r_ret] = Jit::SHARED;
    return r_ret;
  }

#if 1
  Jit::IndexRet Jit::addParamIndexFieldRcvBuf(int r_idx,int wordSize)
  {
    if (paramtype.size() != nparam) {
      std::cout << "error paramtype.size() != nparam\n";
      exit(1);
    }
    int idx_u32 = getRegs( u32 , 1 );
    int idx_u32_mul_4 = getRegs( u64 , 1 );
    oss_idx <<  "cvt.u32.s32 " << getName(idx_u32) << "," << getName(r_idx) << ";\n";
    oss_idx <<  "mul.wide.u32 " << getName(idx_u32_mul_4) << "," << getName(idx_u32) << ",4;\n";

    paramtype.push_back(u64);
    std::ostringstream tmp;
    tmp << ".param .u64 param" << nparam;
    param.push_back(tmp.str());
    int r_param_goff = getRegs( u64 , 1 );
    oss_idx << "ld.param.u64 " << getName(r_param_goff) << ",[param" << nparam << "];\n";    
    nparam++;

    int r_param_goff_p_idx = getRegs( u64 , 1 );
    oss_idx << "add.u64 " << getName(r_param_goff_p_idx) << "," << getName(r_param_goff) << "," << getName(idx_u32_mul_4) << ";\n";

    r_threadId_s32 = getRegs( s32 , 1 );
    oss_idx << "ld.global.s32 " << getName(r_threadId_s32) << ",[" << getName(r_param_goff_p_idx) << "]; // new_idx\n";

    int r_pred_in_buf = getRegs( pred , 1 );
    oss_idx << "setp.lt.s32 " << getName(r_pred_in_buf) << "," << getName(r_threadId_s32) << ",0; // in rcv buffer ?\n";

    oss_idx << "@" << getName(r_pred_in_buf) << "  neg.s32 " << getName(r_threadId_s32) << "," << getName(r_threadId_s32) << "; // rcv buf index\n";
    oss_idx << "@" << getName(r_pred_in_buf) << "  sub.s32 " << getName(r_threadId_s32) << "," << getName(r_threadId_s32) << ",1; // rcv buf index\n";


    paramtype.push_back(u64);
    tmp.str(std::string());
    tmp << ".param .u64 param" << nparam;
    param.push_back(tmp.str());
    int r_param_rcv_buf_base = getRegs( u64 , 1 );
    int r_param_rcv_buf_idx = getRegs( u64 , 1 );
    oss_baseaddr << "ld.param.u64 " << getName(r_param_rcv_buf_base) << ",[param" << nparam << "];   // recv buf base addr\n";    
    oss_baseaddr << "add.u64 " << getName(r_param_rcv_buf_idx) << "," << getName(r_param_rcv_buf_base) << "," << getName( getThreadIdMultiplied(r_threadId_s32,wordSize) ) << ";\n";
    nparam++;

    


    IndexRet ret;
    ret.r_newidx = r_threadId_s32;
    ret.r_pred_in_buf = r_pred_in_buf;
    ret.r_rcvbuf = r_param_rcv_buf_idx;

    mapStateSpace[ret.r_rcvbuf] = Jit::GLOBAL;

    return ret;
  }
#else
  Jit::IndexRet Jit::addParamIndexFieldRcvBuf(int wordSize)
  {
    if (paramtype.size() != nparam) {
      std::cout << "error paramtype.size() != nparam\n";
      exit(1);
    }
    int idx_u32 = getRegs( u32 , 1 );
    int idx_u32_mul_4 = getRegs( u64 , 1 );
    oss_idx <<  "cvt.u32.s32 " << getName(idx_u32) << "," << getName(r_threadId_s32) << ";\n";
    oss_idx <<  "mul.wide.u32 " << getName(idx_u32_mul_4) << "," << getName(idx_u32) << ",4;\n";

    paramtype.push_back(u64);
    std::ostringstream tmp;
    tmp << ".param .u64 param" << nparam;
    param.push_back(tmp.str());
    int r_param_goff = getRegs( u64 , 1 );
    oss_idx << "ld.param.u64 " << getName(r_param_goff) << ",[param" << nparam << "];\n";    
    nparam++;

    int r_param_goff_p_idx = getRegs( u64 , 1 );
    oss_idx << "add.u64 " << getName(r_param_goff_p_idx) << "," << getName(r_param_goff) << "," << getName(idx_u32_mul_4) << ";\n";

    r_threadId_s32 = getRegs( s32 , 1 );
    oss_idx << "ld.global.s32 " << getName(r_threadId_s32) << ",[" << getName(r_param_goff_p_idx) << "]; // new_idx\n";

    int r_pred_in_buf = getRegs( pred , 1 );
    oss_idx << "setp.lt.s32 " << getName(r_pred_in_buf) << "," << getName(r_threadId_s32) << ",0; // in rcv buffer ?\n";

    oss_idx << "@" << getName(r_pred_in_buf) << "  neg.s32 " << getName(r_threadId_s32) << "," << getName(r_threadId_s32) << "; // rcv buf index\n";
    oss_idx << "@" << getName(r_pred_in_buf) << "  sub.s32 " << getName(r_threadId_s32) << "," << getName(r_threadId_s32) << ",1; // rcv buf index\n";


    paramtype.push_back(u64);
    tmp.str(std::string());
    tmp << ".param .u64 param" << nparam;
    param.push_back(tmp.str());
    int r_param_rcv_buf_base = getRegs( u64 , 1 );
    int r_param_rcv_buf_idx = getRegs( u64 , 1 );
    oss_baseaddr << "ld.param.u64 " << getName(r_param_rcv_buf_base) << ",[param" << nparam << "];   // recv buf base addr\n";    
    oss_baseaddr << "add.u64 " << getName(r_param_rcv_buf_idx) << "," << getName(r_param_rcv_buf_base) << "," << getName( getThreadIdMultiplied(r_threadId_s32,wordSize) ) << ";\n";
    nparam++;

    


    IndexRet ret;
    ret.r_newidx = r_threadId_s32;
    ret.r_pred_in_buf = r_pred_in_buf;
    ret.r_rcvbuf = r_param_rcv_buf_idx;

    mapStateSpace[ret.r_rcvbuf] = Jit::GLOBAL;

    return ret;
  }
#endif

  int Jit::addParamLatticeBaseAddr(int r_idx,int idx_multiplier)
  {
    if (paramtype.size() != nparam) {
      std::cout << "error paramtype.size() != nparam\n";
      exit(1);
    }
    paramtype.push_back(u64);
    std::ostringstream tmp;
    tmp << ".param .u64 param" << nparam;
    param.push_back(tmp.str());

    int r_param = getRegs( u64 , 1 );
    int r_ret = getRegs( u64 , 1 );
    oss_baseaddr << "ld.param.u64 " << getName(r_param) << ",[param" << nparam << "];  // lattice type\n";
    oss_baseaddr << "add.u64 " << getName(r_ret) << "," << getName(r_param) << "," << getName( getThreadIdMultiplied(r_idx,idx_multiplier) ) << ";\n";
    nparam++;
    mapStateSpace[r_ret] = Jit::GLOBAL;
    return r_ret;
  }

  int Jit::addParamScalarBaseAddr() 
  {
    if (paramtype.size() != nparam) {
      std::cout << "error paramtype.size() != nparam\n";
      exit(1);
    }
    paramtype.push_back(u64);
    std::ostringstream tmp;
    tmp << ".param .u64 param" << nparam;
    param.push_back(tmp.str());
    int r_param = getRegs( u64 , 1 );
    oss_baseaddr << "ld.param.u64 " << getName(r_param) << ",[param" << nparam << "];  // scalar type\n";
    nparam++;
    mapStateSpace[r_param] = Jit::GLOBAL;
    return r_param;
  }

  static int branchNum = 0;
  static std::vector<int> vecBranch;

  std::string pushTarget()
  {
    std::ostringstream tmp;
    tmp << "B" << branchNum;
    vecBranch.push_back(branchNum);
    branchNum++;
    return tmp.str();
  }

  std::string popTarget()
  {
    std::ostringstream tmp;
    tmp << "B" << vecBranch.back();
    vecBranch.pop_back();
    return tmp.str();
  }

  std::string getTarget() 
  {
    std::ostringstream tmp;
    tmp << "B" << vecBranch.back();
    return tmp.str();
  }

  void Jit::addCondBranchPredToLabel(int pred,const std::string& lab)
  {
    oss_prg << "@" << getName(pred) << "  bra " << lab << ";\n";
  }

  void Jit::addBranchToLabel(const std::string& lab)
  {
    oss_prg << "bra " << lab << ";\n";
  }


  void Jit::addCondBranch_if(IndexRet i)
  {
    oss_prg << "@" << getName(i.r_pred_in_buf) << "  bra " << pushTarget() << ";\n";
  }

  void Jit::addCondBranch_else()
  {
    oss_prg << "bra " << getTarget() << "D;\n";
    oss_prg << getTarget() << ":\n";
  }

  void Jit::addCondBranch_fi()
  {
    oss_prg << popTarget() << "D:\n";
  }

  void Jit::addCondBranchPred_if(int pred)
  {
    oss_prg << "@!" << getName(pred) << "  bra " << pushTarget() << ";\n";
  }

  void Jit::addCondBranchPred_else()
  {
    oss_prg << "bra " << getTarget() << "D;\n";
    oss_prg << getTarget() << ":\n";
  }

  void Jit::addCondBranchPred_fi()
  {
    oss_prg << popTarget() << "D:\n";
  }

  int Jit::getThreadIdMultiplied(int r_idx,int wordSize)
  {
    if (mapRegMul.count(r_idx) < 1) {
      int tmp = getRegs( u64 , 1 );
      int r_idx_u32 = getRegs( u32 , 1 );
      oss_tidmulti << "cvt.u32.s32 " << getName(r_idx_u32) << "," << getName(r_idx) << ";\n";
      oss_tidmulti << "mul.wide.u32 " << getName(tmp) << "," << getName(r_idx_u32) << "," << wordSize << ";\n";
      mapRegMul[r_idx][wordSize]=tmp;
      return tmp;
    } else {
      if (mapRegMul.at(r_idx).count(wordSize) < 1) {
	int tmp = getRegs( u64 , 1 );
	int r_idx_u32 = getRegs( u32 , 1 );
	oss_tidmulti << "cvt.u32.s32 " << getName(r_idx_u32) << "," << getName(r_idx) << ";\n";
	oss_tidmulti << "mul.wide.u32 " << getName(tmp) << "," << getName(r_idx_u32) << "," << wordSize << ";\n";
	mapRegMul.at(r_idx)[wordSize]=tmp;
	return tmp;
      } else {
	return mapRegMul.at(r_idx).at(wordSize);
      }
    }
  }


  void Jit::setPrettyFunction(const std::string& p) 
  {
    prettyFunction = p;    
  }


  void Jit::write() 
  {
    dumpVarDef();
    dumpParam();

    std::ofstream out(filename.c_str());
    out << "// " << prettyFunction << "\n";
    out << ".version 2.3\n" <<
      ".target sm_" << 
      DeviceParams::Instance().getMajor() <<
      DeviceParams::Instance().getMinor() << "\n";
    if (usesSharedMem)
      out << ".extern .shared .align 4 .b8 sdata[];\n";
    out << ".entry " << funcname << " (" <<
      oss_param.str() << ")\n" <<
      "{\n" <<
      oss_vardef.str() <<
      "//\n// Thread ID calculation\n" <<
      oss_tidcalc.str() <<
      "//\n// Index calculation (Map)\n" <<
      oss_idx.str() <<
      "//\n// Thread ID multiplication\n" <<
      oss_tidmulti.str() <<
      "//\n// Base addresses\n" <<
      oss_baseaddr.str() <<
      "//\n// Main body\n" <<
      oss_prg.str() <<
      "}\n";
    out.close();
  } 


}
