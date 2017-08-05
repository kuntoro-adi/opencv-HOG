#include "HOG.h"

/// function to compute angle and magnitude image
/// thanks to 
/// http://www.learnopencv.com/histogram-of-oriented-gradients/
/// Dalal, Navneet, and Bill Triggs. "Histograms of oriented gradients for human detection." Computer Vision and Pattern Recognition, 2005. CVPR 2005. IEEE Computer Society Conference on. Vol. 1. IEEE, 2005.
void computeMagAngle(InputArray src, OutputArray mag, OutputArray ang)
{
	Mat img = src.getMat();
	img.convertTo(img, CV_32F, 1 / 255.0);

	/// calculate gradients using sobel
	Mat gx, gy;
	Sobel(img, gx, CV_32F, 1, 0, 1);
	Sobel(img, gy, CV_32F, 0, 1, 1);

	/// Calculate gradient magnitude and direction
	Mat magnitude, angle;
	cartToPolar(gx, gy, magnitude, angle, 1);

	mag.assign(magnitude);
	ang.assign(angle);
}


/// function to compute histogram of oriented gradients feature
/// thanks to 
/// http://www.learnopencv.com/histogram-of-oriented-gradients/
/// Dalal, Navneet, and Bill Triggs. "Histograms of oriented gradients for human detection." Computer Vision and Pattern Recognition, 2005. CVPR 2005. IEEE Computer Society Conference on. Vol. 1. IEEE, 2005.
void computeHOG(InputArray mag, InputArray ang, OutputArray dst, int dims, bool isWeighted = true)
{
	/// init input values
	Mat magMat = mag.getMat();
	Mat angMat = ang.getMat();

	/// validate magnitude and angle dimensions
	if (magMat.rows != angMat.rows || magMat.cols != angMat.cols) {
		return;
	}

	/// get row and col dimensions
	int rows = magMat.rows;
	int cols = magMat.cols;

	/// set up the expected feature dimension, and  
	/// compute the histogram bin length (arc degree) 
	int featureDim = dims;
	float circleDegree = 360.0;
	float binLength = circleDegree / (float)featureDim;
	float halfBin = binLength / 2;

	/// set up the output feature vector
	/// upper limit and median for each bin
	Mat featureVec(1, featureDim, CV_32F);
	featureVec = 0.0;
	vector<float> uplimits(featureDim);
	vector<float> medbins(featureDim);

	for (int i = 0; i < featureDim; i++) {
		uplimits[i] = (2 * i + 1) * halfBin;
		medbins[i] = i * binLength;

		//cout << "(" << medbins[i] << ") ";
		//cout << uplimits[i] << " ";
	}
	//cout << endl;

	/// begin calculate the feature vector
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			/// get the value of angle and magnitude for 
			/// the current index (i,j)
			float angleVal = angMat.at<float>(i, j);
			float magnitudeVal = magMat.at<float>(i, j);

			/// (this is used to calculate weights)
			float dif = 0.0; /// dfference between the angle and the bin value 
			float prop = 0.0; /// proportion for the value of the current bin 

							  /// value to add for the histogram bin of interest
			float valueToAdd = 0.0;
			/// value to add for the neighbour of histogram bin of interest
			float sideValueToAdd = 0.0;
			/// index for the bin of interest and the neighbour
			int valueIdx = 0;
			int sideIdx = 0;

			/// the first bin (zeroth index) is a little bit tricky 
			/// because its value ranges between below 360 degree and higher 0 degree
			/// we need something more intelligent approach than this
			if (angleVal <= uplimits[0] || angleVal >= uplimits[featureDim - 1]) {

				if (!isWeighted) {
					featureVec.at<float>(0, 0) += magnitudeVal;
				}
				else {
					if (angleVal >= medbins[0] && angleVal <= uplimits[0]) {
						dif = abs(angleVal - medbins[0]);

						valueIdx = 0;
						sideIdx = 1;
					}
					else {
						dif = abs(angleVal - circleDegree);

						valueIdx = 0;
						sideIdx = featureDim - 1;
					}
				}

			}
			/// this is for the second until the last bin
			else {
				for (int k = 0; k < featureDim - 1; k++)
				{
					if (angleVal >= uplimits[k] && angleVal < uplimits[k + 1]) {
						if (!isWeighted) {
							featureVec.at<float>(0, k + 1) += magnitudeVal;
						}
						else {
							dif = abs(angleVal - medbins[k + 1]);
							valueIdx = k + 1;

							if (angleVal >= medbins[k + 1]) {
								sideIdx = (k + 1 == featureDim - 1) ? 0 : k + 2;
							}
							else {
								sideIdx = k;
							}
						}

						break;
					}
				}
			}

			/// add the value proportionally depends of 
			/// how close the angle to the median limits
			if (isWeighted) {
				prop = (binLength - dif) / binLength;
				valueToAdd = prop * magnitudeVal;
				sideValueToAdd = (1.00 - prop) * magnitudeVal;
				featureVec.at<float>(0, valueIdx) += valueToAdd;
				featureVec.at<float>(0, sideIdx) += sideValueToAdd;
			}

			//cout << endl;
			//cout << "-angleVal " << angleVal << " -valueIdx " << valueIdx << " -sideIdx " << sideIdx << endl;
			//cout << "-binLength " << binLength << " -dif " << dif << " -prop " << prop << endl;
			//cout << "binLength - dif " << binLength - dif << " (binLength - dif) / binLength " << (binLength - dif) / binLength << endl;
			//cout << "-> " << featureVec << endl;
		}
	}

	dst.assign(featureVec);
}