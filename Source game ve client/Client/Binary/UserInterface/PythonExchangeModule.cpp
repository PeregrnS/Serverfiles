#include "stdafx.h"
#include "PythonExchange.h"

PyObject * exchangeInitTrading(PyObject * poSelf, PyObject * poArgs)
{
	CPythonExchange::Instance().End();
	return Py_BuildNone();
}

PyObject * exchangeisTrading(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", CPythonExchange::Instance().isTrading());
}

#ifdef ENABLE_GOLD_MAX_EXTENDED
PyObject * exchangeGetElkFromSelf(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("K", CPythonExchange::Instance().GetElkFromSelf());
}

PyObject * exchangeGetElkFromTarget(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("K", CPythonExchange::Instance().GetElkFromTarget());
}
#else
PyObject * exchangeGetElkFromSelf(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", CPythonExchange::Instance().GetElkFromSelf());
}

PyObject * exchangeGetElkFromTarget(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", CPythonExchange::Instance().GetElkFromTarget());
}
#endif

PyObject * exchangeGetAcceptFromSelf(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", CPythonExchange::Instance().GetAcceptFromSelf());
}

PyObject * exchangeGetAcceptFromTarget(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", CPythonExchange::Instance().GetAcceptFromTarget());
}

PyObject * exchangeGetItemVnumFromSelf(PyObject * poSelf, PyObject * poArgs)
{
	int pos;

	if (!PyTuple_GetInteger(poArgs, 0, &pos))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonExchange::Instance().GetItemVnumFromSelf((char) pos));
}

PyObject * exchangeGetItemVnumFromTarget(PyObject * poTarget, PyObject * poArgs)
{
	int pos;

	if (!PyTuple_GetInteger(poArgs, 0, &pos))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonExchange::Instance().GetItemVnumFromTarget((char) pos));
}

PyObject * exchangeGetItemCountFromSelf(PyObject * poSelf, PyObject * poArgs)
{
	int pos;

	if (!PyTuple_GetInteger(poArgs, 0, &pos))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonExchange::Instance().GetItemCountFromSelf((char) pos));
}

PyObject * exchangeGetItemCountFromTarget(PyObject * poTarget, PyObject * poArgs)
{
	int pos;

	if (!PyTuple_GetInteger(poArgs, 0, &pos))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonExchange::Instance().GetItemCountFromTarget((char) pos));
}

PyObject * exchangeGetNameFromSelf(PyObject * poTarget, PyObject * poArgs)
{
	return Py_BuildValue("s", CPythonExchange::Instance().GetNameFromSelf());
}

PyObject * exchangeGetNameFromTarget(PyObject * poTarget, PyObject * poArgs)
{
	return Py_BuildValue("s", CPythonExchange::Instance().GetNameFromTarget());
}

#if defined(ENABLE_LEVEL_IN_TRADE) || defined(ENABLE_EXCHANGE_WINDOW_RENEWAL)
PyObject * exchangeGetLevelFromSelf(PyObject * poTarget, PyObject * poArgs)
{
	return Py_BuildValue("I", CPythonExchange::Instance().GetLevelFromSelf());
}

PyObject * exchangeGetLevelFromTarget(PyObject * poTarget, PyObject * poArgs)
{
	return Py_BuildValue("I", CPythonExchange::Instance().GetLevelFromTarget());
}
#endif

#ifdef ENABLE_EXCHANGE_WINDOW_RENEWAL
PyObject * exchangeGetRaceFromSelf(PyObject * poTarget, PyObject * poArgs)
{
	return Py_BuildValue("I", CPythonExchange::Instance().GetRaceFromSelf());
}

PyObject * exchangeGetRaceFromTarget(PyObject * poTarget, PyObject * poArgs)
{
	return Py_BuildValue("I", CPythonExchange::Instance().GetRaceFromTarget());
}
#endif

PyObject * exchangeGetItemMetinSocketFromTarget(PyObject * poTarget, PyObject * poArgs)
{
	int pos;
	if (!PyTuple_GetInteger(poArgs, 0, &pos))
		return Py_BuildException();
	int iMetinSocketPos;
	if (!PyTuple_GetInteger(poArgs, 1, &iMetinSocketPos))
		return Py_BuildException();
	return Py_BuildValue("i", CPythonExchange::Instance().GetItemMetinSocketFromTarget(pos, iMetinSocketPos));
}

PyObject * exchangeGetItemMetinSocketFromSelf(PyObject * poTarget, PyObject * poArgs)
{
	int pos;
	if (!PyTuple_GetInteger(poArgs, 0, &pos))
		return Py_BuildException();
	int iMetinSocketPos;
	if (!PyTuple_GetInteger(poArgs, 1, &iMetinSocketPos))
		return Py_BuildException();
	return Py_BuildValue("i", CPythonExchange::Instance().GetItemMetinSocketFromSelf(pos, iMetinSocketPos));
}

PyObject * exchangeGetItemAttributeFromTarget(PyObject * poTarget, PyObject * poArgs)
{
	int pos;
	if (!PyTuple_GetInteger(poArgs, 0, &pos))
		return Py_BuildException();
	int iAttrSlotPos;
	if (!PyTuple_GetInteger(poArgs, 1, &iAttrSlotPos))
		return Py_BuildException();

	BYTE byType;
	short sValue;
	CPythonExchange::Instance().GetItemAttributeFromTarget(pos, iAttrSlotPos, &byType, &sValue);

	return Py_BuildValue("ii", byType, sValue);
}

PyObject * exchangeGetItemAttributeFromSelf(PyObject * poTarget, PyObject * poArgs)
{
	int pos;
	if (!PyTuple_GetInteger(poArgs, 0, &pos))
		return Py_BuildException();
	int iAttrSlotPos;
	if (!PyTuple_GetInteger(poArgs, 1, &iAttrSlotPos))
		return Py_BuildException();

	BYTE byType;
	short sValue;
	CPythonExchange::Instance().GetItemAttributeFromSelf(pos, iAttrSlotPos, &byType, &sValue);

	return Py_BuildValue("ii", byType, sValue);
}

PyObject * exchangeGetElkMode(PyObject * poTarget, PyObject * poArgs)
{
	return Py_BuildValue("b", CPythonExchange::Instance().GetElkMode());
}

PyObject * exchangeSetElkMode(PyObject * poTarget, PyObject * poArgs)
{
	int elk_mode;

	if (!PyTuple_GetInteger(poArgs, 0, &elk_mode))
		return Py_BuildException();

	CPythonExchange::Instance().SetElkMode(elk_mode ? true : false);
	return Py_BuildNone();
}

void initTrade()
{
	static PyMethodDef s_methods[] =
	{
		{"InitTrading",					exchangeInitTrading,				METH_VARARGS},
		{"isTrading",					exchangeisTrading,					METH_VARARGS},

		{"GetElkFromSelf",				exchangeGetElkFromSelf,				METH_VARARGS},
		{"GetElkFromTarget",			exchangeGetElkFromTarget,			METH_VARARGS},

		{"GetItemVnumFromSelf",			exchangeGetItemVnumFromSelf,		METH_VARARGS},
		{"GetItemVnumFromTarget",		exchangeGetItemVnumFromTarget,		METH_VARARGS},

		{"GetItemCountFromSelf",		exchangeGetItemCountFromSelf,		METH_VARARGS},
		{"GetItemCountFromTarget",		exchangeGetItemCountFromTarget,		METH_VARARGS},

		{"GetAcceptFromSelf",			exchangeGetAcceptFromSelf,			METH_VARARGS},
		{"GetAcceptFromTarget",			exchangeGetAcceptFromTarget,		METH_VARARGS},

		{"GetNameFromSelf",				exchangeGetNameFromSelf,			METH_VARARGS},
		{"GetNameFromTarget",			exchangeGetNameFromTarget,			METH_VARARGS},

#if defined(ENABLE_LEVEL_IN_TRADE) || defined(ENABLE_EXCHANGE_WINDOW_RENEWAL)
		{"GetLevelFromSelf",			exchangeGetLevelFromSelf,			METH_VARARGS},
		{"GetLevelFromTarget",			exchangeGetLevelFromTarget,			METH_VARARGS},
#endif

#ifdef ENABLE_EXCHANGE_WINDOW_RENEWAL
		{"GetRaceFromSelf", 			exchangeGetRaceFromSelf, 			METH_VARARGS},
		{"GetRaceFromTarget", 			exchangeGetRaceFromTarget, 			METH_VARARGS},
#endif

		{"GetItemMetinSocketFromTarget",	exchangeGetItemMetinSocketFromTarget,	METH_VARARGS},
		{"GetItemMetinSocketFromSelf",		exchangeGetItemMetinSocketFromSelf,		METH_VARARGS},

		{"GetItemAttributeFromTarget",	exchangeGetItemAttributeFromTarget,	METH_VARARGS},
		{"GetItemAttributeFromSelf",	exchangeGetItemAttributeFromSelf,	METH_VARARGS},

		{"GetElkMode",					exchangeGetElkMode,					METH_VARARGS},
		{"SetElkMode",					exchangeSetElkMode,					METH_VARARGS},

		{NULL, NULL},
	};

	PyObject * poModule = Py_InitModule("exchange", s_methods);
	PyModule_AddIntConstant(poModule, "EXCHANGE_ITEM_MAX_NUM", CPythonExchange::EXCHANGE_ITEM_MAX_NUM);
}
//martysama0134's aad276684955eb3421d3edd3e79cd0dc
