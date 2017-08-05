#pragma once

#include <opencv2\core\core.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\ml\ml.hpp>

#include <iostream>
#include <fstream>

using namespace std;
using namespace cv;

void computeMagAngle(InputArray src, OutputArray mag, OutputArray ang);

void computeHOG(InputArray mag, InputArray ang, OutputArray dst, int dims, bool isWeighted);
