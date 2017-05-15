#pragma once

#include <log.h>
#include <fbxsdk.h>
#include <ostream>

class FBXIO
{
public:
	static void getAttributeString(FbxNodeAttribute* attrib);
};


std::ostream& operator<<(std::ostream &os, FbxAMatrix &mat)
{
	os <<
		(float)mat[0][0] << " ," << (float)mat[0][1] << " ," << mat[0][2] << " ," << mat[0][3] << "\n";


	return os;
}
