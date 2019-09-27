/***************************************************************************
 *                           central.cpp  -  description
 *                           ----------------------------
 *                           begin                : January 18, 2002
 *                           copyright            : (C) 2002 by David Ranger
 *                           email                : sabarok@start.com.au
 **************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   any later version.                                                    *
 *                                                                         *
 **************************************************************************/

#include "central.h"
struct catagory CATS;
struct group GROUPS;
struct global_settings CONFIG;

// Primary initialization function. Sets everything up and takes care of the program
void Start(int * argc, char ***argv) {
	LoadMainConfig();
	InitGraphics(argc,argv);
	LoadConfig();
	ShowMain();
}

void SetGroup(const char *group, const char *setting) {
	struct group *CURRENT;
	CURRENT = &GROUPS;
	do {
        if (CURRENT->name == nullptr) { continue; }
		if (strcmp(group, CURRENT->name) == 0) { CURRENT->setting = NewString(setting); return; }
    } while ((CURRENT = CURRENT->next) != nullptr);
}

void SetInfo(const char *catagory, const char *info) {
	struct catagory *CURRENT;
	CURRENT = &CATS;
	do {
        if (CURRENT->name == nullptr) { continue; }
		if (strcmp(catagory, CURRENT->name) == 0) { CURRENT->info = NewString(info); return; }
    } while ((CURRENT = CURRENT->next) != nullptr);
}

const char *GetInfo(const char *catagory) {
	struct catagory *CURRENT;
	CURRENT = &CATS;
	do {
        if (CURRENT->name == nullptr) { continue; }
		if (strcmp(catagory, CURRENT->name) == 0) {
			if (CURRENT->info) { return CURRENT->info; }
			else { return catagory; }
		}
    } while ((CURRENT = CURRENT->next) != nullptr);
	return catagory;
}

const char *GetSetting(const char *group) {
	struct group *CUR;
	CUR = &GROUPS;
	do {
        if (CUR->name == nullptr) { continue; }
		if (strcmp(CUR->name, group) == 0) { return CUR->setting; }
    } while ((CUR = CUR->next) != nullptr);
	return "\0";
}

struct catagory *GetCatStruct(const char *name) {
	struct catagory *CUR;
	CUR = &CATS;
	do {
        if (CUR->name == nullptr) { continue; }
		if (strcmp(CUR->name, name) == 0) { return CUR; }
    } while ((CUR = CUR->next) != nullptr);
    return nullptr;
}

struct group *GetGroupStruct(const char *name) {
        struct group *CUR;
        CUR = &GROUPS;
        do {
                if (CUR->name == nullptr) { continue; }
                if (strcmp(CUR->name, name) == 0) { return CUR; }
        } while ((CUR = CUR->next) != nullptr);
        return nullptr;
}

