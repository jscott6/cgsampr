# Generated by using Rcpp::compileAttributes() -> do not edit by hand
# Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

constructGraph <- function(out_degree, in_degree, fixed, weighted) {
    .Call(`_cgsampr_constructGraph`, out_degree, in_degree, fixed, weighted)
}

initFixed <- function(adjacency_matrix, fixed, search) {
    .Call(`_cgsampr_initFixed`, adjacency_matrix, fixed, search)
}

