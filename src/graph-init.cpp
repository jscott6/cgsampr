
#include <Rcpp.h>

using namespace Rcpp;

#include <Rcpp.h>

using namespace Rcpp;

// run_testthat_tests
RcppExport SEXP _rcpp_module_boot_graph();
RcppExport SEXP _rcpp_module_boot_WeightedGraph();
RcppExport SEXP run_testthat_tests();

static const R_CallMethodDef CallEntries[] = {
    {"run_testthat_tests", (DL_FUNC) &run_testthat_tests, 0},
    {"_rcpp_module_boot_graph", (DL_FUNC) &_rcpp_module_boot_graph, 0},
    {"_rcpp_module_boot_WeightedGraph", (DL_FUNC) &_rcpp_module_boot_WeightedGraph, 0},
    {NULL, NULL, 0}
};

RcppExport void R_init_graphs(DllInfo *dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}
