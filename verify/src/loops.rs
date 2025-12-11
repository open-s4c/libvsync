/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

use std::{collections::{HashMap, HashSet}, hash::BuildHasher, usize};

use petgraph::{algo::tarjan_scc, graphmap::{self, NodeTrait}, prelude::GraphMap, visit::{EdgeRef, GraphBase, IntoEdgeReferences, IntoEdgesDirected, IntoNeighbors, IntoNeighborsDirected, IntoNodeIdentifiers, NodeIndexable}, Directed, EdgeType};

use crate::BoogieInstruction;


fn top_level_loops<G : GraphBase,F : FnMut(HashSet<<G as GraphBase>::NodeId>) -> ()>(cfg: &G, mut callback: F)
where
    for <'a> <G as GraphBase>::NodeId : From< <&'a G as GraphBase>::NodeId >,
    for <'a> &'a G : IntoNeighborsDirected + IntoEdgesDirected + IntoNodeIdentifiers + IntoNeighbors + NodeIndexable + IntoEdgeReferences,
    for <'a> <&'a G as GraphBase>::NodeId: Eq + std::hash::Hash + Copy + std::fmt::Debug,
        <G as GraphBase>::NodeId: Eq + std::hash::Hash + Copy,
{
    let loops = tarjan_scc(cfg)
        .into_iter()
        .filter_map( |vec|
            if vec.len() > 1 || cfg.neighbors(vec[0]).any(|j| j == vec[0]) {
                Some(
                    vec.into_iter().map(|i| <G as GraphBase>::NodeId::from(i)).collect::<HashSet<_>>())
            } else {
                None
            }
        );

    for the_loop in loops {
        callback(the_loop);
    }    
}

trait Removable<T> {
    fn remove(&mut self,to_remove : T);
}

impl <N : NodeTrait, E, Ty:EdgeType, S:BuildHasher> Removable< <Self as GraphBase>::NodeId> for GraphMap<N,E,Ty,S> {
    fn remove(&mut self, to_remove : <Self as GraphBase>::NodeId) {
        let incoming : Vec<_> = self.edges_directed(to_remove, petgraph::Direction::Incoming).map( |edge|
            (edge.source(), edge.target())
        ).collect(); 
        for (from,to) in incoming {
            self.remove_edge(from, to);
        }
    }
}

/// destroys the CFG while iterating through it.
fn all_loops<G : GraphBase, F : FnMut(HashSet<<G as GraphBase>::NodeId>, &G) -> ()>(cfg: &mut G, mut callback: F) -> HashSet< <G as GraphBase>::NodeId >
where
    G : Removable< <G as GraphBase>::NodeId >,
    for <'a> <G as GraphBase>::NodeId : From< <&'a G as GraphBase>::NodeId >,
    for <'a> <&'a G as GraphBase>::NodeId : From< <G as GraphBase>::NodeId >,
    for <'a> &'a G : IntoNeighborsDirected + IntoEdgesDirected + IntoNodeIdentifiers + IntoNeighbors + NodeIndexable + IntoEdgeReferences,
    for <'a> <&'a G as GraphBase>::NodeId: Eq + std::hash::Hash + Copy + std::fmt::Debug,
        <G as GraphBase>::NodeId: Eq + std::hash::Hash + Copy,
{

    let mut loop_headers = HashSet::new();
    loop {
        let mut new_headers = Vec::new();
        top_level_loops(cfg, |the_loop| {
            for i in the_loop.iter().copied() {
                let is_entry = cfg.edges_directed(i.into(), petgraph::Direction::Incoming).any(|edge| ! the_loop.contains(&edge.source().into()));

                if is_entry {
                    new_headers.push(i);
                    loop_headers.insert(i);
                }    
            }
            callback(the_loop, cfg);
        });

        if new_headers.is_empty() {
            break;
        }

        for i in new_headers {
            cfg.remove(i);
        }
    }
    loop_headers
}

pub fn cfg(code: &[BoogieInstruction]) -> GraphMap<usize, (), Directed> {
    let mut graph =
        graphmap::GraphMap::with_capacity(code.len() + 1, 2 * code.len() + 1);
    let label_idx = all_labels(code.iter());

    // let mut output = String::new();

    // for (key, value) in label_idx {
    //     output.push_str(&format!("{}: {}\n", key, value));
    // }

    // panic!("{}", output);

    let mut unreachable = false;
    for (i, instr) in code.iter().enumerate() {
        match &instr {
            BoogieInstruction::Label(_) => {
                unreachable  = false;
            }
            _ => {

            }
        }
        if unreachable == false {
            match &instr {
                BoogieInstruction::Return => {
                }
                BoogieInstruction::Branch(targets, cond)
                => {
                    for label in targets {
                        if !label_idx.contains_key(label) {
                            panic!("Missing label in label_idx: {}", label);
                        }
                        graph.add_edge(i, label_idx[label], ());
                    }
                    if cond != "true" {
                        graph.add_edge(i, i + 1, ());
                    }
                    else {
                        unreachable = true;
                    }
                }
                _ => {
                    graph.add_edge(i, i + 1, ());
                }
            }
        }
    }

    graph
}

pub fn loop_headers(code: &[BoogieInstruction]) -> HashSet<usize> {
    let mut graph = cfg(code);
    all_loops(&mut graph, |_,_| { })
}

fn all_labels<'a, I : Iterator<Item=&'a BoogieInstruction>>(code: I) -> HashMap<String, usize> {
    code.enumerate().filter_map(|(i, instr)| match instr {
        BoogieInstruction::Label(lab) => Some((lab.clone(), i)),
        _ => None,
    }).collect()
}

fn find_exit_name(count: &mut usize, labels: &HashSet<String>) -> String {
    loop {
        *count+=1;
        let name = format!("X{}", count);
        if ! labels.contains(&name) {
            return name;
        }
    }
}

fn insert_loop_fallthrough_back_edges(code: &mut Vec<BoogieInstruction>) {
    let loops = collect_loops(code);
    let graph = cfg(code);
    let mut fallthrough_back_edges = vec![];


    for the_loop in loops.into_iter() {
    
        let loop_headers : HashSet<_> = the_loop.iter().copied().filter(|&i| 
                graph.edges_directed(i, petgraph::Direction::Incoming).any(|edge|
                    ! the_loop.contains(&edge.source()))).collect();

        for j in the_loop {
            if loop_headers.contains(&(j+1)) {

                let mut may_fallthrough = true;

                let BoogieInstruction::Label(label) = &code[j+1] else { panic!("loop headers must be labels"); };

                if let BoogieInstruction::Branch(targets, condition) = &code[j] {
                    may_fallthrough = ! targets.contains(label) && condition != "true";
                }

                if may_fallthrough {
                    fallthrough_back_edges.push((j, label.clone()));
                }
            }
        }
    }

    fallthrough_back_edges.sort_by(|(j1,_),(j2,_)| j2.cmp(j1));

    for (j,label) in fallthrough_back_edges {
        code.insert(j+1, BoogieInstruction::Branch(vec![label], "true".to_string()));
    }
}

fn insert_loop_exit_labels(code: &mut Vec<BoogieInstruction>) {
    let mut graph = cfg(code);
    let mut loop_exits = Vec::new();
    all_loops(&mut graph, |the_loop,cfg| {
        for i in the_loop.iter().copied() {
            for edge in cfg.edges_directed(i, petgraph::Direction::Outgoing) {
                if ! the_loop.contains(&edge.target()) && edge.target() == i+1 {
                    loop_exits.push(edge.target());
                }
            }
        }
    });
    
    let mut count = 0;
    let labels = all_labels(code.iter()).keys().cloned().collect();
    loop_exits.sort();
    loop_exits.dedup();
    loop_exits.reverse();
    if let Some(BoogieInstruction::Return) = code.last() {

    } else {
        code.push(BoogieInstruction::Return);
    }

    for exit in loop_exits {
        if let BoogieInstruction::Label(_) = &code[exit] {
            // already a label - nothing to do
        } else {
            code.insert(exit, BoogieInstruction::Label(find_exit_name(&mut count, &labels)));
        }
    }
}

fn collect_loops(code : &[BoogieInstruction]) -> Vec<HashSet<usize>> {
    let mut graph = cfg(code);
    let mut loops = Vec::new();
    top_level_loops(&mut graph, |the_loop| {
        loops.push(the_loop.clone());
    });
    loops
}

fn duplicate_loops(code: &mut Vec<BoogieInstruction>) {
    /* @TODO
     * Pretty sure this function only really works for the top level loops - nested loops are not unrolled inside the duplicated code! 
    */

    let loops = collect_loops(code);
    let graph = cfg(code);
    let labels = all_labels(code.iter());

    for (loop_id, the_loop) in loops.into_iter().enumerate() {
    


        let loop_headers : HashSet<_> = the_loop.iter().copied().filter(|&i| 
                graph.edges_directed(i, petgraph::Direction::Incoming).any(|edge|
                    ! the_loop.contains(&edge.source()))).collect();


        let mut sorted_loop : Vec<_> = the_loop.iter().copied().collect();
        sorted_loop.sort();

        // first step: insert forced loop

        for &i in sorted_loop.iter() {
            code.push(transformed(&code[i], |label| transform_label(label, loop_id, LOOP_FORCED_ITER_SUFFIX), 
            |targets| {
                targets.iter().flat_map(|label| {
                    let target = labels[label];
                    if loop_headers.contains(&target) {
                        vec![transform_label(label, loop_id, LOOP_FINAL_ITER_SUFFIX)].into_iter()
                    } else if the_loop.contains(&target) {
                        vec![transform_label(label, loop_id, LOOP_FORCED_ITER_SUFFIX)].into_iter()
                    } else {
                        // loop exits are blocked off -- empty goto will become assume false
                        vec![].into_iter()
                    }
                }).collect()
            }));
            
            if graph.edges_directed(i, petgraph::Direction::Outgoing).any(|edge| edge.target() == i+1 && ! the_loop.contains(&(i+1))) {
                // indirect loop exit
                // empty jump label to block off exit!
                code.push(BoogieInstruction::Branch(vec![], "true".to_string()));
            }
        }

        // second step: insert final loop


        for &i in sorted_loop.iter() {
            code.push(transformed(&code[i], |label| transform_label(label, loop_id, LOOP_FINAL_ITER_SUFFIX), 
            |targets| {
                targets.iter().flat_map(|label| {
                    let target = labels[label];
                    if loop_headers.contains(&target) {
                        // back loops are blocked off
                        vec![].into_iter()
                    } else if the_loop.contains(&target) {
                        vec![transform_label(label, loop_id, LOOP_FINAL_ITER_SUFFIX)].into_iter()
                    } else {
                        // loop exits are kept
                        vec![label.clone()].into_iter()
                    }
                }).collect()
            }));
            
            if graph.edges_directed(i, petgraph::Direction::Outgoing).any(|edge| edge.target() == i+1 && ! the_loop.contains(&(i+1))) {
                // indirect loop exit
                assert!(i+1 < code.len());
                let BoogieInstruction::Label(label) = &code[i+1] else {
                    panic!("A loop exit point does not have a label. Please call `insert_loop_exit_labels` before using this function.")
                };
                code.push(BoogieInstruction::Branch(vec![label.clone()], "true".to_string()));
            }
        }
        
        // third step: transform incoming & internal edges to point to the new forced/final loop
        for i in loop_headers.iter().copied() {
            let BoogieInstruction::Label(my_label) = code[i].clone() else { panic!("loop header has entry edge but no label, this should be impossible.") };
            for edge in graph.edges_directed(i, petgraph::Direction::Incoming) {
                let j = edge.source();
                code[j] = transformed(&code[j], |label| label.clone(),
                |targets| {
                    let mut new_targets = Vec::new();
                    for label in targets {
                        let same_label = label == &my_label; 
                        if same_label {
                            new_targets.push(transform_label(label, loop_id, LOOP_FORCED_ITER_SUFFIX));

                            if ! the_loop.contains(&j) {  // back branches are not kept
                                new_targets.push(transform_label(label, loop_id, LOOP_FINAL_ITER_SUFFIX));
                            }
                        } 
                        if ! same_label {
                            new_targets.push(label.clone());
                        }
                    }
                    new_targets
                });
            }
        }
        
    }

}


const LOOP_FORCED_ITER_SUFFIX : &str = "FORCED_ITER";
const LOOP_FINAL_ITER_SUFFIX : &str = "FINAL_ITER";


fn transform_label(label: &String, loop_id : usize, suffix : &str) -> String {
    format!("{}_{}_{}", label, loop_id, suffix)
}

fn transformed<F : Fn(&String) -> String, G : Fn(&[String]) -> Vec<String>>(instr: &BoogieInstruction, label_transformer: F, branch_transformer: G) -> BoogieInstruction {
    match instr {
        BoogieInstruction::Branch(targets,cond) => BoogieInstruction::Branch(branch_transformer(targets), cond.clone()),
        BoogieInstruction::Label(label) => BoogieInstruction::Label(label_transformer(label)),
        _ => instr.clone(),
    }
}
 
/// Unroll all loops
pub fn unroll(code: &[BoogieInstruction]) -> Vec<BoogieInstruction> {
    
    let mut changed_code = code.into();
    insert_loop_fallthrough_back_edges(&mut changed_code);
    insert_loop_exit_labels(&mut changed_code);
    
    duplicate_loops(&mut changed_code);



    changed_code
}