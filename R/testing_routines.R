

library("testthat")

run_cpp_tests <- function() {
  cat(rep("-",40),"\n")
  cat("C++ Test Results\n")
  cat(rep("-",40),"\n")
  cat(rep("-",40), "\n")
  .Call("_graphs_run_testthat_tests", PACKAGE = "graphs")
}

run_R_tests <- function(){
  cat(rep("-",40),"\n")
  cat("R Test Results\n")
  cat(rep("-",40),"\n")
  cat(rep("-",40),"\n")
  cat()
  test_package("graphs")
}


run_tests <- function(){
  run_cpp_tests()
  run_R_tests()
}
