// -*- C++ -*-

/*! @file
 * @brief Main type class for QDP
 */

#ifndef QDP_QDPTYPE_H
#define QDP_QDPTYPE_H

namespace QDP {


/*! \addtogroup group1 QDP main operations
 *
 *  Lattice site-wide operations that can be applied to QDPTypes.
 *  All operations can be used in expressions
 *
 * @{
 */


//! QDPType - major type class/container for all QDP objects
/*! 
 * This is the top level class all users should access for functional
 * and infix operations
 */
template<class T, class C> 
class QDPType
{
public:
  //! Type of the first argument
  typedef T Subtype_t;

  //! Type of the container class
  typedef C Container_t;

  QDPType(Jit& func_, int addr_) : C::C(func_,addr_) {}

  //! Main constructor 
  QDPType(){}

  //! Copy constructor
  QDPType(const QDPType&) {}

  //! Destructor
  ~QDPType(){}


  //---------------------------------------------------------
  // Operators

  inline
  C& assign(const typename WordType<C>::Type_t& rhs)
    {
      C* me = static_cast<C*>(this);
      typedef typename SimpleScalar<typename WordType<C>::Type_t>::Type_t  Scalar_t;
      evaluate(*me,OpAssign(),PETE_identity(Scalar_t(rhs)),all);
      return *me;
    }

  inline
  C& assign(const Zero&)
    {
      C* me = static_cast<C*>(this);
      zero_rep(*me);
      return *me;
    }

  template<class T1,class C1>
  inline
  C& assign(const QDPType<T1,C1>& rhs)
    {
      C* me = static_cast<C*>(this);
      evaluate(*me,OpAssign(),PETE_identity(rhs),all);
      return *me;
    }

  template<class T1,class C1>
  inline
  C& assign(const QDPExpr<T1,C1>& rhs)
    {
      C* me = static_cast<C*>(this);
      evaluate(*me,OpAssign(),rhs,all);
      return *me;
    }


  //! Use this for default operator=
  inline
  C& operator=(const QDPType& rhs)
    {
      return assign(rhs);
    }


  inline
  C& operator+=(const typename WordType<C>::Type_t& rhs)
    {
      C* me = static_cast<C*>(this);
      typedef typename SimpleScalar<typename WordType<C>::Type_t>::Type_t  Scalar_t;
      evaluate(*me,OpAddAssign(),PETE_identity(Scalar_t(rhs)),all);
      return *me;
    }

  template<class T1,class C1>
  inline
  C& operator+=(const QDPType<T1,C1>& rhs)
    {
      C* me = static_cast<C*>(this);
      evaluate(*me,OpAddAssign(),PETE_identity(rhs),all);
      return *me;
    }

  template<class T1,class C1>
  inline
  C& operator+=(const QDPExpr<T1,C1>& rhs)
    {
      C* me = static_cast<C*>(this);
      evaluate(*me,OpAddAssign(),rhs,all);
      return *me;
    }


  inline
  C& operator-=(const typename WordType<C>::Type_t& rhs)
    {
      C* me = static_cast<C*>(this);
      typedef typename SimpleScalar<typename WordType<C>::Type_t>::Type_t  Scalar_t;
      evaluate(*me,OpSubtractAssign(),PETE_identity(Scalar_t(rhs)),all);
      return *me;
    }

  template<class T1,class C1>
  inline
  C& operator-=(const QDPType<T1,C1>& rhs)
    {
      C* me = static_cast<C*>(this);
      evaluate(*me,OpSubtractAssign(),PETE_identity(rhs),all);
      return *me;
    }

  template<class T1,class C1>
  inline
  C& operator-=(const QDPExpr<T1,C1>& rhs)
    {
      C* me = static_cast<C*>(this);
      evaluate(*me,OpSubtractAssign(),rhs,all);
      return *me;
    }


  inline
  C& operator*=(const typename WordType<C>::Type_t& rhs)
    {
      C* me = static_cast<C*>(this);
      typedef typename SimpleScalar<typename WordType<C>::Type_t>::Type_t  Scalar_t;
      evaluate(*me,OpMultiplyAssign(),PETE_identity(Scalar_t(rhs)),all);
      return *me;
    }

  template<class T1,class C1>
  inline
  C& operator*=(const QDPType<T1,C1>& rhs)
    {
      C* me = static_cast<C*>(this);
      evaluate(*me,OpMultiplyAssign(),PETE_identity(rhs),all);
      return *me;
    }

  template<class T1,class C1>
  inline
  C& operator*=(const QDPExpr<T1,C1>& rhs)
    {
      C* me = static_cast<C*>(this);
      evaluate(*me,OpMultiplyAssign(),rhs,all);
      return *me;
    }


  inline
  C& operator/=(const typename WordType<C>::Type_t& rhs)
    {
      C* me = static_cast<C*>(this);
      typedef typename SimpleScalar<typename WordType<C>::Type_t>::Type_t  Scalar_t;
      evaluate(*me,OpDivideAssign(),PETE_identity(Scalar_t(rhs)),all);
      return *me;
    }

  template<class T1,class C1>
  inline
  C& operator/=(const QDPType<T1,C1>& rhs)
    {
      C* me = static_cast<C*>(this);
      evaluate(*me,OpDivideAssign(),PETE_identity(rhs),all);
      return *me;
    }

  template<class T1,class C1>
  inline
  C& operator/=(const QDPExpr<T1,C1>& rhs)
    {
      C* me = static_cast<C*>(this);
      evaluate(*me,OpDivideAssign(),rhs,all);
      return *me;
    }


  inline
  C& operator%=(const typename WordType<C>::Type_t& rhs)
    {
      C* me = static_cast<C*>(this);
      typedef typename SimpleScalar<typename WordType<C>::Type_t>::Type_t  Scalar_t;
      evaluate(*me,OpModAssign(),PETE_identity(Scalar_t(rhs)),all);
      return *me;
    }

  template<class T1,class C1>
  inline
  C& operator%=(const QDPType<T1,C1>& rhs)
    {
      C* me = static_cast<C*>(this);
      evaluate(*me,OpModAssign(),PETE_identity(rhs),all);
      return *me;
    }

  template<class T1,class C1>
  inline
  C& operator%=(const QDPExpr<T1,C1>& rhs)
    {
      C* me = static_cast<C*>(this);
      evaluate(*me,OpModAssign(),rhs,all);
      return *me;
    }


  inline
  C& operator|=(const typename WordType<C>::Type_t& rhs)
    {
      C* me = static_cast<C*>(this);
      typedef typename SimpleScalar<typename WordType<C>::Type_t>::Type_t  Scalar_t;
      evaluate(*me,OpBitwiseOrAssign(),PETE_identity(Scalar_t(rhs)),all);
      return *me;
    }

  template<class T1,class C1>
  inline
  C& operator|=(const QDPType<T1,C1>& rhs)
    {
      C* me = static_cast<C*>(this);
      evaluate(*me,OpBitwiseOrAssign(),PETE_identity(rhs),all);
      return *me;
    }

  template<class T1,class C1>
  inline
  C& operator|=(const QDPExpr<T1,C1>& rhs)
    {
      C* me = static_cast<C*>(this);
      evaluate(*me,OpBitwiseOrAssign(),PETE_identity(rhs),all);
      return *me;
    }


  inline
  C& operator&=(const typename WordType<C>::Type_t& rhs)
    {
      C* me = static_cast<C*>(this);
      typedef typename SimpleScalar<typename WordType<C>::Type_t>::Type_t  Scalar_t;
      evaluate(*me,OpBitwiseAndAssign(),PETE_identity(Scalar_t(rhs)),all);
      return *me;
    }

  template<class T1,class C1>
  inline
  C& operator&=(const QDPType<T1,C1>& rhs)
    {
      C* me = static_cast<C*>(this);
      evaluate(*me,OpBitwiseAndAssign(),PETE_identity(rhs),all);
      return *me;
    }

  template<class T1,class C1>
  inline
  C& operator&=(const QDPExpr<T1,C1>& rhs)
    {
      C* me = static_cast<C*>(this);
      evaluate(*me,OpBitwiseAndAssign(),rhs,all);
      return *me;
    }


  inline
  C& operator^=(const typename WordType<C>::Type_t& rhs)
    {
      C* me = static_cast<C*>(this);
      typedef typename SimpleScalar<typename WordType<C>::Type_t>::Type_t  Scalar_t;
      evaluate(*me,OpBitwiseXorAssign(),PETE_identity(Scalar_t(rhs)),all);
      return *me;
    }

  template<class T1,class C1>
  inline
  C& operator^=(const QDPType<T1,C1>& rhs)
    {
      C* me = static_cast<C*>(this);
      evaluate(*me,OpBitwiseXorAssign(),PETE_identity(rhs),all);
      return *me;
    }

  template<class T1,class C1>
  inline
  C& operator^=(const QDPExpr<T1,C1>& rhs)
    {
      C* me = static_cast<C*>(this);
      evaluate(*me,OpBitwiseXorAssign(),rhs,all);
      return *me;
    }


  inline
  C& operator<<=(const typename WordType<C>::Type_t& rhs)
    {
      C* me = static_cast<C*>(this);
      typedef typename SimpleScalar<typename WordType<C>::Type_t>::Type_t  Scalar_t;
      evaluate(*me,OpLeftShiftAssign(),PETE_identity(Scalar_t(rhs)),all);
      return *me;
    }

  template<class T1,class C1>
  inline
  C& operator<<=(const QDPType<T1,C1>& rhs)
    {
      C* me = static_cast<C*>(this);
      evaluate(*me,OpLeftShiftAssign(),PETE_identity(rhs),all);
      return *me;
    }

  template<class T1,class C1>
  inline
  C& operator<<=(const QDPExpr<T1,C1>& rhs)
    {
      C* me = static_cast<C*>(this);
      evaluate(*me,OpLeftShiftAssign(),rhs,all);
      return *me;
    }


  inline
  C& operator>>=(const typename WordType<C>::Type_t& rhs)
    {
      C* me = static_cast<C*>(this);
      typedef typename SimpleScalar<typename WordType<C>::Type_t>::Type_t  Scalar_t;
      evaluate(*me,OpRightShiftAssign(),PETE_identity(Scalar_t(rhs)),all);
      return *me;
    }

  template<class T1,class C1>
  inline
  C& operator>>=(const QDPType<T1,C1>& rhs)
    {
      C* me = static_cast<C*>(this);
      evaluate(*me,OpRightShiftAssign(),PETE_identity(rhs),all);
      return *me;
    }

  template<class T1,class C1>
  inline
  C& operator>>=(const QDPExpr<T1,C1>& rhs)
    {
      C* me = static_cast<C*>(this);
      evaluate(*me,OpRightShiftAssign(),rhs,all);
      return *me;
    }

public:
  //T* getFdev() const   { return static_cast<const C*>(this)->getFdev();}

  int getId() const   { return static_cast<const C*>(this)->getId();}


        T& elem(int i)       {return static_cast<const C*>(this)->elem(i);}
  const T& elem(int i) const {return static_cast<const C*>(this)->elem(i);}

        T& elem()       {return static_cast<const C*>(this)->elem();}
  const T& elem() const {return static_cast<const C*>(this)->elem();}
};

/*! @} */ // end of group1

//-----------------------------------------------------------------------------
// Traits classes to support return types
//-----------------------------------------------------------------------------

// Default unary(QDPType) -> QDPType
template<class T1, class C1, class Op>
struct UnaryReturn<QDPType<T1,C1>, Op> {
  typedef QDPType<typename UnaryReturn<T1, Op>::Type_t,
		  typename UnaryReturn<C1, Op>::Type_t>  Type_t;
};

// Default binary(QDPType,QDPType) -> QDPType
template<class T1, class C1, class T2, class C2, class Op>
struct BinaryReturn<QDPType<T1,C1>, QDPType<T2,C2>, Op> {
  typedef QDPType<typename BinaryReturn<T1, T2, Op>::Type_t,
		  typename BinaryReturn<C1, C2, Op>::Type_t>  Type_t;
};

// Currently, the only trinary operator is ``where'', so return 
// based on T2 and T3
// Default trinary(QDPType,QDPType,QDPType) -> QDPType
template<class T1, class C1, class T2, class C2, class T3, class C3, class Op>
struct TrinaryReturn<QDPType<T1,C1>, QDPType<T2,C2>, QDPType<T3,C3>, Op> {
  typedef QDPType<typename BinaryReturn<T2, T3, Op>::Type_t,
		  typename BinaryReturn<C2, C3, Op>::Type_t>  Type_t;
};


//-----------------------------------------------------------------------------
// We need to specialize CreateLeaf<T> for our class, so that operators
// know what to stick in the leaves of the expression tree.
//-----------------------------------------------------------------------------

template<class T, class C>
struct CreateLeaf<QDPType<T,C> >
{
  typedef QDPType<T,C> Inp_t;
  typedef Reference<Inp_t> Leaf_t;
//  typedef Inp_t Leaf_t;
  inline static
  Leaf_t make(const Inp_t &a) { return Leaf_t(a); }
};


//-----------------------------------------------------------------------------
// Specialization of LeafFunctor class for applying the EvalLeaf1
// tag to a QDPType. The apply method simply returns the array
// evaluated at the point.
//-----------------------------------------------------------------------------

template<class T, class C>
struct LeafFunctor<QDPType<T,C>, ElemLeaf>
{
  typedef Reference<T> Type_t;
//  typedef T Type_t;
  inline static Type_t apply(const QDPType<T,C> &a, const ElemLeaf &f)
    { 
      return Type_t(a.elem());
    }
};

template<class T, class C>
struct LeafFunctor<QDPType<T,C>, EvalLeaf1>
{
  typedef Reference<T> Type_t;
//  typedef T Type_t;
  inline static Type_t apply(const QDPType<T,C> &a, const EvalLeaf1 &f)
    { 
      return Type_t(a.elem(f.val1()));
    }
};



template<class T>
struct LeafFunctor<QDPType<T,OLattice<T> >, ParamLeaf>
{
  typedef QDPTypeJIT<typename JITContainerType<T>::Type_t,typename JITContainerType<OLattice<T> >::Type_t>  TypeA_t;
  //typedef typename JITContainerType< OLattice<T> >::Type_t  TypeA_t;
  typedef TypeA_t  Type_t;
  inline static Type_t apply(const QDPType<T,OLattice<T> > &a, const ParamLeaf& p)
  {
    if (p.isCoal())
      return Type_t( p.getFunc() , p.getParamLattice( WordSize<T>::Size ) , p.getLayout() );
    else
      return Type_t( p.getFunc() , p.getParamLattice( JITContainerType<T>::Type_t::Size_t * WordSize<T>::Size ) , p.getLayout() );
    //    return Type_t( p.getFunc() , p.getParamLattice( WordSize<T>::Size ) , p.getLayout() );
  }
};

template<class T>
struct LeafFunctor<QDPType<T,OScalar<T> >, ParamLeaf>
{
  typedef QDPTypeJIT<typename JITContainerType<T>::Type_t,typename JITContainerType<OScalar<T> >::Type_t>  TypeA_t;
  //typedef typename JITContainerType< OScalar<T> >::Type_t  TypeA_t;
  typedef TypeA_t  Type_t;
  inline static Type_t apply(const QDPType<T,OScalar<T> > &a, const ParamLeaf& p)
  {
    return Type_t( p.getFunc() , p.getParamScalar() , Jit::LatticeLayout::SCAL );
  }
};



template<class T, class C>
struct LeafFunctor<QDPType<T,C>, AddressLeaf>
{
  typedef int Type_t;
  inline static
  Type_t apply(const QDPType<T,C>& s, const AddressLeaf& p) 
  {
    //p.setAddr( s.getFdev() );
    p.setAddr( QDPCache::Instance().getDevicePtr( s.getId() ) );
    return 0;
  }
};



template<class T, class C>
struct LeafFunctor<QDPType<T,C>, ShiftPhase1>
{
  typedef int Type_t;
  static int apply(const QDPType<T,C> &s, const ShiftPhase1 &f) {
    return 0;
  }
};



template<class T, class C>
struct LeafFunctor<QDPType<T,C>, ShiftPhase2>
{
  typedef int Type_t;
  static int apply(const QDPType<T,C> &s, const ShiftPhase2 &f) {
    return 0;
  }
};




} // namespace QDP

#endif

