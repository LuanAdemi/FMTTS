#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include <string>

using namespace cv;
using namespace std;

vector<string> trackerTypes = {"BOOSTING", "MIL", "KCF", "TLD", "MEDIANFLOW", "GOTURN", "MOSSE", "CSRT"}; 

// create tracker by name

Mat translateImg(Mat &img, int offsetx, int offsety){
	Mat trans_mat = (Mat_<double>(2,3) << 1, 0, offsetx, 0, 1, offsety);
	warpAffine(img,img,trans_mat,img.size());
	return trans_mat;
}

Ptr<Tracker> createTrackerByName(string trackerType) 
{
	Ptr<Tracker> tracker;
	if (trackerType ==  trackerTypes[0])
		tracker = TrackerBoosting::create();
	else if (trackerType == trackerTypes[1])
		tracker = TrackerMIL::create();
	else if (trackerType == trackerTypes[2])
		tracker = TrackerKCF::create();
	else if (trackerType == trackerTypes[3])
		tracker = TrackerTLD::create();
	else if (trackerType == trackerTypes[4])
		tracker = TrackerMedianFlow::create();
	else if (trackerType == trackerTypes[5])
		tracker = TrackerGOTURN::create();
	else if (trackerType == trackerTypes[6])
		tracker = TrackerMOSSE::create();
	else if (trackerType == trackerTypes[7])
		tracker = TrackerCSRT::create();
	else {
		cout << "Incorrect tracker name" << endl;
		cout << "Available trackers are: " << endl;
		for (vector<string>::iterator it = trackerTypes.begin() ; it != trackerTypes.end(); ++it)
			std::cout << " " << *it << endl;
	}
	return tracker;
}

// Fill the vector with random colors
void getRandomColors(vector<Scalar> &colors, int numColors)
{
	RNG rng(0);
	for(int i=0; i < numColors; i++)
		colors.push_back(Scalar(rng.uniform(0,255), rng.uniform(0, 255), rng.uniform(0, 255))); 
}

int main(int argc, char * argv[]) 
{
	cout << "Default tracking algoritm is CSRT" << endl;
	cout << "Available tracking algorithms are:" << endl;
	for (vector<string>::iterator it = trackerTypes.begin() ; it != trackerTypes.end(); ++it)
		std::cout << " " << *it << endl;
	
	// Set tracker type. Change this to try different trackers.
	string trackerType = "CSRT";

	// set default values for tracking algorithm and video
	string videoPath = "dragon2.mp4";
	
	// Initialize MultiTracker with tracking algo
	vector<Rect> bboxes;

	// create a video capture object to read videos
	cv::VideoCapture cap(videoPath);
	Mat frame;
	Mat frame2;
	Mat frame3;
	
	// quit if unabke to read video file
	if(!cap.isOpened()) 
	{
		cout << "Error opening video file " << videoPath << endl;
		return -1;
	}

	// read first frame
	cap >> frame;
	Point pt =  Point(425, 10);
		Point pt2 =  Point(425, 500);
		line(frame, pt2, pt, Scalar(0,255,0), 1, 8, 0);
		Point pt3 =  Point(0, 250);
		Point pt4 =  Point(850, 250);
		line(frame, pt3, pt4, Scalar(0,255,0), 1, 8, 0);


	// draw bounding boxes over objects
	// selectROI's default behaviour is to draw box starting from the center
	// when fromCenter is set to false, you can draw box starting from top left corner
	bool showCrosshair = true;
	bool fromCenter = false;
	cv::selectROIs("FMTTS", frame, bboxes, showCrosshair, fromCenter);
	
	// quit if there are no objects to track
	if(bboxes.size() < 1)
		return 0;
	
	vector<Scalar> colors;  
	getRandomColors(colors, bboxes.size()); 
	
	// Create multitracker
	Ptr<MultiTracker> multiTracker = cv::MultiTracker::create();

	// initialize multitracker
	for(int i=0; i < bboxes.size(); i++)
		multiTracker->add(createTrackerByName(trackerType), frame, Rect2d(bboxes[i]));  
	
	// process video and track objects
	cout << "\n==========================================================\n";
	cout << "Started tracking, press ESC to quit." << endl;
		while(cap.isOpened()) 
	{
		// get frame from the video
		cap >> frame;
		cap >> frame2;
		cap >> frame3;
		// stop the program if reached end of video
		if (frame.empty()) break;
		if (frame2.empty()) break;
		//update the tracking result with new frame
		multiTracker->update(frame);
		
		
		Point pt =  Point(425, 10);
		Point pt2 =  Point(425, 500);
		line(frame, pt2, pt, Scalar(0,255,0), 1, 8, 0);
		Point pt3 =  Point(0, 250);
		Point pt4 =  Point(850, 250);
		line(frame, pt3, pt4, Scalar(0,255,0), 1, 8, 0);
		// draw tracked objects
		for(unsigned i=0; i<multiTracker->getObjects().size(); i++)
		{
			
				
			Point center_of_system = Point(425,250);
			rectangle(frame, multiTracker->getObjects()[i], colors[i], 2, 1);
			Point center_of_rect = (multiTracker->getObjects()[i].br() + multiTracker->getObjects()[i].tl())*0.5;
			circle(frame,center_of_rect, 1, Scalar(0,255,0), 10);
			line(frame, center_of_rect, center_of_system, Scalar(0,255,0), 1, 8, 0);
			Point textpoint1 = Point(10,(20 * (i+1))+20*i);
			Point textpoint2 = Point(10,(20 * (i+1))+20*(i+1));
			putText(frame, "X=" + to_string(center_of_rect.x - 425),textpoint1,FONT_HERSHEY_COMPLEX_SMALL, 0.8, colors[i], 1, CV_AA);
			putText(frame, "Y=" + to_string(-(center_of_rect.y -250)),textpoint2,FONT_HERSHEY_COMPLEX_SMALL, 0.8, colors[i], 1, CV_AA);
			translateImg(frame2,-(center_of_rect.x-90),-(center_of_rect.y-315));
			translateImg(frame3,-(center_of_rect.x-90),-(center_of_rect.y-315));
			
		}
		
		for(int y=0;y<frame3.rows;y++)
		{
			for(int x=0;x<frame3.cols;x++)
			{
				// get pixel
				Vec3b color = frame3.at<Vec3b>(Point(x,y));

				if(color[0] > 50 && color[1] > 50 && color[2] > 50)
						{
							color[0] = 0;
							color[1] = 255;
							color[2] = 0;
							//cout << "Pixel >200 :" << x << "," << y << endl;
						}
				// set pixel
				frame3.at<Vec3b>(Point(x,y)) = color;
			}
		}
		
		namedWindow( "Stabelisert", WINDOW_NORMAL );// Create a window for display.
		resizeWindow("Stabelisert", 300,300);
		namedWindow( "Computer Vision", WINDOW_NORMAL );// Create a window for display.
		resizeWindow("Computer Vision", 300,300);

		// show frame
		imshow("FMTTS", frame);
		imshow("Stabelisert", frame2);
		imshow("Computer Vision", frame3);
		
		// quit on x button
		if  (waitKey(1) == 27) break;
		
	 }

 
}