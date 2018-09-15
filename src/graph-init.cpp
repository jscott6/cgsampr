
#include <Rcpp.h>
using namespace Rcpp;

// run_testthat_tests
RcppExport SEXP _rcpp_module_boot_graph_module();
RcppExport SEXP _rcpp_module_boot_base_module();
RcppExport SEXP run_testthat_tests();

static const R_CallMethodDef CallEntries[] = {
    {"run_testthat_tests", (DL_FUNC) &run_testthat_tests, 0},
    {"_rcpp_module_boot_base_module", (DL_FUNC) &_rcpp_module_boot_base_module, 0},
    {"_rcpp_module_boot_graph_module", (DL_FUNC) &_rcpp_module_boot_graph_module, 0},
    {NULL, NULL, 0}
};

RcppExport void R_init_graphs(DllInfo *dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}
