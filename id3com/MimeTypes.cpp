/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2000 Philip Oldaker.  All rights reserved.
/////////////////////////////////////////////////////////////////////////////
//
// This library is free software; you can redistribute it and/or modify it
// under the terms of the GNU Library General Public License as published by
// the Free Software Foundation; either version 2 of the License, or (at your
// option) any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
// License for more details.
//
// You should have received a copy of the GNU Library General Public License
// along with this library; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

// The id3lib authors encourage improvements and optimisations to be sent to
// the id3lib coordinator.  Please see the README file for details on where to
// send such submissions.  See the AUTHORS file for a list of people who have
// contributed to id3lib.  See the ChangeLog file for a list of changes to
// id3lib.  These files are distributed with id3lib at
// http://download.sourceforge.net/id3lib/
//
/////////////////////////////////////////////////////////////////////////////
// MimeTypes.cpp
/////////////////////////////////////////////////////////////////////////////
// Change Log
//
// Date          Developer             Changes
//
// 18 Aug 2000   Philip Oldaker        Added Picture Functionality
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ID3COM.h"
#include "MimeTypes.h"

/////////////////////////////////////////
ID3_PicTypeDef ID3PicTypeImpl[] = 
{
	{ ID3_PICTURE_OTHER,			_T("Other") },
	{ ID3_PICTURE_FILE_ICON,		_T("32x32 pixels 'file icon' (PNG only)") },
	{ ID3_PICTURE_OTHER_FILE_ICON,	_T("Other file icon")  },
	{ ID3_PICTURE_COVER_FRONT,		_T("Cover (front)") },
	{ ID3_PICTURE_COVER_BACK,		_T("Cover (back)")  },
	{ ID3_PICTURE_LEAFLET_PAGE,		_T("Leaflet page")   },
	{ ID3_PICTURE_MEDIA,			_T("Media (e.g. lable side of CD)")   },
	{ ID3_PICTURE_LEAD_ARTIST,		_T("Lead artist/lead performer/soloist")  },
	{ ID3_PICTURE_ARTIST,			_T("Artist/performer")   },
	{ ID3_PICTURE_CONDUCTOR,		_T("Conductor")   },
	{ ID3_PICTURE_BAND,				_T("Band/Orchestra")   },
	{ ID3_PICTURE_COMPOSER,			_T("Composer")   },
	{ ID3_PICTURE_LYRICIST,			_T("Lyricist/text writer")   },
	{ ID3_PICTURE_REC_LOCATION,		_T("Recording Location")   },
	{ ID3_PICTURE_DUR_RECORDING,	_T("During recording")   },
	{ ID3_PICTURE_DUR_PERFORMANCE,	_T("During performance")   },
	{ ID3_PICTURE_MOVIE,			_T("Movie/video screen capture")  }, 
	{ ID3_PICTURE_FISH_NONSENSE,	_T("A bright coloured fish")   },
	{ ID3_PICTURE_ILLUSTRATION,		_T("Illustration")   },
	{ ID3_PICTURE_BAND_LOGO,		_T("Band/artist logotype")  }, 
	{ ID3_PICTURE_PUBLISHER_LOGO,	_T("Publisher/Studio logotype")   },
// end
	{ ID3_PICTURE_LASTPICTUREID,	_T("") }
};

#define TEMP_IMAGE_FILENAME _T("IT001")

void CMimeTypes::Load()
{
	HKEY hKey;
	if (RegOpenKeyEx(HKEY_CLASSES_ROOT,_T("MIME\\Database\\Content Type"),0,KEY_READ | KEY_ENUMERATE_SUB_KEYS,&hKey) != ERROR_SUCCESS)
		return;
	LPCTSTR pszImage = _T("image");
	LPCTSTR pszExt = _T("Extension");
	TCHAR szKey[MAX_PATH];
	TCHAR szValue[_MAX_EXT];
	DWORD dwLen=0;
	DWORD dwType;
	int i=0;
	while (RegEnumKey(hKey,i,szKey,sizeof(szKey)) == ERROR_SUCCESS)
	{
		if (_tcsncmp(szKey,pszImage,lstrlen(pszImage)) == 0)
		{
			HKEY hKeyExt;
			if (RegOpenKeyEx(hKey,szKey,0,KEY_READ | KEY_QUERY_VALUE,&hKeyExt) == ERROR_SUCCESS)
			{
				dwLen = sizeof(szValue);
				if (RegQueryValueEx(hKeyExt,pszExt,0,&dwType,(LPBYTE)szValue,&dwLen) == ERROR_SUCCESS)
				{
					m_mapMIMETypes[szKey] = szValue;
				}
				RegCloseKey(hKeyExt);
			}
		}
		i++;
	}
	RegCloseKey(hKey);
	// add default icon mime type 
	m_mapMIMETypes[_T("image/ico")] = _T(".ico");
}

tstring CMimeTypes::GetExtFromMimeType(LPCTSTR pszMime)
{
	tstring sExt;
	if (m_mapMIMETypes.size() == 0)
		Load();
	mapMIMETypes::iterator it = m_mapMIMETypes.find(pszMime);
	if (it != m_mapMIMETypes.end())
	{
		sExt = (*it).second;
	}
	return sExt;
}

tstring CMimeTypes::GetMimeTypeFromFileName(LPCTSTR pszFileName)
{
	tstring sMime;
	if (pszFileName == NULL || lstrlen(pszFileName) == 0)
	{
		return sMime;
	}
	if (m_mapMIMETypes.size() == 0)
		Load();
	TCHAR szExtension[_MAX_EXT];
	_tsplitpath(pszFileName, NULL, NULL, NULL, szExtension);
	bool bFound=false;
	for (mapMIMETypes::iterator it=m_mapMIMETypes.begin(); it != m_mapMIMETypes.end(); it++)
	{
		if (_tcsicmp((*it).second.c_str(), szExtension) == 0)
		{
			sMime = (*it).first;
			bFound = true;
			break;
		}
	}
	if (!sMime.length() == 0)
	{
		sMime = _T("image/");
		lstrcpy(szExtension, szExtension+1);
		sMime += szExtension;
	}
	return sMime;
}

tstring CMimeTypes::GetTempFileName(LPCTSTR pszExt)
{
	TCHAR szTempPath[MAX_PATH];
	szTempPath[0] = 0;
	if (GetTempPath(sizeof(szTempPath),szTempPath))
	{
		TCHAR szLongPath[MAX_PATH];
		if (GetLongPathName(szTempPath,szLongPath,sizeof(szLongPath)) > 0)
			lstrcpy(szTempPath,szLongPath);
		lstrcat(szTempPath,TEMP_IMAGE_FILENAME);
		if (*pszExt != _T('.'))
			lstrcat(szTempPath,_T("."));
		lstrcat(szTempPath,pszExt);
	}
	return szTempPath;
}
