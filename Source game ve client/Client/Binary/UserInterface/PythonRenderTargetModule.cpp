#include "StdAfx.h"

#ifdef ENABLE_RENDER_TARGET
#include "PythonApplication.h"

PyObject* renderTargetSelectModel(PyObject* poSelf, PyObject* poArgs)
{
	BYTE index = 0;
	if (!PyTuple_GetByte(poArgs, 0, &index))
	{
		return Py_BadArgument();
	}

	int modelIndex = 0;
	if (!PyTuple_GetInteger(poArgs, 1, &modelIndex))
	{
		return Py_BadArgument();
	}

	CRenderTargetManager::Instance().GetRenderTarget(index)->SelectModel(modelIndex);

	return Py_BuildNone();
}
PyObject* renderTargetSetArmor(PyObject* poSelf, PyObject* poArgs)
{
	BYTE index = 0;
	if (!PyTuple_GetByte(poArgs, 0, &index))
	{
		return Py_BadArgument();
	}

	int modelIndex = 0;
	if (!PyTuple_GetInteger(poArgs, 1, &modelIndex))
	{
		return Py_BadArgument();
	}

	CRenderTargetManager::Instance().GetRenderTarget(index)->SetArmor(modelIndex);

	return Py_BuildNone();
}

PyObject* renderTargetSetHair(PyObject* poSelf, PyObject* poArgs)
{
	BYTE index = 0;
	if (!PyTuple_GetByte(poArgs, 0, &index))
	{
		return Py_BadArgument();
	}

	int modelIndex = 0;
	if (!PyTuple_GetInteger(poArgs, 1, &modelIndex))
	{
		return Py_BadArgument();
	}

	CRenderTargetManager::Instance().GetRenderTarget(index)->SetHair(modelIndex);

	return Py_BuildNone();
}

PyObject* renderTargetSetWeapon(PyObject* poSelf, PyObject* poArgs)
{
	BYTE index = 0;
	if (!PyTuple_GetByte(poArgs, 0, &index))
	{
		return Py_BadArgument();
	}

	int modelIndex = 0;
	if (!PyTuple_GetInteger(poArgs, 1, &modelIndex))
	{
		return Py_BadArgument();
	}

	CRenderTargetManager::Instance().GetRenderTarget(index)->SetWeapon(modelIndex);

	return Py_BuildNone();
}

//
PyObject* renderTargetSetUseSkillWithEffect(PyObject* poSelf, PyObject* poArgs)
{
	BYTE index;
	if (!PyTuple_GetByte(poArgs, 0, &index))
		return Py_BadArgument();

	int skillID;
	if (!PyTuple_GetInteger(poArgs, 1, &skillID))
		return Py_BadArgument();

	int effectID;
	if (!PyTuple_GetInteger(poArgs, 2, &effectID))
		return Py_BadArgument();

	CRenderTargetManager::Instance().GetRenderTarget(index)->UseSkillWithEffect(skillID, effectID);

	return Py_BuildNone();
}
//

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
PyObject* renderTargetSetAcce(PyObject* poSelf, PyObject* poArgs)
{
	BYTE index = 0;
	if (!PyTuple_GetByte(poArgs, 0, &index))
	{
		return Py_BadArgument();
	}

	int modelIndex = 0;
	if (!PyTuple_GetInteger(poArgs, 1, &modelIndex))
	{
		return Py_BadArgument();
	}

	CRenderTargetManager::Instance().GetRenderTarget(index)->SetAcce(modelIndex);

	return Py_BuildNone();
}
#endif

#ifdef ENABLE_AURA_COSTUME_SYSTEM
PyObject* renderTargetSetAura(PyObject* poSelf, PyObject* poArgs)
{
	BYTE index = 0;
	if (!PyTuple_GetByte(poArgs, 0, &index))
	{
		return Py_BadArgument();
	}

	int modelIndex = 0;
	if (!PyTuple_GetInteger(poArgs, 1, &modelIndex))
	{
		return Py_BadArgument();
	}

	CRenderTargetManager::Instance().GetRenderTarget(index)->SetAura(modelIndex);

	return Py_BuildNone();
}
#endif

#ifdef ENABLE_SHINING_ITEM_SYSTEM
PyObject* renderTargetSetShining(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BadArgument();

	int iItemVnum;
	if (!PyTuple_GetInteger(poArgs, 1, &iItemVnum))
		return Py_BadArgument();

	const std::shared_ptr<CRenderTarget> target = CRenderTargetManager::Instance().GetRenderTarget(iIndex);

	if (target)
	{
		CItemData* pItemData;
		if (!CItemManager::Instance().GetItemDataPointer(iItemVnum, &pItemData))
			return Py_BuildNone();

		BYTE num;
		DWORD itemSubType = pItemData->GetSubType();
		if ((CItemData::SHINING_WEAPON == itemSubType))
		{
			num = 0/*CHR_SHINING_NUM - CHR_SHINING_WEAPON_1*/;
		}
		else if ((CItemData::SHINING_ARMOR == itemSubType))
		{
			num = 3/*CHR_SHINING_NUM - CHR_SHINING_ARMOR_1*/;
		}
		else
		{
			num = 5/*CHR_SHINING_NUM - CHR_SHINING_SPECIAL*/;
		}

		if (target->IsReady())
		{
			target->GetModel()->SetShiningRender(num, iItemVnum);
			// target->GetModel()->SetShining(num, iItemVnum);
		}
	}
	return Py_BuildNone();
}
#endif

PyObject* renderTargetSetVisibility(PyObject* poSelf, PyObject* poArgs)
{
	BYTE index = 0;
	if (!PyTuple_GetByte(poArgs, 0, &index))
	{
		return Py_BadArgument();
	}

	bool isShow = false;
	if (!PyTuple_GetBoolean(poArgs, 1, &isShow))
	{
		return Py_BadArgument();
	}

	CRenderTargetManager::Instance().GetRenderTarget(index)->SetVisibility(isShow);

	return Py_BuildNone();
}

PyObject* renderTargetSetBackground(PyObject* poSelf, PyObject* poArgs)
{
	BYTE index = 0;
	if (!PyTuple_GetByte(poArgs, 0, &index))
	{
		return Py_BadArgument();
	}

	char * szPathName;
	if (!PyTuple_GetString(poArgs, 1, &szPathName))
	{
		return Py_BadArgument();
	}

	CRenderTargetManager::Instance().GetRenderTarget(index)->CreateBackground(
		szPathName, CPythonApplication::Instance().GetWidth(),
		CPythonApplication::Instance().GetHeight());
	return Py_BuildNone();
}

PyObject* renderTargetModelEye(PyObject* poSelf, PyObject* poArgs)
{
	BYTE index = 0;
	if (!PyTuple_GetByte(poArgs, 0, &index))
	{
		return Py_BadArgument();
	}

	float x;
	if (!PyTuple_GetFloat(poArgs, 1, &x))
	{
		return Py_BadArgument();
	}
	float y;
	if (!PyTuple_GetFloat(poArgs, 2, &y))
	{
		return Py_BadArgument();
	}
	float z;
	if (!PyTuple_GetFloat(poArgs, 3, &z))
	{
		return Py_BadArgument();
	}

	CRenderTarget* renderTarget = CRenderTargetManager::Instance().GetRenderTarget(index).get();

	renderTarget->SetEye(x, y, z);
	return Py_BuildNone();
}

PyObject* renderTargetModelTarget(PyObject* poSelf, PyObject* poArgs)
{
	BYTE index = 0;
	if (!PyTuple_GetByte(poArgs, 0, &index))
	{
		return Py_BadArgument();
	}

	float x;
	if (!PyTuple_GetFloat(poArgs, 1, &x))
	{
		return Py_BadArgument();
	}
	float y;
	if (!PyTuple_GetFloat(poArgs, 2, &y))
	{
		return Py_BadArgument();
	}
	float z;
	if (!PyTuple_GetFloat(poArgs, 3, &z))
	{
		return Py_BadArgument();
	}

	CRenderTarget* renderTarget = CRenderTargetManager::Instance().GetRenderTarget(index).get();

	renderTarget->SetTarget(x, y, z);
	return Py_BuildNone();
}

// #ifdef ENABLE_QUEST_RENDER_EVENT
PyObject* renderTargetModelViewReset(PyObject* poSelf, PyObject* poArgs)
{
	BYTE index = 0;
	if (!PyTuple_GetByte(poArgs, 0, &index))
	{
		return Py_BadArgument();
	}

	CRenderTargetManager::Instance().GetRenderTarget(index)->ModelViewReset();
	return Py_BuildNone();
}
// #endif

#ifdef ENABLE_RENDER_TARGET_EX
PyObject* renderTargetSetAutoRotate(PyObject* poSelf, PyObject* poArgs)
{
	BYTE index = 0;
	if (!PyTuple_GetByte(poArgs, 0, &index))
	{
		return Py_BadArgument();
	}

	bool autoRotate = true;
	if (!PyTuple_GetBoolean(poArgs, 1, &autoRotate))
	{
		return Py_BadArgument();
	}

	CRenderTarget* renderTarget = CRenderTargetManager::Instance().GetRenderTarget(index).get();
	if (renderTarget != nullptr)
	{
		renderTarget->SetAutoRotate(autoRotate);
	}
	else
	{
		TraceError("RenderTargetModule::SetAutoRotate::renderTarget is null");
	}

	return Py_BuildNone();
}

PyObject* renderTargetMouseRotation(PyObject* poSelf, PyObject* poArgs)
{
	BYTE index = 0;
	if (!PyTuple_GetByte(poArgs, 0, &index))
	{
		return Py_BadArgument();
	}

	float value = 0;
	if (!PyTuple_GetFloat(poArgs, 1, &value))
	{
		return Py_BadArgument();
	}

	CRenderTarget* renderTarget = CRenderTargetManager::Instance().GetRenderTarget(index).get();
	if (renderTarget != nullptr)
	{
		renderTarget->MouseRotation(value);
	}
	else
	{
		TraceError("RenderTargetModule::SetModelRotation::renderTarget is null");
	}

	return Py_BuildNone();
}

PyObject* renderTargetMouseZoom(PyObject* poSelf, PyObject* poArgs)
{
	BYTE index = 0;
	if (!PyTuple_GetByte(poArgs, 0, &index))
	{
		return Py_BadArgument();
	}

	bool bZoom = false;
	if (!PyTuple_GetBoolean(poArgs, 1, &bZoom))
	{
		return Py_BadArgument();
	}

	CRenderTargetManager::Instance().GetRenderTarget(index)->MouseZoom(bZoom);
	return Py_BuildNone();
}
#endif

PyObject* renderTargetSetMotion(PyObject* poSelf, PyObject* poArgs)
{
	BYTE index = 0;
	if (!PyTuple_GetByte(poArgs, 0, &index))
	{
		return Py_BadArgument();
	}

	WORD wMotion = 0;
	if (!PyTuple_GetInteger(poArgs, 1, &wMotion))
	{
		return Py_BadArgument();
	}

	CRenderTargetManager::Instance().GetRenderTarget(index)->SetMotion(wMotion);

	return Py_BuildNone();
}

void initRenderTarget()
{
	static PyMethodDef s_methods[] =
	{
		{ "SelectModel", 			renderTargetSelectModel, 	METH_VARARGS },
		{ "SetVisibility", 			renderTargetSetVisibility, 	METH_VARARGS },
		{ "SetBackground", 			renderTargetSetBackground, 	METH_VARARGS },
		{ "SetArmor", 				renderTargetSetArmor, 		METH_VARARGS },
		{ "SetHair", 				renderTargetSetHair, 		METH_VARARGS },
		{ "SetWeapon", 				renderTargetSetWeapon, 		METH_VARARGS },

		{ "SetUseSkillWithEffect", renderTargetSetUseSkillWithEffect, METH_VARARGS },

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		{ "SetAcce", 				renderTargetSetAcce, 		METH_VARARGS },
#endif
#ifdef ENABLE_AURA_COSTUME_SYSTEM
		{ "SetAura", 				renderTargetSetAura, 		METH_VARARGS },
#endif
#ifdef ENABLE_SHINING_ITEM_SYSTEM
		{ "SetShining", 			renderTargetSetShining, 	METH_VARARGS },
#endif
		{ "SetEye", 				renderTargetModelEye, 		METH_VARARGS },
		{ "SetTarget", 				renderTargetModelTarget, 	METH_VARARGS },
// #ifdef ENABLE_QUEST_RENDER_EVENT
		{ "ModelViewReset",			renderTargetModelViewReset,	METH_VARARGS },
// #endif
#ifdef ENABLE_RENDER_TARGET_EX
		{ "SetAutoRotate",			renderTargetSetAutoRotate,	METH_VARARGS },
		{ "MouseRotation",			renderTargetMouseRotation,	METH_VARARGS },
		{ "MouseZoom",				renderTargetMouseZoom,		METH_VARARGS },
#endif
		{ "SetMotion", 				renderTargetSetMotion, 		METH_VARARGS },
		{ NULL, 					NULL, 						NULL 		 },
	};

	PyObject* poModule = Py_InitModule("renderTarget", s_methods);
}
#endif
