
library(testthat)
library(Racmacs)
context("Test underconstrained positions")

# Read titer_table with missing data

titertable <- read.titerTable(test_path("../testdata/titer_tables/titer_table5_underconstrained.csv"))
map <- acmap(titer_table = titertable)

test_that("Warn of undercontrained positions", {

  # Check for warning
  map <- expect_warning(
    optimizeMap(
      map = map,
      number_of_dimensions = 2,
      number_of_optimizations = 1,
      minimum_column_basis = "none",
    ),
    paste(
      "The following antigens have do not have enough titrations to position in 2 dimensions.",
      "Coordinates were still optimized but positions will be unreliable\n\n'ANTIGEN 4'"
    )
  )

  # Check points were still positioned
  expect_false(anyNA(agBaseCoords(map, 1)))

})


test_that("Error for underconstrained points with infinite positions", {

  titerTable(map)[4, 2] <- "*"
  map <- expect_warning(
    optimizeMap(
      map = map,
      number_of_dimensions = 2,
      number_of_optimizations = 1,
      minimum_column_basis = "none",
    ),
    paste(
      "The following antigens are too underconstrained to position in 2 dimensions",
      "and coordinates have been set to NaN:\n\n'ANTIGEN 4'"
    )
  )

  expect_equal(agCoords(map)[4,], c(NaN, NaN))

})
