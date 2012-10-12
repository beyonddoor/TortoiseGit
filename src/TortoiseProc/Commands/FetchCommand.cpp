// TortoiseGit - a Windows shell extension for easy version control

// Copyright (C) 2011-2012 Sven Strickroth <email@cs-ware.de>

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
#include "StdAfx.h"
#include "FetchCommand.h"
#include "AppUtils.h"

bool FetchCommand::Execute()
{
	if (!GitAdminDir().HasAdminDir(g_Git.m_CurrentDir) && !GitAdminDir().IsBareRepo(g_Git.m_CurrentDir)) {
		CMessageBox::Show(hwndExplorer, IDS_NOWORKINGCOPY, IDS_APPNAME, MB_ICONERROR);
		return false;
	}

	bool autoClose = false;
	if (parser.HasVal(_T("closeonend")))
		autoClose = !!parser.GetLongVal(_T("closeonend"));

	return CAppUtils::Fetch(parser.GetVal(_T("remote")), true, autoClose);
}
