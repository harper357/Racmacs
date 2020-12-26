
#include <RcppArmadillo.h>
#include "acmap_optimization.h"
#include "acmap_titers.h"
#include "acmap_map.h"
#include "procrustes.h"
#include "ac_dimension_test.h"
#include "ac_noisy_bootstrap.h"
#include "ac_stress_blobs.h"
#include "ac_optim_map_stress.h"

#ifndef Racmacs__RacmacsWrap__h
#define Racmacs__RacmacsWrap__h

// Functions for checking classes
void check_class(
  const SEXP &sxp,
  const std::string &classname
){
  if(!Rf_inherits(sxp, classname.c_str())){
    std::string msg = "Object must be of class '" + classname + "'\n";
    Rf_error(msg.c_str());
  }
}

void check_matrix(
    const SEXP &sxp
){
  if(!Rf_isMatrix(sxp)){
    Rf_error("Object must be of class 'matrix'");
  }
}

// declaring the specialization
namespace Rcpp {

  // FROM: ACOPTIMIZATION
  template <>
  SEXP wrap(const AcOptimization& acopt){

    List out = List::create(
      _["ag_base_coords"] = acopt.get_ag_base_coords(),
      _["sr_base_coords"] = acopt.get_sr_base_coords(),
      _["min_column_basis"] = acopt.get_min_column_basis(),
      _["fixed_column_bases"] = acopt.get_fixed_column_bases(),
      _["transformation"] = acopt.get_transformation(),
      _["translation"] = acopt.get_translation(),
      _["stress"] = acopt.get_stress(),
      _["comment"] = acopt.get_comment()
    );

    // Set class attribute and return
    out.attr("class") = CharacterVector::create("acoptimization", "list");
    return out;

  }

  // FROM: ACCOORDS
  template <>
  SEXP wrap(const AcCoords& coords){
    return wrap(
      List::create(
        _["ag_coords"] = coords.ag_coords,
        _["sr_coords"] = coords.sr_coords
      )
    );
  }

  // FROM: PROCRUSTES DATA
  template <>
  SEXP wrap(const ProcrustesData &pc){
    return wrap(
      List::create(
        _["ag_dists"] = pc.ag_dists,
        _["sr_dists"] = pc.sr_dists,
        _["ag_rmsd"] = pc.ag_rmsd,
        _["sr_rmsd"] = pc.sr_rmsd,
        _["total_rmsd"] = pc.total_rmsd
      )
    );
  }

  // FROM: ACTITER
  template <>
  SEXP wrap(const AcTiter& t){
    return wrap(t.toString());
  }

  // FROM: ACTITER VECTOR
  template <>
  SEXP wrap(const std::vector<AcTiter>& titers){
    CharacterVector titers_out(titers.size());
    for(arma::uword i=0; i<titers.size(); i++){
      titers_out[i] = titers[i].toString();
    }
    return wrap(titers_out);
  }

  // FROM: ACTITERTABLE
  template <>
  SEXP wrap(const AcTiterTable& t){

    int num_ags = t.nags();
    int num_sr = t.nsr();

    CharacterMatrix titers_out(num_ags, num_sr);
    for(int ag=0;ag<num_ags;ag++){
      for(int sr=0;sr<num_sr;sr++){
        std::string titer = t.get_titer_string(ag, sr);
        titers_out(ag, sr) = titer;
      }
    }

    return wrap(titers_out);
  }

  // FROM: PROCRUSTES
  template <>
  SEXP wrap(const Procrustes& p){
    return wrap(
      List::create(
        _["R"] = p.R,
        _["tt"] = p.tt,
        _["s"] = p.s
      )
    );
  }

  // FROM: ARMA::VEC
  template <>
  SEXP wrap(const arma::vec& v){
    NumericVector out(v.size());
    for(arma::uword i=0; i<v.size(); i++){
      out[i] = v[i];
    }
    return wrap(out);
  }

  // FROM: ANTIGEN
  SEXP wrap(const AcAntigen& ag){

    List out = wrap(
      List::create(
        _["name"] = ag.get_name(),
        _["id"] = ag.get_id(),
        _["shown"] = ag.get_shown(),
        _["size"] = ag.get_size(),
        _["fill"] = ag.get_fill(),
        _["shape"] = ag.get_shape(),
        _["outline"] = ag.get_outline(),
        _["outline_width"] = ag.get_outline_width(),
        _["rotation"] = ag.get_rotation(),
        _["aspect"] = ag.get_aspect(),
        _["drawing_order"] = ag.get_drawing_order()
      )
    );

    // Set class attribute and return
    out.attr("class") = CharacterVector::create("acantigen", "list");
    return out;

  }

  // FROM: SERUM
  SEXP wrap(const AcSerum& sr){

    List out = wrap(
      List::create(
        _["name"] = sr.get_name(),
        _["id"] = sr.get_id(),
        _["shown"] = sr.get_shown(),
        _["size"] = sr.get_size(),
        _["fill"] = sr.get_fill(),
        _["shape"] = sr.get_shape(),
        _["outline"] = sr.get_outline(),
        _["outline_width"] = sr.get_outline_width(),
        _["rotation"] = sr.get_rotation(),
        _["aspect"] = sr.get_aspect(),
        _["drawing_order"] = sr.get_drawing_order()
      )
    );

    // Set class attribute and return
    out.attr("class") = CharacterVector::create("acserum", "list");
    return out;

  }

  // FROM: ACMAP
  template <>
  SEXP wrap(const AcMap& acmap){

    // Antigens
    List antigens = List::create();
    for(auto &antigen : acmap.antigens){
      antigens.push_back(as<List>(wrap(antigen)));
    }

    // Sera
    List sera = List::create();
    for(auto &serum : acmap.sera){
      sera.push_back(as<List>(wrap(serum)));
    }

    // Optimizations
    List optimizations = List::create();
    for(auto &optimization : acmap.optimizations){
      optimizations.push_back(as<List>(wrap(optimization)));
    }

    // Titer table layers
    List titer_table_layers = List::create();
    for(auto &titer_table_layer : acmap.titer_table_layers){
      titer_table_layers.push_back(as<CharacterMatrix>(wrap(titer_table_layer)));
    }

    // Titer table flat
    CharacterMatrix titer_table_flat = as<CharacterMatrix>(wrap(acmap.titer_table_flat));

    // Assemable list
    List out = List::create(
      _["name"] = acmap.name,
      _["antigens"] = antigens,
      _["sera"] = sera,
      _["optimizations"] = optimizations,
      _["titer_table_flat"] = titer_table_flat,
      _["titer_table_layers"] = titer_table_layers
    );

    // Set class attribute and return
    out.attr("class") = CharacterVector::create("acmap", "list");
    return wrap(out);

  }

  // Dimtest results
  template <>
  SEXP wrap(const DimTestOutput& dimtestout){

    List coords = List::create();
    for(auto &coord : dimtestout.coords){
      coords.push_back(as<NumericMatrix>(wrap(coord)));
    }

    List predictions = List::create();
    for(auto &prediction : dimtestout.predictions){
      predictions.push_back(as<NumericVector>(wrap(prediction)));
    }

    return wrap(
      List::create(
        _["test_indices"] = dimtestout.test_indices,
        _["dim"] = dimtestout.dim,
        _["coords"] = coords,
        _["predictions"] = predictions
      )
    );

  }

  // Noisy bootstrap results
  template <>
  SEXP wrap(const NoisyBootstrapOutput& noisybootstrapout){

    return wrap(
      List::create(
        _["ag_noise"] = noisybootstrapout.ag_noise,
        _["coords"] = noisybootstrapout.coords
      )
    );

  }

  // Stress blob results 2d
  template <>
  SEXP wrap(const StressBlobGrid2d& blobgrid){

    return wrap(
      List::create(
        _["grid"] = blobgrid.grid,
        _["coords"] = List::create(blobgrid.xcoords, blobgrid.ycoords),
        _["stress_lim"] = blobgrid.stress_lim
      )
    );

  }

  // For converting from R to C++
  // TO: std::string
  template <>
  std::string as(SEXP sxp){
    if(TYPEOF(sxp) != 9 && TYPEOF(sxp) != 16){
      Rf_error("Input must be a string");
    } else if(TYPEOF(sxp) == 16 && Rf_length(sxp) != 1){
      Rf_error("Input cannot be a character vector longer than 1");
    }
    CharacterVector charvec = as<CharacterVector>(sxp);
    String out = charvec[0];
    return out.get_cstring();
  }

  // TO: AcOptimizerOptions
  template <>
  AcOptimizerOptions as(SEXP sxp){

    List opt = as<List>(sxp);
    return AcOptimizerOptions{
      opt["dim_annealing"],
      opt["method"],
      opt["maxit"],
      opt["num_cores"],
      opt["report_progress"],
      opt["progress_bar_length"]
    };

  };


  // TO: ACOPTIMIZATION
  template <>
  AcOptimization as(SEXP sxp){

    // Check input
    check_class(sxp, "acoptimization");
    List opt = as<List>(sxp);

    // Get variables
    if(!opt.containsElementNamed("ag_base_coords")) { stop("Optimization must contain ag_base_coords"); }
    if(!opt.containsElementNamed("sr_base_coords")) { stop("Optimization must contain sr_base_coords"); }
    arma::mat ag_base_coords = as<arma::mat>(wrap(opt["ag_base_coords"]));
    arma::mat sr_base_coords = as<arma::mat>(wrap(opt["sr_base_coords"]));

    // Setup object
    AcOptimization acopt = AcOptimization(
      ag_base_coords.n_cols,
      ag_base_coords.n_rows,
      sr_base_coords.n_rows
    );

    // Populate
    acopt.set_ag_base_coords( ag_base_coords );
    acopt.set_sr_base_coords( sr_base_coords );

    if(opt.containsElementNamed("stress")) {
      acopt.set_stress( as<double>(wrap(opt["stress"])) );
    }
    if(opt.containsElementNamed("transformation")) {
      acopt.set_transformation( as<arma::mat>(wrap(opt["transformation"])) );
    }
    if(opt.containsElementNamed("translation")) {
      acopt.set_translation( as<arma::mat>(wrap(opt["translation"])) );
    }
    if(opt.containsElementNamed("comment")) {
      acopt.set_comment( as<std::string>(wrap(opt["comment"])) );
    }
    if(opt.containsElementNamed("fixed_column_bases")) {
      acopt.set_fixed_column_bases( as<arma::vec>(wrap(opt["fixed_column_bases"])) );
    }
    if(opt.containsElementNamed("min_column_basis")) {
      acopt.set_min_column_basis( as<std::string>(wrap(opt["min_column_basis"])) );
    }

    // Return the object
    return acopt;

  };

  // TO: ACTITER
  template <>
  AcTiter as(SEXP sxp){
    std::string titer = as<std::string>(sxp);
    return AcTiter(titer);
  };

  // TO: ACTITERTABLE
  template <>
  AcTiterTable as(SEXP sxp){

    check_matrix(sxp);
    CharacterMatrix titers = as<CharacterMatrix>(sxp);
    int num_ags = titers.nrow();
    int num_sr = titers.ncol();
    AcTiterTable titertable = AcTiterTable(
      num_ags,
      num_sr
    );

    for(int ag=0; ag<num_ags; ag++){
      for(int sr=0; sr<num_sr; sr++){
        titertable.set_titer_string(
          ag, sr,
          as<std::string>(titers(ag,sr))
        );
      }
    }

    return titertable;

  };

  // TO: ACTITER VECTOR
  template <>
  std::vector<AcTiter> as(SEXP sxp){
    CharacterVector titerstrings = as<CharacterVector>(sxp);
    int ntiters = titerstrings.size();
    std::vector<AcTiter> out(ntiters);
    for(int i=0; i<ntiters; i++){
      out[i] = as<AcTiter>(wrap(titerstrings(i)));
    }
    return out;
  }

  // TO: ACTITERTABLE VECTOR
  template <>
  std::vector<AcTiterTable> as(SEXP sxp){
    List list = as<List>(sxp);
    std::vector<AcTiterTable> out;
    for(int i=0; i<list.size(); i++){
      out.push_back(
        as<AcTiterTable>(wrap(
          list(i)
        ))
      );
    }
    return out;
  }

  // TO: ACANTIGEN
  template <>
  AcAntigen as(SEXP sxp){

    check_class(sxp, "acantigen");
    List list = as<List>(sxp);
    AcAntigen ag;

    if(list.containsElementNamed("name")) { ag.set_name(list["name"]); }
    if(list.containsElementNamed("id")) { ag.set_id(list["id"]); }
    if(list.containsElementNamed("shown")) { ag.set_shown(list["shown"]); }
    if(list.containsElementNamed("size")) { ag.set_size(list["size"]); }
    if(list.containsElementNamed("fill")) { ag.set_fill(list["fill"]); }
    if(list.containsElementNamed("shape")) { ag.set_shape(list["shape"]); }
    if(list.containsElementNamed("outline")) { ag.set_outline(list["outline"]); }
    if(list.containsElementNamed("outline_width")) { ag.set_outline_width(list["outline_width"]); }
    if(list.containsElementNamed("rotation")) { ag.set_rotation(list["rotation"]); }
    if(list.containsElementNamed("aspect")) { ag.set_aspect(list["aspect"]); }
    if(list.containsElementNamed("drawing_order")) { ag.set_drawing_order(list["drawing_order"]); }

    return ag;

  }

  // TO: ACSERUM
  template <>
  AcSerum as(SEXP sxp){

    check_class(sxp, "acserum");
    List list = as<List>(sxp);
    AcSerum sr;

    if(list.containsElementNamed("name")) { sr.set_name(list["name"]); }
    if(list.containsElementNamed("id")) { sr.set_id(list["id"]); }
    if(list.containsElementNamed("shown")) { sr.set_shown(list["shown"]); }
    if(list.containsElementNamed("size")) { sr.set_size(list["size"]); }
    if(list.containsElementNamed("fill")) { sr.set_fill(list["fill"]); }
    if(list.containsElementNamed("shape")) { sr.set_shape(list["shape"]); }
    if(list.containsElementNamed("outline")) { sr.set_outline(list["outline"]); }
    if(list.containsElementNamed("outline_width")) { sr.set_outline_width(list["outline_width"]); }
    if(list.containsElementNamed("rotation")) { sr.set_rotation(list["rotation"]); }
    if(list.containsElementNamed("aspect")) { sr.set_aspect(list["aspect"]); }
    if(list.containsElementNamed("drawing_order")) { sr.set_drawing_order(list["drawing_order"]); }

    return sr;

  }

  // TO: ACMAP
  template <>
  AcMap as(SEXP sxp){

    check_class(sxp, "acmap");
    List list = as<List>(sxp);
    List antigens = list["antigens"];
    List sera = list["sera"];
    AcMap acmap(antigens.size(), sera.size());

    // Antigens
    for(arma::uword i=0; i<acmap.antigens.size(); i++){
      acmap.antigens[i] = as<AcAntigen>(wrap(antigens[i]));
    }

    // Sera
    for(arma::uword i=0; i<acmap.sera.size(); i++){
      acmap.sera[i] = as<AcSerum>(wrap(sera[i]));
    }

    // Optimizations
    if(list.containsElementNamed("optimizations")){
      List optimizations = list["optimizations"];
      for(arma::uword i=0; i<optimizations.size(); i++){
        acmap.optimizations.push_back(as<AcOptimization>(wrap(optimizations[i])));
      }
    }

    // Titer table layers
    if(list.containsElementNamed("titer_table_layers")){
      List titer_table_layers = list["titer_table_layers"];
      for(arma::uword i=0; i<titer_table_layers.size(); i++){
        acmap.titer_table_layers.push_back(as<AcTiterTable>(wrap(titer_table_layers[i])));
      }
    }

    // Titer table flat
    if(list.containsElementNamed("titer_table_flat")){
      acmap.titer_table_flat = as<AcTiterTable>(wrap(list["titer_table_flat"]));
    }

    return acmap;

  }

  // TO: ACMAP VECTOR
  template <>
  std::vector<AcMap> as(SEXP sxp){
    List maps = as<List>(sxp);
    int nmaps = maps.size();
    std::vector<AcMap> out;
    for(int i=0; i<nmaps; i++){
      out.push_back(as<AcMap>(wrap(maps[i])));
    }
    return out;
  }

}

#endif
