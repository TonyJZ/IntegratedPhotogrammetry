#include "StdAfx.h"
#include "CalibUtility/BestFitLib.h"

#include "F:\OpenRS\IntegratedPhotogrammetry\external\best-fit\BestFit.h"
void line_fitting(int ptNum, double *point, double *line_gradient, double *line_intercept)
{
	//利用bestFit库进行拟合
	BestFitIO input;
	input.numPoints = ptNum;
	input.points = point;

	// Output settings in this simple example left with default values.
	BestFitIO output;

	// Create the best-fitting circle object, and make it do the work.
	int type = BestFitFactory::Line;
	BestFit *b = BestFitFactory::Create(type, std::cout);
	b->Compute(input, output);

	// output contains solved parameters, and optionally the adjusted points.
	*line_gradient = output.outputFields[BestFitIO::LineGradient];
	*line_intercept = output.outputFields[BestFitIO::LineYIntercept];

	// The factory allocated memory needs to be freed.  
	delete b;
}