# This dummy function definition is included with the package to ensure that
# 'tools::package_native_routine_registration_skeleton()' generates the required
# registration info for the 'run_testthat_tests' symbol.
run_testthat_tests <- function() {
  .Call("_graphs_run_testthat_tests", PACKAGE = "graphs")
}
