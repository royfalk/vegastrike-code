/***************************************************************************
 *                           central.h  -  description
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

// Check to see if we're already loaded
#ifndef CENTRAL_H
#define CENTRAL_H

// Which interface to use. Only have one of these uncommented
// If you're using 'build' to compile, you will need to move any files not being linked to dont_link
// If you're using make or VC++, your results may vary
//#define GTK
//#define CONSOLE

#define VERSION "1.1"

#ifdef GTK
#include <gtk/gtk.h>
#endif

#include "display.h"

#include <stdio.h>
#include <stdlib.h>
#include "general.h"
#include "file.h"

#ifndef MAX_READ
#define MAX_READ 1024	// Maximum number of characters to read from a line
#endif

#define CONFIG_FILE "setup.config"

void Start(int*,char***);
void SetGroup(const char *group, const char *setting);
void SetInfo(const char *catagory, const char *info);
const char *GetInfo(const char *catagory);
const char *GetSetting(const char *group);
struct catagory *GetCatStruct(const char *name);
struct group *GetGroupStruct(const char *name);
typedef struct _GtkWidget GtkWidget;
struct catagory {
	const char *group;
	const char *name;
	const char *info;
	GtkWidget *button;
	struct catagory *next;
};

struct group {
	const char *name;
	const char *setting;
	struct group *next;
};

struct global_settings {
	const char *program_name;
	const char *config_file;
	const char *temp_file;
    const char *data_path;
	int columns;
};

// The structs are used primarily for the interface
extern struct catagory CATS;
extern struct group GROUPS;
extern struct global_settings CONFIG;

#endif    //CENTRAL_H
