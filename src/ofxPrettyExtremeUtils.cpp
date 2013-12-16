#include "ofxPrettyExtremeUtils.h"
#include <assert.h>

static float rectPoints[12];

ofRectangle ofMaintainAndCenter(ofRectangle imageRegion, ofRectangle boundRegion){

	ofRectangle r(0,0,50,50);
	float imgRatio = imageRegion.width/imageRegion.height;
	float boundRatio = boundRegion.width/boundRegion.height;
	if(imgRatio == boundRatio)
		return boundRegion;
	if(imgRatio < boundRatio){ //image is too tall - center horizontally
		float y = boundRegion.y;
		float w = imgRatio * boundRegion.height;
		float x = boundRegion.x + (boundRegion.width-w)/2.0f;
		float h = boundRegion.height;
		return ofRectangle(x,y,w,h);
	}
	//else //image is too wide - center vertically
	
	float x = boundRegion.x;
	float h = boundRegion.width / imgRatio;
	float y = boundRegion.y + (boundRegion.height-h)/2.0f;
	float w = boundRegion.width;
	return ofRectangle(x,y,w,h);
	
}

///Much faster counter than using ofDirectory
int ofCountFiles(string path, string extension){
	string ppath = ofFilePath::getPathForDirectory(path);
	if(ppath.empty()){
		return 0;
	}
	string originalDirectory = ppath;
	Poco::File myDir = Poco::File::File(ofToDataPath(ppath));

	if(myDir.exists()){
		// File::list(vector<File>) is broken on windows as of march 23, 2011...
		// so we need to use File::list(vector<string>) and build a vector<File>
		// in the future the following can be replaced width: cur.list(files);
		vector<string>fileStrings;
		myDir.list(fileStrings);
		int count = 0;
		for(int i=0;i<(int)fileStrings.size();i++){
			if(fileStrings[i].find(extension) != std::string::npos){
				count++;
			}
		}
		return count;
	}else{
		return 0;
	}
}

//----------------------------------------------------------
void startSmoothingLocal();
void startSmoothingLocal(){
	#ifndef TARGET_OPENGLES
		glPushAttrib(GL_COLOR_BUFFER_BIT | GL_ENABLE_BIT);
	#endif

	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_LINE_SMOOTH);

	//why do we need this?
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


//----------------------------------------------------------
void endSmoothingLocal();
void endSmoothingLocal(){
	#ifndef TARGET_OPENGLES
		glPopAttrib();
	#endif
}

void ofRectRounded(float x,float y,float w,float h,float r, bool outline){

	// use smoothness, if requested:
	if (outline) startSmoothingLocal();

	if(r==0)
	{
		if (ofGetRectMode() == OF_RECTMODE_CORNER){
			rectPoints[0] = x;
			rectPoints[1] = y;

			rectPoints[2] = x+w;
			rectPoints[3] = y;

			rectPoints[4] = x+w;
			rectPoints[5] = y+h;

			rectPoints[6] = x;
			rectPoints[7] = y+h;
		}else{
			rectPoints[0] = x-w/2;
			rectPoints[1] = y-h/2;

			rectPoints[2] = x+w/2;
			rectPoints[3] = y-h/2;

			rectPoints[4] = x+w/2;
			rectPoints[5] = y+h/2;

			rectPoints[6] = x-w/2;
			rectPoints[7] = y+h/2;
		}

		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_FLOAT, 0, &rectPoints[0]);
		glDrawArrays((!outline) ? GL_TRIANGLE_FAN : GL_LINE_LOOP, 0, 4);
		
	} else {
		//use a corner radius!
		//still need these, but in a different order
			if (ofGetRectMode() == OF_RECTMODE_CORNER){
				rectPoints[2] = x;
				rectPoints[3] = y;

				rectPoints[0] = x+w;
				rectPoints[1] = y;

				rectPoints[6] = x+w;
				rectPoints[7] = y+h;

				rectPoints[4] = x;
				rectPoints[5] = y+h;
			}else{
				rectPoints[2] = x-w/2;
				rectPoints[3] = y-h/2;

				rectPoints[0] = x+w/2;
				rectPoints[1] = y-h/2;

				rectPoints[6] = x+w/2;
				rectPoints[7] = y+h/2;

				rectPoints[4] = x-w/2;
				rectPoints[5] = y+h/2;
			}
		
			//lock r to half of width or height
			float mindim = min(w,h);
			float useRadius = min(mindim/2.0f,r);

			//max curve points: 32


			int curvepoints = ofMap(useRadius,3,150,4,32);

			curvepoints = min(32,curvepoints);
			//but at least 4
			curvepoints = max(4,curvepoints);

			GLfloat verts[4*(32)*2];

			//start at top-right corner, right edge of curve
			int pos = 0;
			float theta = 0;
			float thetaIncrement = HALF_PI/(curvepoints-1);
			int cornerID = 0;
			//top-right
			for(int i=0;i<curvepoints;i++)
			{
				verts[2*pos+0]=rectPoints[cornerID+0]-useRadius+useRadius*cos(theta);
				verts[2*pos+1]=rectPoints[cornerID+1]+useRadius-useRadius*sin(theta);
				pos++;
				theta+=thetaIncrement;
			}
			//top-left
			theta = HALF_PI;
			cornerID+=2;
			for(int i=0;i<curvepoints;i++)
			{
				verts[2*pos+0]=rectPoints[cornerID+0]+useRadius+useRadius*cos(theta);
				verts[2*pos+1]=rectPoints[cornerID+1]+useRadius-useRadius*sin(theta);
				pos++;
				theta+=thetaIncrement;
			}
			//bottom-left
			theta = PI;
			cornerID+=2;
			for(int i=0;i<curvepoints;i++)
			{
				verts[2*pos+0]=rectPoints[cornerID+0]+useRadius+useRadius*cos(theta);
				verts[2*pos+1]=rectPoints[cornerID+1]-useRadius-useRadius*sin(theta);
				pos++;
				theta+=thetaIncrement;
			}
			//bottom-right
			theta = PI+HALF_PI;
			cornerID+=2;
			for(int i=0;i<curvepoints;i++)
			{
				verts[2*pos+0]=rectPoints[cornerID+0]-useRadius+useRadius*cos(theta);
				verts[2*pos+1]=rectPoints[cornerID+1]-useRadius-useRadius*sin(theta);
				pos++;
				theta+=thetaIncrement;
			}
			
			glEnableClientState( GL_VERTEX_ARRAY );
			glVertexPointer(2, GL_FLOAT, 0, verts );
				//glDrawArrays( GL_TRIANGLE_FAN, 0, 4*curvepoints );
			glDrawArrays((!outline) ? GL_TRIANGLE_FAN : GL_LINE_LOOP, 0, 4*curvepoints);
			//glDisableClientState( GL_TEXTURE_COORD_ARRAY );

	}

	// use smoothness, if requested:
	if (outline) endSmoothingLocal();
}

void ofGetCompositeAlphas(float proportionof1vs0, float alphaAgainstBackground, float& level1, float& level2){

	/*
	Suppose we want to crossfade between an image A at 50% alpha and image B at 50% alpha. 
	As we crossfade, we still want to see 50% background continuously through the fade. 
	But in the middle we certainly don't want to draw both images at 25%. That won't work.
	So we use this function to determine the alpha at which to draw both the first and second
	image to get the desired result.

	All float alpha values are 0 -> 1
	*/
	
	if(alphaAgainstBackground==1.0f){
		if(proportionof1vs0 == 0.0f){
			level1 = 1.0f;
			level2 = 0.0f;
			return;
		}
		if(proportionof1vs0 == 1.0f){
			level1 = 0.0f;
			level2 = 1.0f;
			return;
		}
	}
	
	//float finalBothLayers = alphaAgainstBackground;
	float finalFirstLayer = alphaAgainstBackground * (1.0f - proportionof1vs0);
	level2 = /*finalBothLayers*/alphaAgainstBackground - finalFirstLayer;
	level1 = finalFirstLayer/((1.0f-alphaAgainstBackground)+finalFirstLayer);
	return;	
}

void ofDrawRadialSegment(bool fill, int segmentPairs, float innerRad, float outerRad, 
					   float startTheta, float endTheta, float reduction)
{
	//Note: startTheta is always zero! Rotate in calling function!
	//		   /
	//		  /\
	//		 /  theta
	//		/____|__

	static float radPoints[400];
	int usePairs = min(segmentPairs,98);
	usePairs = max(usePairs,4);
	float usePairsF = (float)usePairs;
	//float endThetaF = (float)endTheta;
	float totalTheta = endTheta-startTheta;
	//float conversion = totalTheta/usePairsF;

	float reducedInnerRad = innerRad + reduction;
	float reducedOuterRad = outerRad - reduction;

	float innerThetaReduction = atan(reduction/reducedInnerRad);
	float outerThetaReduction = atan(reduction/reducedOuterRad);

	float innerThetaStep = (totalTheta - 2.0f*innerThetaReduction)/(usePairsF);
	float outerThetaStep = (totalTheta - 2.0f*outerThetaReduction)/(usePairsF);

	if(innerRad==0){
		if(fill)
			ofFill();
		else
			ofNoFill();
		ofEllipse(0,0,reducedOuterRad*2,reducedOuterRad*2);
		return;
	}
	//glPushMatrix();
	//glRotatef(startTheta*360/TWO_PI,0,0,1);

	if(fill)
	{		
		for(int i=0;i<=usePairs;i+=1)
		{
			radPoints[4*i+0] = reducedOuterRad*sin(outerThetaReduction+startTheta+outerThetaStep*i);
			radPoints[4*i+1] = reducedOuterRad*cos(outerThetaReduction+startTheta+outerThetaStep*i);
			radPoints[4*i+2] = reducedInnerRad*sin(innerThetaReduction+startTheta+innerThetaStep*i);
			radPoints[4*i+3] = reducedInnerRad*cos(innerThetaReduction+startTheta+innerThetaStep*i);
		}

		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_FLOAT, 0, &radPoints[0]);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, usePairs*2+2);
	}
	else
	{
		for(int i=0;i<=usePairs;i+=1)
		{
			radPoints[2*i+0] = reducedOuterRad*sin(outerThetaReduction+startTheta+outerThetaStep*i);
			radPoints[2*i+1] = reducedOuterRad*cos(outerThetaReduction+startTheta+outerThetaStep*i);
			radPoints[2*(usePairs+1)+2*i+0] = reducedInnerRad*sin(innerThetaReduction+startTheta+innerThetaStep*(usePairs-i));
			radPoints[2*(usePairs+1)+2*i+1] = reducedInnerRad*cos(innerThetaReduction+startTheta+innerThetaStep*(usePairs-i));
		}

		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_FLOAT, 0, &radPoints[0]);
		glDrawArrays(GL_LINE_LOOP, 0, usePairs*2+2);
	}
	//glPopMatrix();
		
}