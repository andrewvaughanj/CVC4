/*********************                                                        */
/*! \file aig_bitblaster.h
 ** \verbatim
 ** Top contributors (to current version):
 **   Liana Hadarean, Mathias Preiner, Tim King
 ** This file is part of the CVC4 project.
 ** Copyright (c) 2009-2018 by the authors listed in the file AUTHORS
 ** in the top-level source directory) and their institutional affiliations.
 ** All rights reserved.  See the file COPYING in the top-level source
 ** directory for licensing information.\endverbatim
 **
 ** \brief AIG bitblaster.
 **
 ** AIG Bitblaster based on ABC.
 **/

#include "cvc4_private.h"

#ifndef __CVC4__THEORY__BV__BITBLAST__AIG_BITBLASTER_H
#define __CVC4__THEORY__BV__BITBLAST__AIG_BITBLASTER_H

#include "theory/bv/bitblast/bitblaster.h"

#include "base/tls.h"

class Abc_Obj_t_;
typedef Abc_Obj_t_ Abc_Obj_t;

class Abc_Ntk_t_;
typedef Abc_Ntk_t_ Abc_Ntk_t;

class Abc_Aig_t_;
typedef Abc_Aig_t_ Abc_Aig_t;

class Cnf_Dat_t_;
typedef Cnf_Dat_t_ Cnf_Dat_t;

namespace CVC4 {
namespace theory {
namespace bv {

class AigBitblaster : public TBitblaster<Abc_Obj_t*>
{
 public:
  AigBitblaster();
  ~AigBitblaster();

  void makeVariable(TNode node, Bits& bits) override;
  void bbTerm(TNode node, Bits& bits) override;
  void bbAtom(TNode node) override;
  Abc_Obj_t* bbFormula(TNode formula);
  bool solve(TNode query);
  static Abc_Aig_t* currentAigM();
  static Abc_Ntk_t* currentAigNtk();

 private:
  typedef std::unordered_map<TNode, Abc_Obj_t*, TNodeHashFunction> TNodeAigMap;
  typedef std::unordered_map<Node, Abc_Obj_t*, NodeHashFunction> NodeAigMap;

  static CVC4_THREAD_LOCAL Abc_Ntk_t* s_abcAigNetwork;
  std::unique_ptr<context::Context> d_nullContext;
  std::unique_ptr<prop::SatSolver> d_satSolver;
  TNodeAigMap d_aigCache;
  NodeAigMap d_bbAtoms;

  NodeAigMap d_nodeToAigInput;
  // the thing we are checking for sat
  Abc_Obj_t* d_aigOutputNode;

  void addAtom(TNode atom);
  void simplifyAig();
  void storeBBAtom(TNode atom, Abc_Obj_t* atom_bb) override;
  Abc_Obj_t* getBBAtom(TNode atom) const override;
  bool hasBBAtom(TNode atom) const override;
  void cacheAig(TNode node, Abc_Obj_t* aig);
  bool hasAig(TNode node);
  Abc_Obj_t* getAig(TNode node);
  Abc_Obj_t* mkInput(TNode input);
  bool hasInput(TNode input);
  void convertToCnfAndAssert();
  void assertToSatSolver(Cnf_Dat_t* pCnf);
  Node getModelFromSatSolver(TNode a, bool fullModel) override
  {
    Unreachable();
  }

  class Statistics
  {
   public:
    IntStat d_numClauses;
    IntStat d_numVariables;
    TimerStat d_simplificationTime;
    TimerStat d_cnfConversionTime;
    TimerStat d_solveTime;
    Statistics();
    ~Statistics();
  };

  Statistics d_statistics;
};

}  // namespace bv
}  // namespace theory
}  // namespace CVC4
#endif  //  __CVC4__THEORY__BV__BITBLAST__AIG_BITBLASTER_H
