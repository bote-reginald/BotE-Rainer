// FileReader.h: Schnittstelle f�r die Klasse CFileReader.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILEREADER_H__7DB56B9E_FEB0_4471_B811_E0DCCFE1FEC6__INCLUDED_)
#define AFX_FILEREADER_H__7DB56B9E_FEB0_4471_B811_E0DCCFE1FEC6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "BuildingInfo.h"

//#define CONVERT_ALPHA4_ALPHA5
//#define CONVERT_ALPHA5_ALPHA4
#define READ_ALPHA4
//#define READ_ALPHA5

class CFileReader : public CObject  
{
public:
	CFileReader();
	virtual ~CFileReader();
	
	void ReadDataFromFile(CArray<CBuildingInfo,CBuildingInfo>* m_BuildingInfos);
	void WriteDataToFile(CArray<CBuildingInfo,CBuildingInfo>* BuildingInfos, BOOLEAN language = 0);
};

#endif // !defined(AFX_FILEREADER_H__7DB56B9E_FEB0_4471_B811_E0DCCFE1FEC6__INCLUDED_)
