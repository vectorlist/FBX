#include <fbxio.h>

void FBXIO::getAttributeString(FbxNodeAttribute *attrib)
{
	if (attrib == nullptr) return;

	FbxString typeName;
	switch (attrib->GetAttributeType())
	{
	case FbxNodeAttribute::eUnknown: typeName = "unidentified"; break;
	case FbxNodeAttribute::eNull: typeName = "null"; break;
	case FbxNodeAttribute::eMarker: typeName = "marker"; break;
	case FbxNodeAttribute::eSkeleton: typeName = "skeleton"; break;
	case FbxNodeAttribute::eMesh: typeName = "mesh"; break;
	case FbxNodeAttribute::eNurbs: typeName = "nurbs"; break;
	case FbxNodeAttribute::ePatch: typeName = "patch"; break;
	case FbxNodeAttribute::eCamera: typeName = "camera"; break;
	case FbxNodeAttribute::eCameraStereo: typeName = "stereo"; break;
	case FbxNodeAttribute::eCameraSwitcher: typeName = "camera switcher"; break;
	case FbxNodeAttribute::eLight: typeName = "light"; break;
	case FbxNodeAttribute::eOpticalReference: typeName = "optical reference"; break;
	case FbxNodeAttribute::eOpticalMarker: typeName = "marker"; break;
	case FbxNodeAttribute::eNurbsCurve: typeName = "nurbs curve"; break;
	case FbxNodeAttribute::eTrimNurbsSurface: typeName = "trim nurbs surface"; break;
	case FbxNodeAttribute::eBoundary: typeName = "boundary"; break;
	case FbxNodeAttribute::eNurbsSurface: typeName = "nurbs surface"; break;
	case FbxNodeAttribute::eShape: typeName = "shape"; break;
	case FbxNodeAttribute::eLODGroup: typeName = "lodgroup"; break;
	case FbxNodeAttribute::eSubDiv: typeName = "subdiv"; break;
	default: typeName = "unknown"; break;
	}

	FbxString attrName = attrib->GetName();
	LOG << "Type : " << typeName << ENDL;
	LOG << "Atrribute : " << attrName << ENDL;
}