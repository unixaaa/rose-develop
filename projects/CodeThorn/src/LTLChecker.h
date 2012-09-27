// -*- C++ -*-
#ifndef __LTLCHECKER_H__
#define __LTLCHECKER_H__

// Copyright (c) 2012 Lawrence Livermore National Security, LLC.
// Produced at the Lawrence Livermore National Laboratory
// Written by Adrian Prantl <adrian@llnl.gov>.
//
// UCRL-CODE-155962.
// All rights reserved.
//
// This file is part of ROSE. For details, see http://www.rosecompiler.org/.
// Please read the COPYRIGHT file for Our Notice and for the BSD License.

#include "StateRepresentation.h"
#include "AType.h"
#include "LTL.h"
#include <map>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>

namespace LTL {
  using namespace boost;
  using namespace std;
  using namespace AType;

  typedef vector< vector<BoolLattice> > LTLProperties;
  typedef adjacency_list<vecS, vecS, bidirectionalS, const EState*> BoostTransitionGraph;
  typedef adjacency_list<listS, listS, bidirectionalS, const EState*> MutableBoostTransitionGraph;
  typedef graph_traits<BoostTransitionGraph> GraphTraits;


  /**
   * A dataflow-based checker for LTL formulae.
   * \date 2012
   * \author Adrian Prantl
   */
  class Checker {
  public:
    Checker(EStateSet& ess, TransitionGraph& g);
    /// verify the LTL formula f
    AType::BoolLattice verify(const Formula& f);

    Label collapse_transition_graph(BoostTransitionGraph &g, BoostTransitionGraph &reduced) const;

  protected:
    Label start;
    BoostTransitionGraph g;
    TransitionGraph& transitionGraph;
    EStateSet& eStateSet;
    /// the final states in the transition graph
    deque<Label> endpoints;
  };

};

#endif
