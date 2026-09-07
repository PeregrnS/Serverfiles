#include "PythonrootlibManager.h"

#ifdef __USE_CYTHON__
#include "Python27/Python.h"
#ifdef _DEBUG
#pragma comment (lib, "rootlib_d.lib")
#else
#pragma comment (lib, "rootlib.lib")
#endif

struct rootlib_SMethodDef
{
	char* func_name;
	void (*func)();
};

PyMODINIT_FUNC initcolorInfo();
PyMODINIT_FUNC initconsoleModule();
PyMODINIT_FUNC initconstInfo();
PyMODINIT_FUNC initdebugInfo();
PyMODINIT_FUNC initdragon_soul_refine_settings();
PyMODINIT_FUNC initemotion();
PyMODINIT_FUNC initexception();
PyMODINIT_FUNC initgame();
PyMODINIT_FUNC initgrid();
PyMODINIT_FUNC initingamewiki();
PyMODINIT_FUNC initingamewikiconfig();
PyMODINIT_FUNC initingamewikiui();
PyMODINIT_FUNC initinterfaceModule();
PyMODINIT_FUNC initintroCreate();
PyMODINIT_FUNC initintroEmpire();
PyMODINIT_FUNC initintroLoading();
PyMODINIT_FUNC initintroLogin();
PyMODINIT_FUNC initintroLogo();
PyMODINIT_FUNC initintroSelect();
PyMODINIT_FUNC initlocaleInfo();
PyMODINIT_FUNC initmouseModule();
PyMODINIT_FUNC initmusicInfo();
PyMODINIT_FUNC initnetworkModule();
PyMODINIT_FUNC initplayerSettingModule();
PyMODINIT_FUNC initPrototype();
PyMODINIT_FUNC initrendertargetinfo();
PyMODINIT_FUNC initserverCommandParser();
PyMODINIT_FUNC initserverInfo();
PyMODINIT_FUNC initserverinfo_default();
PyMODINIT_FUNC initstringCommander();
PyMODINIT_FUNC initsystem();
PyMODINIT_FUNC inittest_affect();
PyMODINIT_FUNC initui();
PyMODINIT_FUNC inituiacce();
PyMODINIT_FUNC inituiAffectShower();
PyMODINIT_FUNC inituiantimultiplefarm();
PyMODINIT_FUNC inituiAttachMetin();
PyMODINIT_FUNC inituiattr67add();
PyMODINIT_FUNC inituiAutoban();
PyMODINIT_FUNC inituiautoskillreader();
PyMODINIT_FUNC inituibattlepass();
PyMODINIT_FUNC inituibiologmanager();
PyMODINIT_FUNC inituibossgui();
PyMODINIT_FUNC inituiCandidate();
PyMODINIT_FUNC inituiCharacter();
PyMODINIT_FUNC inituicharacterdetails();
PyMODINIT_FUNC inituiChat();
PyMODINIT_FUNC inituicheckbox();
PyMODINIT_FUNC inituichestdropinfo();
PyMODINIT_FUNC inituiCommon();
PyMODINIT_FUNC inituiCube();
PyMODINIT_FUNC inituiDragonSoul();
PyMODINIT_FUNC inituidungeoninfo();
PyMODINIT_FUNC inituiEquipmentDialog();
PyMODINIT_FUNC inituievent();
PyMODINIT_FUNC inituieventcalendar();
PyMODINIT_FUNC inituiEx();
PyMODINIT_FUNC inituiExchange();
PyMODINIT_FUNC inituiGameButton();
PyMODINIT_FUNC inituiGameOption();
PyMODINIT_FUNC inituigameoptionnew();
PyMODINIT_FUNC inituigemshop();
PyMODINIT_FUNC inituiGuild();
PyMODINIT_FUNC inituiHelp();
PyMODINIT_FUNC inituihunting();
PyMODINIT_FUNC inituihuntingmapnames();
PyMODINIT_FUNC inituihwidmanager();
PyMODINIT_FUNC inituiInventory();
PyMODINIT_FUNC inituiitemmanager();
PyMODINIT_FUNC inituiitemshop();
PyMODINIT_FUNC inituimaintenance();
PyMODINIT_FUNC inituiMapNameShower();
PyMODINIT_FUNC inituiMessenger();
PyMODINIT_FUNC inituiMiniMap();
PyMODINIT_FUNC inituimovechannel();
PyMODINIT_FUNC inituimovechannel_default();
PyMODINIT_FUNC inituiOption();
PyMODINIT_FUNC inituiParty();
PyMODINIT_FUNC inituiPhaseCurtain();
PyMODINIT_FUNC inituiPickMoney();
PyMODINIT_FUNC inituipickupfilter();
PyMODINIT_FUNC inituiPlayerGauge();
PyMODINIT_FUNC inituiPointReset();
PyMODINIT_FUNC inituipreview();
PyMODINIT_FUNC inituiprivateshop();
PyMODINIT_FUNC inituiPrivateShopBuilder();
PyMODINIT_FUNC inituiprivateshopsearch();
PyMODINIT_FUNC inituiQuest();
PyMODINIT_FUNC inituirankinfo();
PyMODINIT_FUNC inituiRefine();
PyMODINIT_FUNC inituiremoteshop();
PyMODINIT_FUNC inituirendertargetwindow();
PyMODINIT_FUNC inituiRestart();
PyMODINIT_FUNC inituireward();
PyMODINIT_FUNC inituiSafebox();
PyMODINIT_FUNC inituiScriptLocale();
PyMODINIT_FUNC inituiselectitem();
PyMODINIT_FUNC inituiselectitemex();
PyMODINIT_FUNC inituiSelectMusic();
PyMODINIT_FUNC inituiShop();
PyMODINIT_FUNC inituiskillcolor();
PyMODINIT_FUNC inituiskillgroup();
PyMODINIT_FUNC inituiskybox();
PyMODINIT_FUNC inituispinwheel();
PyMODINIT_FUNC inituiswitchbot();
PyMODINIT_FUNC inituiSystem();
PyMODINIT_FUNC inituiSystemOption();
PyMODINIT_FUNC inituiTarget();
PyMODINIT_FUNC inituiTaskBar();
PyMODINIT_FUNC inituiTip();
PyMODINIT_FUNC inituiToolTip();
PyMODINIT_FUNC inituiUploadMark();
PyMODINIT_FUNC inituivote4buff();
PyMODINIT_FUNC inituiwarpshower();
PyMODINIT_FUNC inituiWeb();
PyMODINIT_FUNC inituiWhisper();
PyMODINIT_FUNC initutilfile();
PyMODINIT_FUNC initutils();

rootlib_SMethodDef rootlib_init_methods[] =
{
	{ "colorInfo", initcolorInfo },
	{ "consoleModule", initconsoleModule },
	{ "constInfo", initconstInfo },
	{ "debugInfo", initdebugInfo },
	{ "dragon_soul_refine_settings", initdragon_soul_refine_settings },
	{ "emotion", initemotion },
	{ "exception", initexception },
	{ "game", initgame },
	{ "grid", initgrid },
	{ "ingamewiki", initingamewiki },
	{ "ingamewikiconfig", initingamewikiconfig },
	{ "ingamewikiui", initingamewikiui },
	{ "interfaceModule", initinterfaceModule },
	{ "introCreate", initintroCreate },
	{ "introEmpire", initintroEmpire },
	{ "introLoading", initintroLoading },
	{ "introLogin", initintroLogin },
	{ "introLogo", initintroLogo },
	{ "introSelect", initintroSelect },
	{ "localeInfo", initlocaleInfo },
	{ "mouseModule", initmouseModule },
	{ "musicInfo", initmusicInfo },
	{ "networkModule", initnetworkModule },
	{ "playerSettingModule", initplayerSettingModule },
	{ "Prototype", initPrototype },
	{ "rendertargetinfo", initrendertargetinfo },
	{ "serverCommandParser", initserverCommandParser },
	{ "serverInfo", initserverInfo },
	{ "serverinfo_default", initserverinfo_default },
	{ "stringCommander", initstringCommander },
	{ "system", initsystem },
	{ "test_affect", inittest_affect },
	{ "ui", initui },
	{ "uiacce", inituiacce },
	{ "uiAffectShower", inituiAffectShower },
	{ "uiantimultiplefarm", inituiantimultiplefarm },
	{ "uiAttachMetin", inituiAttachMetin },
	{ "uiattr67add", inituiattr67add },
	{ "uiAutoban", inituiAutoban },
	{ "uiautoskillreader", inituiautoskillreader },
	{ "uibattlepass", inituibattlepass },
	{ "uibiologmanager", inituibiologmanager },
	{ "uibossgui", inituibossgui },
	{ "uiCandidate", inituiCandidate },
	{ "uiCharacter", inituiCharacter },
	{ "uicharacterdetails", inituicharacterdetails },
	{ "uiChat", inituiChat },
	{ "uicheckbox", inituicheckbox },
	{ "uichestdropinfo", inituichestdropinfo },
	{ "uiCommon", inituiCommon },
	{ "uiCube", inituiCube },
	{ "uiDragonSoul", inituiDragonSoul },
	{ "uidungeoninfo", inituidungeoninfo },
	{ "uiEquipmentDialog", inituiEquipmentDialog },
	{ "uievent", inituievent },
	{ "uieventcalendar", inituieventcalendar },
	{ "uiEx", inituiEx },
	{ "uiExchange", inituiExchange },
	{ "uiGameButton", inituiGameButton },
	{ "uiGameOption", inituiGameOption },
	{ "uigameoptionnew", inituigameoptionnew },
	{ "uigemshop", inituigemshop },
	{ "uiGuild", inituiGuild },
	{ "uiHelp", inituiHelp },
	{ "uihunting", inituihunting },
	{ "uihuntingmapnames", inituihuntingmapnames },
	{ "uihwidmanager", inituihwidmanager },
	{ "uiInventory", inituiInventory },
	{ "uiitemmanager", inituiitemmanager },
	{ "uiitemshop", inituiitemshop },
	{ "uimaintenance", inituimaintenance },
	{ "uiMapNameShower", inituiMapNameShower },
	{ "uiMessenger", inituiMessenger },
	{ "uiMiniMap", inituiMiniMap },
	{ "uimovechannel", inituimovechannel },
	{ "uimovechannel_default", inituimovechannel_default },
	{ "uiOption", inituiOption },
	{ "uiParty", inituiParty },
	{ "uiPhaseCurtain", inituiPhaseCurtain },
	{ "uiPickMoney", inituiPickMoney },
	{ "uipickupfilter", inituipickupfilter },
	{ "uiPlayerGauge", inituiPlayerGauge },
	{ "uiPointReset", inituiPointReset },
	{ "uipreview", inituipreview },
	{ "uiprivateshop", inituiprivateshop },
	{ "uiPrivateShopBuilder", inituiPrivateShopBuilder },
	{ "uiprivateshopsearch", inituiprivateshopsearch },
	{ "uiQuest", inituiQuest },
	{ "uirankinfo", inituirankinfo },
	{ "uiRefine", inituiRefine },
	{ "uiremoteshop", inituiremoteshop },
	{ "uirendertargetwindow", inituirendertargetwindow },
	{ "uiRestart", inituiRestart },
	{ "uireward", inituireward },
	{ "uiSafebox", inituiSafebox },
	{ "uiScriptLocale", inituiScriptLocale },
	{ "uiselectitem", inituiselectitem },
	{ "uiselectitemex", inituiselectitemex },
	{ "uiSelectMusic", inituiSelectMusic },
	{ "uiShop", inituiShop },
	{ "uiskillcolor", inituiskillcolor },
	{ "uiskillgroup", inituiskillgroup },
	{ "uiskybox", inituiskybox },
	{ "uispinwheel", inituispinwheel },
	{ "uiswitchbot", inituiswitchbot },
	{ "uiSystem", inituiSystem },
	{ "uiSystemOption", inituiSystemOption },
	{ "uiTarget", inituiTarget },
	{ "uiTaskBar", inituiTaskBar },
	{ "uiTip", inituiTip },
	{ "uiToolTip", inituiToolTip },
	{ "uiUploadMark", inituiUploadMark },
	{ "uivote4buff", inituivote4buff },
	{ "uiwarpshower", inituiwarpshower },
	{ "uiWeb", inituiWeb },
	{ "uiWhisper", inituiWhisper },
	{ "utilfile", initutilfile },
	{ "utils", initutils },
	{ NULL, NULL },
};

static PyObject* rootlib_isExist(PyObject* self, PyObject* args)
{
	char* func_name;

	if (!PyArg_ParseTuple(args, "s", &func_name))
		return NULL;

	for (int i = 0; NULL != rootlib_init_methods[i].func_name; i++)
	{
		if (0 == _stricmp(rootlib_init_methods[i].func_name, func_name))
		{
			return Py_BuildValue("i", 1);
		}
	}
	return Py_BuildValue("i", 0);
}

static PyObject* rootlib_moduleImport(PyObject* self, PyObject* args)
{
	char* func_name;

	if (!PyArg_ParseTuple(args, "s", &func_name))
		return NULL;

	for (int i = 0; NULL != rootlib_init_methods[i].func_name; i++)
	{
		if (0 == _stricmp(rootlib_init_methods[i].func_name, func_name))
		{
			rootlib_init_methods[i].func();
			if (PyErr_Occurred())
				return NULL;
			PyObject* m = PyDict_GetItemString(PyImport_GetModuleDict(), rootlib_init_methods[i].func_name);
			if (m == NULL) {
				PyErr_SetString(PyExc_SystemError,
					"dynamic module not initialized properly");
				return NULL;
			}
			Py_INCREF(m);
			return Py_BuildValue("S", m);
		}
	}
	return NULL;
}

static PyObject* rootlib_getList(PyObject* self, PyObject* args)
{
	int iTupleSize = 0;
	while (NULL != rootlib_init_methods[iTupleSize].func_name) { iTupleSize++; }

	PyObject* retTuple = PyTuple_New(iTupleSize);
	for (int i = 0; NULL != rootlib_init_methods[i].func_name; i++)
	{
		PyObject* retSubString = PyString_FromString(rootlib_init_methods[i].func_name);
		PyTuple_SetItem(retTuple, i, retSubString);
	}
	return retTuple;
}

void initrootlibManager()
{
	static struct PyMethodDef methods[] =
	{
		{"isExist", rootlib_isExist, METH_VARARGS},
		{"moduleImport", rootlib_moduleImport, METH_VARARGS},
		{"getList", rootlib_getList, METH_VARARGS},
		{NULL, NULL},
	};

	PyObject* m;
	m = Py_InitModule("rootlib", methods);
}

#endif
