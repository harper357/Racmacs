% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/map_optimize.R
\name{checkHemisphering}
\alias{checkHemisphering}
\title{Check for hemisphering or trapped points}
\usage{
checkHemisphering(
  map,
  optimization_number = 1,
  grid_spacing = 0.25,
  stress_lim = 0.1,
  options = list()
)
}
\arguments{
\item{map}{The acmap data object}

\item{optimization_number}{The map optimization number}

\item{stepsize}{Grid spacing in antigenic units of the search grid to use
when searching for hemisphering positions}
}
\value{
Returns a data frame with information on any points that were found
to be hemisphering or trapped.
}
\description{
Check for hemisphering or trapped points
}
\seealso{
Other {map diagnostic functions}: 
\code{\link{bootstrapBlobs}()},
\code{\link{bootstrapMap}()},
\code{\link{dimensionTestMap}()},
\code{\link{mapRelaxed}()}
}
\concept{{map diagnostic functions}}
