#ifndef AUTOWHITEBALANCE_H
#define AUTOWHITEBALANCE_H

#include <cv.h>

using namespace cv;
using namespace std;

class AutoWhiteBalancer {
	private:
		void printScalar(Scalar scalar) {
			cout << scalar[0] << ":" << scalar[1] << ":" << scalar[2] << endl;
		}
	public:
		int *offsetLeft;
		int *offsetRight;
		int channels;

		AutoWhiteBalancer(int channels) {
			offsetLeft = new int[channels];
			offsetRight = new int[channels];

			for(int channel=0;channel<channels;channel++) {
				offsetLeft[channel] = 0;
				offsetRight[channel] = 255;
			}
		}

		~AutoWhiteBalancer() {
			delete[] offsetRight;
			delete[] offsetLeft;
		}
		
		void applyCalibration(Mat image) {
			Scalar left(offsetLeft[0], offsetLeft[1], offsetLeft[2]);
			Scalar right(offsetRight[0], offsetRight[1], offsetRight[2]);
			Scalar scale;
			divide(Scalar(255,255,255), right-left, scale);
			printScalar(right-left);
			printScalar(scale);
			Scalar shift = left;

			image = (image - shift);
			multiply(image, scale, image);
		}
};

class AutoWhiteBalance {
	private:
		static Mat getHistForChannel(Mat imageWithOneChannel) {
			/// Establish the number of bins
			int histSize = 256;

			/// Set the ranges ( for B,G,R) )
			float range[] = { 0, 256 } ;
			const float* histRange = { range };

			bool uniform = true; bool accumulate = false;

			Mat hist;

			/// Compute the histograms:
			calcHist( &imageWithOneChannel, 1, 0, Mat(), hist, 1, &histSize, &histRange, uniform, accumulate );

			return hist;
		}
	public:
		static AutoWhiteBalancer* createAutoWhiteBalancer(Mat image) {
			AutoWhiteBalancer* awb = new AutoWhiteBalancer(image.channels());
			vector<Mat> bgr_planes;
			split(image, bgr_planes);
			int size = image.rows * image.cols;

			for(int channel=0;channel < image.channels(); channel++) {
				Mat hist = getHistForChannel(bgr_planes[channel]);

				double integralLeft = 0;
				double integralRight = 0;
				for(int i=0; i<hist.rows; ++i) {
					float val = hist.at<float>(i)*100.0/size;
					integralLeft += val;
					if(integralLeft > 1) {
						awb->offsetLeft[channel] = i;
						break;
					}
				}
				for(int i=hist.rows; i >= 0; --i) {
					integralRight += hist.at<float>(i)*100.0/size;
					if(integralRight > 1) {
						awb->offsetRight[channel] = i;
						break;
					}
				}
			}
			return awb;

		}

		Mat getHistogram(Mat src) {
			/// Separate the image in 3 places ( B, G and R )
			vector<Mat> bgr_planes;
			split( src, bgr_planes );

			Mat b_hist, g_hist, r_hist;

			b_hist = getHistForChannel(bgr_planes[0]);
			g_hist = getHistForChannel(bgr_planes[1]);
			r_hist = getHistForChannel(bgr_planes[2]);


			// Draw the histograms for B, G and R
			int histSize = 256;
			int hist_w = 512; int hist_h = 400;
			int bin_w = cvRound( (double) hist_w/histSize );

			Mat histImage( hist_h, hist_w, CV_8UC3, Scalar( 0,0,0) );

			/// Normalize the result to [ 0, histImage.rows ]
			normalize(b_hist, b_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
			normalize(g_hist, g_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
			normalize(r_hist, r_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );

			/// Draw for each channel
			for( int i = 1; i < histSize; i++ )
			{
				line( histImage, Point( bin_w*(i-1), hist_h - cvRound(b_hist.at<float>(i-1)) ) ,
						Point( bin_w*(i), hist_h - cvRound(b_hist.at<float>(i)) ),
						Scalar( 255, 0, 0), 2, 8, 0  );
				line( histImage, Point( bin_w*(i-1), hist_h - cvRound(g_hist.at<float>(i-1)) ) ,
						Point( bin_w*(i), hist_h - cvRound(g_hist.at<float>(i)) ),
						Scalar( 0, 255, 0), 2, 8, 0  );
				line( histImage, Point( bin_w*(i-1), hist_h - cvRound(r_hist.at<float>(i-1)) ) ,
						Point( bin_w*(i), hist_h - cvRound(r_hist.at<float>(i)) ),
						Scalar( 0, 0, 255), 2, 8, 0  );
			}
			return histImage;
		}
};

#endif /* end of include guard: AUTOWHITEBALANCE_H */
