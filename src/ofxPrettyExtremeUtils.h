#pragma once

#include "ofMain.h"
#include "ofRectangle.h"

ofRectangle ofMaintainAndCenter(ofRectangle imageRegion, ofRectangle boundRegion);
int ofCountFiles(string path, string extension);
void ofRectRounded(float x,float y,float w,float h,float r, bool outline = false);

	
////Suppose we want to crossfade between an image A at 50% alpha and image B at 50% alpha. 
////As we crossfade, we still want to see 50% background continuously through the fade. 
////But in the middle we certainly don't want to draw both images at 25%. That won't work.
////So we use this function to determine the alpha at which to draw both the first and second
////image to get the desired result.
////
////All float alpha values are 0 -> 1	
void ofGetCompositeAlphas(float proportionof1vs0, float alphaAgainstBackground, float& level1, float& level2);

void ofDrawRadialSegment(bool fill, int segmentPairs, float innerRad, float outerRad, 
					   float startTheta, float endTheta, float reduction);