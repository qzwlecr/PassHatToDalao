#ifndef RECOLIC_PUT_HAT
#define RECOLIC_PUT_HAT

#include "stdafx.hpp"

#include <opencv2/core.hpp>
#include <random>
#include <iostream>
#include <ctime>
#include <cmath>
#include <vector>
#include <string>
using namespace std;
using namespace cimg_library;
using cv::Point2f;
extern pair<vector<string>, vector<string>> hatImageInfos;
std::vector<std::string> DivideString(const std::string &tod, const char divider);
//extern CImg<unsigned int> recolic_read_png(const string &where);

void putHatOn()
{
    //Judge which hat to select
    float k = (analyseResult.bottomLine.lineBegin.y - analyseResult.bottomLine.lineEnd.y) / (analyseResult.bottomLine.lineBegin.x - analyseResult.bottomLine.lineEnd.x);
    if(std::isnan(k))
    {
        cout << "It seems that it's difficult to put a hat on, query has been refused. " << endl;
        throw overflow_error("WARNING: at putHatOn(), line 19, float is nan or -nan. Refused to continue.");
    }
    default_random_engine engine(static_cast<unsigned int>(time(nullptr)));
    const string *pHatInfo;
    if(k > 0)
    { //Right hat
        uniform_int_distribution<int> getRand(0, hatImageInfos.second.size() - 1);
        pHatInfo = &(hatImageInfos.second)[getRand(engine)];
    }
    else
    { //Left hat
        uniform_int_distribution<int> getRand(0, hatImageInfos.first.size() - 1);
        pHatInfo = &(hatImageInfos.first)[getRand(engine)];
    }
    const string &hatInfo = *pHatInfo;
    //Read config
    auto resBuf = DivideString(hatInfo, '|');
    if(resBuf.size() != 5)
    {
        cout << "Invalid config file:" << hatInfo << endl;
        throw invalid_argument("Invalid config file.");
    }
    Point2f srcBeginPoint(stof(resBuf[0]), stof(resBuf[1]));
    Point2f srcEndPoint(stof(resBuf[2]), stof(resBuf[3]));
    CImg<float> srcHat(resBuf[4].c_str());
    cout << "RECOLIC_DEBUG > ";
    for(size_t cter = 0; cter < 5; ++cter)
        cout << resBuf[cter] << ' ';
    cout << endl;
    //methods...
    auto getLength = [](const Point2f &pa, const Point2f &pb) -> float {return sqrt((pb.y-pa.y)*(pb.y-pa.y) + (pb.x-pa.x)*(pb.x-pa.x));};
    auto getk = [](const Point2f &pa, const Point2f &pb) -> float {return (pa.y - pb.y)/(pa.x - pb.x);};
    auto getMiddlePoint = [](const Point2f &pa, const Point2f &pb) -> Point2f {return Point2f((pa.x+pb.x)/2, (pa.y+pb.y)/2);};
    auto printPoint = [](const Point2f &toPrint) {cout << '(' << toPrint.x << ',' << toPrint.y << ')'; };
    float rateToMulti = getLength(analyseResult.bottomLine.lineBegin, analyseResult.bottomLine.lineEnd) / getLength(srcBeginPoint, srcEndPoint);
    cout << "RECOLIC_DEBUG > rate=" << rateToMulti << "srcHat.width=" << srcHat.width() << ",height=" << srcHat.height() << endl;
    srcHat.resize(srcHat.width()*rateToMulti, srcHat.height()*rateToMulti);
    srcBeginPoint.x *= rateToMulti;srcBeginPoint.y *= rateToMulti;srcEndPoint.x *= rateToMulti;srcEndPoint.y *= rateToMulti;
    float ksrc = getk(srcBeginPoint, srcEndPoint);
    cout << "RECOLIC_DEBUG > ksrc=" << ksrc << " k=" << k << endl;
    float angleSrc = atan(ksrc);
    float angleDst = atan(k);
    float originHeight = srcHat.height();
    float offsetAngle = (angleDst - angleSrc);
    cout << "RECOLIC_DEBUG > offsetAngle=" << offsetAngle << endl;
    srcHat.display();
    srcHat.rotate(offsetAngle*57.29578);
    if(srcBeginPoint.x > srcEndPoint.x)
        swap(srcBeginPoint, srcEndPoint);
    float sinAngle = sin(offsetAngle), cosAngle = cos(offsetAngle);
    Point2f newSrcBeginPoint(originHeight*sinAngle + srcBeginPoint.x*cosAngle - srcBeginPoint.y*sinAngle, srcBeginPoint.x*sinAngle + srcBeginPoint.y*cosAngle);
    cout << "RECOLIC_DEBUG > new point:";
    printPoint(newSrcBeginPoint);
    cout << "old src is ";
    printPoint(srcBeginPoint); cout << endl;
    Point2f offsetPointArrow(analyseResult.bottomLine.lineBegin.x - newSrcBeginPoint.x, analyseResult.bottomLine.lineBegin.y - newSrcBeginPoint.y);
    srcHat.display();
    cout << "RECOLIC_DEBUG > Drawing at " << offsetPointArrow.x << ',' << offsetPointArrow.y << endl;
    originImage.draw_image(offsetPointArrow.x, offsetPointArrow.y, srcHat);
    return;
}


std::vector<std::string> DivideString(const std::string &tod, const char divider)
{
	size_t lastPos = 0;
	size_t thisPos = tod.find(divider);
	std::vector<std::string> sbuf;
	if (thisPos != std::string::npos)
	{
		sbuf.push_back(tod.substr(0, thisPos));
		goto gt_1;
	}
	else
        return sbuf;
	do {
		sbuf.push_back(tod.substr(lastPos + 1, thisPos - lastPos - 1));
	gt_1:
		lastPos = thisPos;
		thisPos = tod.find(divider, lastPos + 1);
	} while (thisPos != std::string::npos);
	sbuf.push_back(tod.substr(lastPos + 1));
	return sbuf;
}

#endif
