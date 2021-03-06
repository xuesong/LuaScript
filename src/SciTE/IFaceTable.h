// This file is part of LuaScript.
// 
// Original work Copyright 1998-2004 by Neil Hodgson <neilh@scintilla.org>
// Derived work Copyright (C)2016 Justin Dailey <dail8859@yahoo.com>
// 
// LuaScript is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#pragma once

#include <vector>

enum IFaceType {
	iface_void,
	iface_int,
	iface_length,
	iface_position,
	iface_colour,
	iface_bool,
	iface_keymod,
	iface_string,
	iface_tstring,
	iface_stringresult,
	iface_tstringresult,
	iface_cells, // This and everything else below is not "scriptable"
	iface_textrange,
	iface_findtext,
	iface_formatrange
};	

struct IFaceConstant {
	const char *name;
	int value;
};

struct IFaceFunction {
	const char *name;
	int value;
	IFaceType returnType;
	IFaceType paramType[2];
};

struct IFaceProperty {
	const char *name;
	int getter;
	int setter;
	IFaceType valueType;
	IFaceType paramType;
	
	IFaceFunction GetterFunction() const {
		IFaceFunction result = {"(property getter)",getter,valueType,{paramType,iface_void}};
		return result;
	}
	
	IFaceFunction SetterFunction() const {
		IFaceFunction result = {"(property setter)",setter,iface_void,{valueType, iface_void}};
		if (paramType != iface_void) {
			result.paramType[0] = paramType;
			if (valueType == iface_stringresult)
				result.paramType[1] = iface_string;
			else if (valueType == iface_tstringresult)
				result.paramType[1] = iface_tstring;
			else
				result.paramType[1] = valueType;
		}
		if ((paramType == iface_void) && ((valueType == iface_string) || (valueType == iface_stringresult))) {
			result.paramType[0] = paramType;
			if (valueType == iface_stringresult)
				result.paramType[1] = iface_string;
			else
				result.paramType[1] = valueType;
		}
		else if ((paramType == iface_void) && ((valueType == iface_tstring) || (valueType == iface_tstringresult))) {
			result.paramType[0] = paramType;
			if (valueType == iface_tstringresult)
				result.paramType[1] = iface_tstring;
			else
				result.paramType[1] = valueType;
		}
		return result;
	}
};

inline bool IFaceTypeIsScriptable(IFaceType t, int index) {
	return t < iface_tstringresult || (index == 1 && (t == iface_stringresult || t == iface_tstringresult));
}

inline bool IFaceTypeIsNumeric(IFaceType t) {
	return (t > iface_void && t < iface_bool);
}

inline bool IFaceFunctionIsScriptable(const IFaceFunction &f) {
	return IFaceTypeIsScriptable(f.paramType[0], 0) && IFaceTypeIsScriptable(f.paramType[1], 1);
}

inline bool IFacePropertyIsScriptable(const IFaceProperty &p) {
	return (((p.valueType > iface_void) && (p.valueType <= iface_tstringresult) && (p.valueType != iface_keymod)) &&
		((p.paramType < iface_colour) || (p.paramType == iface_string) || (p.paramType == iface_tstring) || (p.paramType == iface_bool)) && (p.getter || p.setter));
}

class IFaceTableInterface {
public:
	virtual const IFaceConstant *FindConstant(const char *name) const = 0;
	virtual const IFaceFunction *FindFunction(const char *name) const = 0;
	virtual const IFaceFunction *FindFunctionByConstantName(const char *name) const = 0;
	virtual const IFaceFunction *FindFunctionByValue(int value) const = 0;
	virtual const IFaceProperty *FindProperty(const char *name) const = 0;
	virtual int GetConstantName(int value, char *nameOut, unsigned nameBufferLen, const char *hint) const = 0;
	virtual const IFaceFunction *GetFunctionByMessage(int message) const = 0;
	virtual IFaceFunction GetPropertyFuncByMessage(int message) const = 0;
};

class IFaceTable : public IFaceTableInterface {
public:

public:
	IFaceTable(const char *_prefix,
		const std::vector<IFaceFunction> &_functions,
		const std::vector<IFaceConstant> &_constants,
		const std::vector<IFaceProperty> &_properties) :
		prefix(_prefix),
		functions(_functions),
		constants(_constants),
		properties(_properties)
	{}

	const char *prefix;

	const std::vector<IFaceFunction> &functions;
	const std::vector<IFaceConstant> &constants;
	const std::vector<IFaceProperty> &properties;

	// IFaceTableInterface
	const IFaceConstant *FindConstant(const char *name) const;
	const IFaceFunction *FindFunction(const char *name) const;
	const IFaceFunction *FindFunctionByConstantName(const char *name) const;
	const IFaceFunction *FindFunctionByValue(int value) const;
	const IFaceProperty *FindProperty(const char *name) const;
	int GetConstantName(int value, char *nameOut, unsigned nameBufferLen, const char *hint) const;
	const IFaceFunction *GetFunctionByMessage(int message) const;
	IFaceFunction GetPropertyFuncByMessage(int message) const;

	std::vector<std::string> GetAllConstantNames() const;
	std::vector<std::string> GetAllFunctionNames() const;
	std::vector<std::string> GetAllPropertyNames() const;
};
