import sys
# Initialize module mappings
modules = {
    'GetMainCharacterIndex': 'playerm',
    'GetNameByVID': 'chrm',
    'SendShopEndPacket': 'netm',
    'SetGuildMarkPath': 'appm',
    'GetPrivateShopItemPrice': 'shopm',
    'Button': 'uim',
    'mouseController': 'mouseModulem',
    'AppendChat': 'chatm',
    'ClearSlot': 'wndMgrm',
    'GetCurrentMapName': 'backgroundm',
    'path': 'osm',
    'exists': 'osm',
    'path.exists': 'osm',
    'SetAffect': 'chrmgrm',
    'SelectItem': 'itemm',
    'ArrangeTextTail': 'textTailm',
    'RegisterChatTail': 'textTailm',
    'ItemToolTip': 'uiToolTipm',
    'GetMonsterName': 'nonplayerm',
    'GetLevelByVID': 'nonplayerm',
    'GetGradeByVID': 'nonplayerm',
    'SelectAnswer': 'eventm',
    'ENVIRONMENT_NIGHT': 'constInfom',
    'floor': 'mathm',
    'fmod': 'mathm',
    'factorial': 'mathm',
    'GenerateColor': 'grpm',
    'clock': 'timem',
    'clock_getres': 'timem',
    'clock_gettime': 'timem',
}

# Module variable mappings
module_mapping = {}
for module_name in sys.modules.keys():
    module_dir = dir(__import__(module_name))
    for function_name, alias in modules.items():
        if function_name in module_dir:
            module_mapping[alias] = module_name

# Attempt to import each module
try:
    chr = __import__(module_mapping.get('playerm', 'chr'))
    app = __import__(module_mapping.get('appm', 'app'))
    shop = __import__(module_mapping.get('shopm', 'shop'))
    player = __import__(module_mapping.get('playerm', 'playerm2g2'))
    net = __import__(module_mapping.get('netm', 'm2netm2g'))
    ui = __import__(module_mapping.get('uim', 'jebac_mh'))
    mouseModule = __import__(module_mapping.get('mouseModulem', 'mouseModule'))
    chat = __import__(module_mapping.get('chatm', 'chat'))
    wndMgr = __import__(module_mapping.get('wndMgrm', 'wndMgr'))
    background = __import__(module_mapping.get('backgroundm', 'background'))
    os = __import__(module_mapping.get('osm', 'os'))
    chrmgr = __import__(module_mapping.get('chrmgrm', 'chrmgr'))
    item = __import__(module_mapping.get('itemm', 'item'))
    textTail = __import__(module_mapping.get('textTailm', 'textTail'))
    uiToolTip = __import__(module_mapping.get('uiToolTipm', 'uiToolTip'))
    nonplayer = __import__(module_mapping.get('nonplayerm', 'nonplayer'))
    event = __import__(module_mapping.get('eventm', 'event'))
    math = __import__(module_mapping.get('mathm', 'math'))
    grp = __import__(module_mapping.get('grpm', 'grp'))
    time = __import__(module_mapping.get('timem', 'time'))


	

except ImportError as e:
    print("ImportError: " + str(e))
import dbg
SPAM = TRUE
Czciaka = 'Tahoma:13'



xoxx = 0
TargetVid2=0
TargetVid=0
VidyPozwolenie=0
ListaVidow=[]
ListaVidow2=[]
Lista = []
autowtawanie_pozwo = 0

Mobber_ID = 0
Poty = 0
Poty_1_ID = 0
Poty_1_Icon = 0
Poty_2_ID = 0
Poty_2_Icon = 0
Slotlista = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
cfg = []
xox = 0
Dopalacz_ID = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
SlotUlepszania = 0
telestep = 0
teleport_mode = 0
listash=[]
ch = 1
cipa = 0
listatypow = []
Pozwolenie = 0
met = 0
Odleglosc = 999999
papec = 0
listash=[]
listash2=[]
listaWysz = []
SlotUlepszaniaDO = 0
Przyciskoff = '|cFFFF0000|Hitem|h'
Przyciskon =  '|cff00FF00|H|h'
Mobbercombo1 = ('Normal','Mega')
Autoaacombo1 = ('Normal','Rotatie')
listazapisku = ['1','2','3','4','5','6','7','8','9','10','11','12','13','14','15']
x = 0
COLOR_TEXT = {}
oldSendChatPacket = net.SendChatPacket


class Attack(ui.Window):
	state = 'stop'
	instance = None  # Class-level variable to track if an instance is open #####!
	hp_threshold = 40  # Default HP threshold	
	def __init__(self):
		if Attack.instance is not None:
			chat.AppendChat(chat.CHAT_TYPE_NOTICE, "[FARM] NO SPAM - An instance is created!")
			return  # If an instance already exists, do not create a new one

		ui.Window.__init__(self)
		Attack.instance = self  # Set the instance variable to the new instance
		self.BuildWindow()

		self.ProcessTimeStamp = 0  # Initialize ProcessTimeStamp
		try:
			self.Krzycz()
			self.Krzycz1()
			self.WczytajFarmCord()
			self.Loadfff()

		except:
			pass
		self.itemlist = {}
		self.itemlist1 = {}

		self.autoAttackEditLine, self.autoAttackBtn = gui_instance.getAutoAttackComponents()

        # Set parent of AutoAttack components to the Board of Attack class
		#self.autoAttackEditLine[0].SetParent(self.Board)
		#self.autoAttackEditLine[0].SetPosition(70, 150)  # Adjust position inside the Board
		#self.autoAttackEditLine[0].Show()
		self.autoAttackBtn.SetParent(self.Board)
		self.autoAttackBtn.SetPosition(30, 55)  # Adjust position inside the Board
		self.autoAttackBtn.Show()

	def __del__(self):
		#Attack.instance = None  # Reset the singleton instance on deletion
		ui.Window.__del__(self)

	def BuildWindow(self):
		self.Board1 = ui.Bar()
		self.Board1.SetSize(240,350)
		self.Board1.SetCenterPosition()
		self.Board1.AddFlag('movable')
		self.Board1.AddFlag('float')
		self.Board1.SetColor(grp.GenerateColor(0.0, 0.0, 0.0, 0.0))
		self.Board1.Show()

		self.Board = ui.BoardWithTitleBar()
		self.Board.SetParent(self.Board1)
		self.Board.SetSize(240,350)
		self.Board.SetPosition(0, 0)
		self.Board.SetTitleName('Metin2.Dev - Althaia Proj')
		self.Board.SetCloseEvent(self.Close)
		self.Board.Show()


		self.Auto_Attack_img = ui.ExpandedImageBox()
		self.Auto_Attack_img.SetParent(self)
		self.Auto_Attack_img.SetPosition(2, wndMgr.GetScreenHeight()-150)
#		self.Auto_Attack_img.LoadImage('banner.tga')
		self.Auto_Attack_img.Show()
#
#
		self.Ustawieniao = ui.BoardWithTitleBar()
		self.Ustawieniao.SetSize(390, 195)
		self.Ustawieniao.SetCenterPosition()
		self.Ustawieniao.AddFlag('movable')
		self.Ustawieniao.AddFlag('float')
		self.Ustawieniao.SetTitleName('Setari')
		self.Ustawieniao.SetCloseEvent(self.Ustawieniaoclose)
		self.Ustawieniao.Hide()


		self.Pogoda = ui.BoardWithTitleBar()
		self.Pogoda.SetSize(120, 190)
		self.Pogoda.SetCenterPosition()
		self.Pogoda.AddFlag('movable')
		self.Pogoda.AddFlag('float')
		self.Pogoda.SetTitleName('Setari Vreme / Camera')
		self.Pogoda.SetCloseEvent(self.ClosePogoda)
		self.Pogoda.Hide()


		self.BoardInfo = ui.BoardWithTitleBar()
		self.BoardInfo.SetSize(260, 140)
		self.BoardInfo.SetCenterPosition()
		self.BoardInfo.AddFlag('movable')
		self.BoardInfo.AddFlag('float')
		self.BoardInfo.SetTitleName('About')
		self.BoardInfo.SetCloseEvent(self.CloseInfo)
		self.BoardInfo.Hide()


		self.BoardZap = ui.BoardWithTitleBar()
		self.BoardZap.SetSize(670, 137)
		self.BoardZap.SetCenterPosition()
		self.BoardZap.AddFlag('movable')
		self.BoardZap.AddFlag('float')
		self.BoardZap.SetTitleName('Seteaza-ti locatia de teleportare')
		self.BoardZap.SetCloseEvent(self.CloseZap)
		self.BoardZap.Hide()


		self.comp = Component()
		self.Rozdajpele = self.comp.SlotbarText(self.Ustawieniao, 'Mod Mantie', 255, 35, 115, 15)
		self.Rozdajautoataczku = self.comp.SlotbarText(self.Ustawieniao, 'Mod Auto Attack', 255, 90, 115, 15)


		self.Mobbercombo = self.comp.ComboBox(self.Ustawieniao, 'Normal', 255, 55, 115)
		for Mobbercombo in Mobbercombo1:
			self.Mobbercombo.InsertItem(1,str(Mobbercombo))

		self.Autoaacombo = self.comp.ComboBox(self.Ustawieniao, 'Normal', 255,110, 115)
		for Autoaacombo in Autoaacombo1:
			self.Autoaacombo.InsertItem(1,str(Autoaacombo))



		self.Zapcombo = self.comp.ComboBox(self.BoardZap, 'Nr. Salvare', 31, 100, 85)
		for Zapcombo in listazapisku:
			self.Zapcombo.InsertItem(1,str(Zapcombo))

		self.btns3 = ui.Button()
		self.btns3.SetPosition(wndMgr.GetScreenWidth() - 105, 165)
#		self.btns3.SetPosition(3, wndMgr.GetScreenHeight()/2-wndMgr.GetScreenHeight()/10)
		self.btns3.SetEvent(self.OpenWindow)
		self.btns3.SetUpVisual('d:/ymir work/ui/game/windows/tab_button_large_01.sub')
		self.btns3.SetOverVisual('d:/ymir work/ui/game/windows/tab_button_large_02.sub')
		self.btns3.SetDownVisual('d:/ymir work/ui/game/windows/tab_button_large_03.sub')
		self.btns3.SetText('AutoHunt')
		self.btns3.Hide()



		self.Zoom = self.comp.Button(self.Pogoda, 'Zoom', '', 15, 80, self.Zoom_func, 'd:/ymir work/ui/public/large_button_01.sub', 'd:/ymir work/ui/public/large_button_02.sub', 'd:/ymir work/ui/public/large_button_03.sub')
		self.Dzien = self.comp.Button(self.Pogoda, 'Zi', '', 15, 30, self.Dzien_func, 'd:/ymir work/ui/public/large_button_01.sub', 'd:/ymir work/ui/public/large_button_02.sub', 'd:/ymir work/ui/public/large_button_03.sub')
		self.Noc = self.comp.Button(self.Pogoda, 'Noapte', '', 15, 55, self.Noc_func, 'd:/ymir work/ui/public/large_button_01.sub', 'd:/ymir work/ui/public/large_button_02.sub', 'd:/ymir work/ui/public/large_button_03.sub')
		self.Snieg = self.comp.ToggleButton(self.Pogoda, 'Ninsoare', '', 15, 105, (lambda arg = 'off': self.Snieg_func(arg)), (lambda arg = 'on': self.Snieg_func(arg)), 'd:/ymir work/ui/public/large_button_01.sub', 'd:/ymir work/ui/public/large_button_02.sub', 'd:/ymir work/ui/public/large_button_03.sub')
		self.Kamera = self.comp.ToggleButton(self.Pogoda, 'Camera mod', '', 15, 155, (lambda arg = 'off': self.Kamera_func(arg)), (lambda arg = 'on': self.Kamera_func(arg)), 'd:/ymir work/ui/public/large_button_01.sub', 'd:/ymir work/ui/public/large_button_02.sub', 'd:/ymir work/ui/public/large_button_03.sub')

		self.Info = self.comp.Button(self.Board, '', '', 20, 9, self.OpenWindowInfo, 'd:/ymir work/ui/game/taskbar/Open_Chat_Log_Button_01.sub', 'd:/ymir work/ui/game/taskbar/Open_Chat_Log_Button_02.sub', 'd:/ymir work/ui/game/taskbar/Open_Chat_Log_Button_03.sub')
		self.Zamknijgre = self.comp.Button(self.Board, 'Inchide client', '', 80, 200, self.Zamknijgre_func, 'd:/ymir work/ui/public/large_button_01.sub', 'd:/ymir work/ui/public/large_button_02.sub', 'd:/ymir work/ui/public/large_button_03.sub')
		self.AutoAtak = self.comp.ToggleButton(self.Board, 'AFK Attack', '', 30, 30, (lambda arg = 'off': self.AutoAtak_func(arg)), (lambda arg = 'on': self.AutoAtak_func(arg)), 'd:/ymir work/ui/public/large_button_01.sub', 'd:/ymir work/ui/public/large_button_02.sub', 'd:/ymir work/ui/public/large_button_03.sub')
		self.Mobberek = self.comp.ToggleButton(self.Board, 'Mantie', '', 125, 30, (lambda arg = 'off': self.Mobber_func(arg)), (lambda arg = 'on': self.Mobber_func(arg)), 'd:/ymir work/ui/public/large_button_01.sub', 'd:/ymir work/ui/public/large_button_02.sub', 'd:/ymir work/ui/public/large_button_03.sub')
		self.Poty = self.comp.ToggleButton(self.Board, 'Potiuni', '', 125, 55, (lambda arg = 'off': self.Poty_func(arg)), (lambda arg = 'on': self.Poty_func(arg)), 'd:/ymir work/ui/public/large_button_01.sub', 'd:/ymir work/ui/public/large_button_02.sub', 'd:/ymir work/ui/public/large_button_03.sub')
		self.AutoWstawanie = self.comp.ToggleButton(self.Board, 'Reinvie', '', 125, 80, (lambda arg = 'off': self.AutoWstawanie_func(arg)), (lambda arg = 'on': self.AutoWstawanie_func(arg)), 'd:/ymir work/ui/public/large_button_01.sub', 'd:/ymir work/ui/public/large_button_02.sub', 'd:/ymir work/ui/public/large_button_03.sub')

		self.AutoSkilestr = self.comp.Button(self.Board, 'Retarget Skill', '', 30, 80, self.AutoSkileWindow, 'd:/ymir work/ui/public/large_button_01.sub', 'd:/ymir work/ui/public/large_button_02.sub', 'd:/ymir work/ui/public/large_button_03.sub')
		#self.Ustawienia = self.comp.Button(self.Board, 'Setari Auto Attack', '', 25, 55, self.OpenWindowUstawieniao, 'd:/ymir work/ui/public/large_button_01.sub', 'd:/ymir work/ui/public/large_button_02.sub', 'd:/ymir work/ui/public/large_button_03.sub')

		self.Pogodastr = self.comp.Button(self.Board, 'Vreme / Camera', '', 80, 180, self.OpenWindowPogoda, 'd:/ymir work/ui/public/large_button_01.sub', 'd:/ymir work/ui/public/large_button_02.sub', 'd:/ymir work/ui/public/large_button_03.sub')
#
		self.tylpotki = self.comp.ThinBoard(self.Board, FALSE, 185, 210, 50, 130, FALSE)
		self.slotbar_Potyhpczas, self.Potyhpczas = self.comp.EditLine(self.Board, '50', 160, 230, 20, 15, 2)
		self.slotbar_Potympczas, self.Potympczas = self.comp.EditLine(self.Board, '50', 160, 260, 20, 15, 2)
		self.slotbar_Peleczas, self.Peleczas = self.comp.EditLine(self.Board, '30', 160, 290, 20, 15, 3)
		self.odilepotyhp = self.comp.SlotbarText(self.Board, 'Use when HP drops below %', 10, 230, 150, 15)
		self.odilepotymp = self.comp.SlotbarText(self.Board, 'Use when MP drops below %', 10, 260, 150, 15)
		self.coilepele = self.comp.SlotbarText(self.Board, 'Timp folosire mantie', 10, 290, 150, 15)


		self.Aurax1 = self.comp.ExpandedImage(self.Board, 65, 140, 'd:/ymir work/ui/skill/warrior/geomgyeong_03.sub')
		self.Berekx1 = self.comp.ExpandedImage(self.Board, 105, 140, 'd:/ymir work/ui/skill/warrior/jeongwi_03.sub')
		self.Silnex1 = self.comp.ExpandedImage(self.Board, 65, 140, 'd:/ymir work/ui/skill/warrior/cheongeun_03.sub')
		self.Ostrzex1 = self.comp.ExpandedImage(self.Board, 105, 140, 'd:/ymir work/ui/skill/sura/gwigeom_03.sub')
		self.Strachx1 = self.comp.ExpandedImage(self.Board, 65, 140, 'd:/ymir work/ui/skill/sura/gongpo_03.sub')
		self.Zbrojax1 = self.comp.ExpandedImage(self.Board, 145, 140, 'd:/ymir work/ui/skill/sura/jumagap_03.sub')
		self.Ochronkax1 = self.comp.ExpandedImage(self.Board, 65, 140, 'd:/ymir work/ui/skill/sura/heuksin_03.sub')
		self.Duchx1 = self.comp.ExpandedImage(self.Board, 105, 140, 'd:/ymir work/ui/skill/sura/muyeong_03.sub')
		self.Atakx1 = self.comp.ExpandedImage(self.Board, 65, 140, 'd:/ymir work/ui/skill/shaman/jeungryeok_03.sub')
		self.Zwinnoscx1 = self.comp.ExpandedImage(self.Board, 105, 140, 'd:/ymir work/ui/skill/shaman/kwaesok_03.sub')
		self.Blogox1 = self.comp.ExpandedImage(self.Board, 65, 140, 'd:/ymir work/ui/skill/shaman/hosin_03.sub')
		self.Pomocx1 = self.comp.ExpandedImage(self.Board, 105, 140, 'd:/ymir work/ui/skill/shaman/gicheon_03.sub')
		self.Odbiciex1 = self.comp.ExpandedImage(self.Board, 145, 140, 'd:/ymir work/ui/skill/shaman/boho_03.sub')

		self.Aurax = self.comp.ToggleButton(self.Board, '', '', 65, 140, (lambda arg = 'off': self.Skill4Buff_func(arg)), (lambda arg = 'on': self.Skill4Buff_func(arg)), 'd:/ymir work/ui/public/slot_cover_button_01.sub', 'd:/ymir work/ui/public/slot_cover_button_02.sub', 'd:/ymir work/ui/public/slot_cover_button_03.sub')
		self.Berekx = self.comp.ToggleButton(self.Board, '', '', 105, 140, (lambda arg = 'off': self.Skill3Buff_func(arg)), (lambda arg = 'on': self.Skill3Buff_func(arg)), 'd:/ymir work/ui/public/slot_cover_button_01.sub', 'd:/ymir work/ui/public/slot_cover_button_02.sub', 'd:/ymir work/ui/public/slot_cover_button_03.sub')
		self.Silnex = self.comp.ToggleButton(self.Board, '', '', 65, 140, (lambda arg = 'off': self.Skill4Buff_func(arg)), (lambda arg = 'on': self.Skill4Buff_func(arg)), 'd:/ymir work/ui/public/slot_cover_button_01.sub', 'd:/ymir work/ui/public/slot_cover_button_02.sub', 'd:/ymir work/ui/public/slot_cover_button_03.sub')
		self.Ostrzex = self.comp.ToggleButton(self.Board, '', '', 105, 140, (lambda arg = 'off': self.Skill3Buff_func(arg)), (lambda arg = 'on': self.Skill3Buff_func(arg)), 'd:/ymir work/ui/public/slot_cover_button_01.sub', 'd:/ymir work/ui/public/slot_cover_button_02.sub', 'd:/ymir work/ui/public/slot_cover_button_03.sub')
		self.Strachx = self.comp.ToggleButton(self.Board, '', '', 65, 140, (lambda arg = 'off': self.Skill4Buff_func(arg)), (lambda arg = 'on': self.Skill4Buff_func(arg)), 'd:/ymir work/ui/public/slot_cover_button_01.sub', 'd:/ymir work/ui/public/slot_cover_button_02.sub', 'd:/ymir work/ui/public/slot_cover_button_03.sub')
		self.Zbrojax = self.comp.ToggleButton(self.Board, '', '', 145, 140, (lambda arg = 'off': self.Skill5buff_func(arg)), (lambda arg = 'on': self.Skill5buff_func(arg)), 'd:/ymir work/ui/public/slot_cover_button_01.sub', 'd:/ymir work/ui/public/slot_cover_button_02.sub', 'd:/ymir work/ui/public/slot_cover_button_03.sub')
		self.Ochronkax = self.comp.ToggleButton(self.Board, '', '', 65, 140, (lambda arg = 'off': self.Skill4Buff_func(arg)), (lambda arg = 'on': self.Skill4Buff_func(arg)), 'd:/ymir work/ui/public/slot_cover_button_01.sub', 'd:/ymir work/ui/public/slot_cover_button_02.sub', 'd:/ymir work/ui/public/slot_cover_button_03.sub')
		self.Duchx = self.comp.ToggleButton(self.Board, '', '', 105, 140, (lambda arg = 'off': self.Skill3Buff_func(arg)), (lambda arg = 'on': self.Skill3Buff_func(arg)), 'd:/ymir work/ui/public/slot_cover_button_01.sub', 'd:/ymir work/ui/public/slot_cover_button_02.sub', 'd:/ymir work/ui/public/slot_cover_button_03.sub')
		self.Atakx = self.comp.ToggleButton(self.Board, '', '', 65, 140, (lambda arg = 'off': self.Skill6buff_func(arg)), (lambda arg = 'on': self.Skill6buff_func(arg)), 'd:/ymir work/ui/public/slot_cover_button_01.sub', 'd:/ymir work/ui/public/slot_cover_button_02.sub', 'd:/ymir work/ui/public/slot_cover_button_03.sub')
		self.Zwinnoscx = self.comp.ToggleButton(self.Board, '', '', 105, 140, (lambda arg = 'off': self.Skill5buff_func(arg)), (lambda arg = 'on': self.Skill5buff_func(arg)), 'd:/ymir work/ui/public/slot_cover_button_01.sub', 'd:/ymir work/ui/public/slot_cover_button_02.sub', 'd:/ymir work/ui/public/slot_cover_button_03.sub')
		self.Blogox = self.comp.ToggleButton(self.Board, '', '', 65, 140, (lambda arg = 'off': self.Skill4Buff_func(arg)), (lambda arg = 'on': self.Skill4Buff_func(arg)), 'd:/ymir work/ui/public/slot_cover_button_01.sub', 'd:/ymir work/ui/public/slot_cover_button_02.sub', 'd:/ymir work/ui/public/slot_cover_button_03.sub')
		self.Pomocx = self.comp.ToggleButton(self.Board, '', '', 105, 140, (lambda arg = 'off': self.Skill6buff_func1(arg)), (lambda arg = 'on': self.Skill6buff_func1(arg)), 'd:/ymir work/ui/public/slot_cover_button_01.sub', 'd:/ymir work/ui/public/slot_cover_button_02.sub', 'd:/ymir work/ui/public/slot_cover_button_03.sub')
		self.Odbiciex = self.comp.ToggleButton(self.Board, '', '', 145, 140, (lambda arg = 'off': self.Skill5buff_func(arg)), (lambda arg = 'on': self.Skill5buff_func(arg)), 'd:/ymir work/ui/public/slot_cover_button_01.sub', 'd:/ymir work/ui/public/slot_cover_button_02.sub', 'd:/ymir work/ui/public/slot_cover_button_03.sub')
		self.slotbar_Skill1czas, self.Skill1czas = self.comp.EditLine(self.Board, '120', 68, 125, 25, 15, 3)
		self.slotbar_Skill2czas, self.Skill2czas = self.comp.EditLine(self.Board, '83', 108, 125, 25, 15, 3)
		self.slotbar_Skill3czas, self.Skill3czas = self.comp.EditLine(self.Board, '45', 148, 125, 25, 15, 3)

		self.About1 = self.comp.TextLine(self.BoardInfo, 'Althaia Project - English', 20, 30, self.comp.RGB(223, 129, 83),Czciaka)
		self.About2 = self.comp.TextLine(self.BoardInfo, 'Click on "Retarget Skill" to set the skill for', 20, 45, self.comp.RGB(223, 129, 83),Czciaka)
		self.About7 = self.comp.TextLine(self.BoardInfo, 'your character.', 20, 56, self.comp.RGB(223, 129, 83),Czciaka)
		self.About3 = self.comp.TextLine(self.BoardInfo, 'Do not use "AFK Attack" and "Target Hunt"', 20, 67, self.comp.RGB(223, 129, 83),Czciaka)
		self.About6 = self.comp.TextLine(self.BoardInfo, 'at the same time. It is your own risk!', 20, 78, self.comp.RGB(223, 129, 83),Czciaka)
		self.About4 = self.comp.TextLine(self.BoardInfo, 'This system is still in testing.', 20, 89, self.comp.RGB(223, 129, 83),Czciaka)
		self.About5 = self.comp.TextLine(self.BoardInfo, 'System remade, adjusted and (kind of.. optimized)', 8, 106, self.comp.RGB(223, 129, 83),Czciaka)
		self.About8 = self.comp.TextLine(self.BoardInfo, ' by KolenMG', 20, 116, self.comp.RGB(223, 129, 83),Czciaka)

		self.infotext = self.comp.TextLine(self.Board, 'Doar magiile cu durata pot fi utilizate.',33,100,self.comp.RGB(223,129,83),Czciaka)
		self.infotext2 = self.comp.TextLine(self.Board, 'Apasa pe butonul About pentru mai multe info.',10,110,self.comp.RGB(223,129,83),Czciaka)


		self.MobberSlot = ui.ExpandedImageBox()
		self.MobberSlot.SetParent(self.tylpotki)
		self.MobberSlot.LoadImage('d:/ymir work/ui/public/Slot_Base.sub')
		self.MobberSlot.OnMouseLeftButtonUp = lambda: self.MobberSlotFunc()
		self.MobberSlot.OnMouseRightButtonUp = lambda: self.usunpele()
		self.MobberSlot.SetPosition(9,89)
		self.MobberSlot.Show()

		self.MobberSlotIcon = ui.ExpandedImageBox()
		self.MobberSlotIcon.SetParent(self.tylpotki)
		self.MobberSlotIcon.SetPosition(9,89)
		self.MobberSlotIcon.LoadImage('')
		self.MobberSlotIcon.OnMouseLeftButtonUp = lambda: self.MobberSlotFunc()
		self.MobberSlot.OnMouseRightButtonUp = lambda: self.usunpele()
		self.MobberSlotIcon.Show()

		self.PotySlot = ui.ExpandedImageBox()
		self.PotySlot.SetParent(self.tylpotki)
		self.PotySlot.LoadImage('d:/ymir work/ui/public/Slot_Base.sub')
		self.PotySlot.OnMouseLeftButtonUp = lambda: self.Set_Poty_1_ID()
		self.PotySlot.OnMouseRightButtonUp = lambda: self.usunpotehp()
		self.PotySlot.SetPosition(9,5)
		self.PotySlot.Show()

		self.PotySlotIcon = ui.ExpandedImageBox()
		self.PotySlotIcon.SetParent(self.tylpotki)
		self.PotySlotIcon.SetPosition(9,5)
		self.PotySlotIcon.OnMouseLeftButtonUp = lambda: self.Set_Poty_1_ID()
		self.PotySlotIcon.OnMouseRightButtonUp = lambda: self.usunpotehp()
		self.PotySlotIcon.Show()

		self.PotySlot2 = ui.ExpandedImageBox()
		self.PotySlot2.SetParent(self.tylpotki)
		self.PotySlot2.LoadImage('d:/ymir work/ui/public/Slot_Base.sub')
		self.PotySlot2.OnMouseLeftButtonUp = lambda: self.Set_Poty_2_ID()
		self.PotySlot2.OnMouseRightButtonUp = lambda: self.usunpotemp()
		self.PotySlot2.SetPosition(9,47)
		self.PotySlot2.Show()

		self.PotySlotIcon2 = ui.ExpandedImageBox()
		self.PotySlotIcon2.SetParent(self.tylpotki)
		self.PotySlotIcon2.SetPosition(9,47)
		self.PotySlotIcon2.OnMouseLeftButtonUp = lambda: self.Set_Poty_2_ID()
		self.PotySlotIcon2.OnMouseRightButtonUp = lambda: self.usunpotemp()
		self.PotySlotIcon2.Show()


	def DivideToFloat(self,x, y):
		try:
			return x * (y**-1)
		except:
			return 0

	def GetTmpTeleport(self,DestX, DestY):
		global DivideToFloat
		(PlayerX, PlayerY, PlayerZ) = player.GetMainCharacterPosition()
		DifX = DestX - PlayerX
		DifY = DestY - PlayerY
		Vektor = self.DivideToFloat(2000, math.sqrt(DifX**2 + DifY**2))
		TempX = PlayerX + Vektor*DifX
		TempY = PlayerY + Vektor*DifY
		Count = self.DivideToFloat((DestX - PlayerX), (Vektor*DifX))
		return (TempX, TempY, Count)

	def Debug(self):
		player.SetSingleDIKKeyState(app.DIK_UP, TRUE)
		player.SetSingleDIKKeyState(app.DIK_UP, FALSE)

	def TeleportToDest(self,aimx, aimy):
		global Debug, GetTmpTeleport, TeleportState
		(TmpX, TmpY, Count) = self.GetTmpTeleport(aimx, aimy)
		TmpCount = 0
		myVid = player.GetMainCharacterIndex()
		while TmpCount < Count:
			chr.SelectInstance(myVid)
			(TmpX, TmpY, Crap) = self.GetTmpTeleport(aimx, aimy)
			chr.SetPixelPosition(int(TmpX), int(TmpY))
			TmpCount += 1
			self.Debug()

		chr.SetPixelPosition(int(aimx), int(aimy))
		self.Debug()
		TeleportState = 1
		chr.SelectInstance(myVid)

	def Zoom_func(self):
		self.zoom=XX()
		self.zoom.XX1(1)
		self.zoom.XX2(self.Zoom_func)
		app.SetCameraMaxDistance(80000) #100000

	def MobberSlotFunc(self):
		global Mobber_ID
		if mouseModule.mouseController.isAttached():
			attachedSlotVnum = mouseModule.mouseController.GetAttachedItemIndex()
			item.SelectItem(attachedSlotVnum)

			Mobber_ID = mouseModule.mouseController.GetAttachedItemIndex()
			mouseModule.mouseController.DeattachObject()

			item.SelectItem(int(attachedSlotVnum))
			self.MobberSlotIcon.LoadImage(str(item.GetIconImageFileName()))

	def Set_Poty_1_ID(self):
		global Poty_1_ID
		global Poty_1_Icon
		if mouseModule.mouseController.isAttached():
			attachedSlotType = mouseModule.mouseController.GetAttachedType()
			attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
			attachedSlotVnum = mouseModule.mouseController.GetAttachedItemIndex()

			Poty_1_ID = mouseModule.mouseController.GetAttachedItemIndex()
			item.SelectItem(attachedSlotVnum)

		item.SelectItem(int(attachedSlotVnum))
		self.PotySlotIcon.LoadImage(str(item.GetIconImageFileName()))
		mouseModule.mouseController.DeattachObject()


	def Set_Poty_2_ID(self):
		global Poty_2_ID
		global Poty_2_Icon
		if mouseModule.mouseController.isAttached():
			attachedSlotType = mouseModule.mouseController.GetAttachedType()
			attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
			attachedSlotVnum = mouseModule.mouseController.GetAttachedItemIndex()

			Poty_2_ID = mouseModule.mouseController.GetAttachedItemIndex()
			item.SelectItem(attachedSlotVnum)

		item.SelectItem(int(attachedSlotVnum))
		self.PotySlotIcon2.LoadImage(str(item.GetIconImageFileName()))
		mouseModule.mouseController.DeattachObject()


	def SetItemSlot(self, renderingSlotNumber, ItemIndex, ItemCount = 0):
		if 0 == ItemIndex or None == ItemIndex:
			wndMgr.ClearSlot(self.hWnd, renderingSlotNumber)
			return

		item.SelectItem(ItemIndex)
		itemIcon = item.GetIconImage()

		item.SelectItem(ItemIndex)
		(width, height) = item.GetItemSize()

		wndMgr.SetSlot(self.hWnd, renderingSlotNumber, ItemIndex, width, height, itemIcon)
		wndMgr.SetSlotCount(self.hWnd, renderingSlotNumber, ItemCount)

	def SelectEmptySlot(self, slot):
		global Dopalacz_ID,Slotlista,cfg,xox,Dopalacz_ID
		if mouseModule.mouseController.isAttached():
			attachedSlotType = mouseModule.mouseController.GetAttachedType()
			attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
			attachedSlotVnum = mouseModule.mouseController.GetAttachedItemIndex()
			itemVnum = player.GetItemIndex(attachedSlotPos)
			Dopalacz_ID[xox] = (attachedSlotVnum)
			Slotlista[xox] = (slot)
			self.itemlist[slot] = attachedSlotPos
			xox += 1
			if player.SLOT_TYPE_INVENTORY == attachedSlotType:
				self.gives.SetItemSlot(slot, itemVnum)
		mouseModule.mouseController.DeattachObject()

	def SelectEmptySlot1(self, slot):
		global xoxx,Otwieracz_ID
		if mouseModule.mouseController.isAttached():
			attachedSlotType = mouseModule.mouseController.GetAttachedType()
			attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
			attachedSlotVnum = mouseModule.mouseController.GetAttachedItemIndex()
			itemVnum = player.GetItemIndex(attachedSlotPos)
			Otwieracz_ID[xoxx] = (attachedSlotVnum)
			self.itemlist1[slot] = attachedSlotPos
			xoxx += 1
			if player.SLOT_TYPE_INVENTORY == attachedSlotType:
				self.gives1.SetItemSlot(slot, itemVnum)
		mouseModule.mouseController.DeattachObject()


	def Dzien_func(self):
		background.SetEnvironmentData(0)

	def Noc_func(self):
		background.SetEnvironmentData(1)

	def Zamknijgre_func(self):
		app.Exit()

	def AutoAtak_func(self, arg):
		if arg=='on':
			self.AutoAttackStartFunc()
			self.AutoAtak.SetText(Przyciskon+'A.Attack ON')
		elif arg=='off':
			self.AutoAttackStopFunc()
			self.AutoAtak.SetText(Przyciskoff+'A.Attack OFF')
	def AutoAttackStartFunc(self):
		self.AutoAttackx = XX()
		self.AutoAttackx.XX1(5)
		self.AutoAttackx.XX2(self.AutoAttackStartFunc)
		if self.Autoaacombo.GetCurrentText() == 'Normal':
			player.SetAttackKeyState(TRUE)

		if self.Autoaacombo.GetCurrentText() == 'Rotatie':
			Direction = app.GetRandom(0,3)
			player.SetAttackKeyState(TRUE)
			chr.SetDirection(Direction)
	def AutoAttackStopFunc(self):
		player.SetAttackKeyState(FALSE)
		self.AutoAttackx=XX()
		self.AutoAttackx.XX1(999999999999)
		self.AutoAttackx.XX2(self.AutoAttackStopFunc)
		player.SetAttackKeyState(FALSE)

	def Poty_func(self, arg):
		if arg=='on':
			self.PotyStartFunc()
			self.Poty.SetText(Przyciskon+'Potiuni ON')
		elif arg=='off':
			self.PotyStopFunc()
			self.Poty.SetText(Przyciskoff+'Potiuni OFF')


	def PotyStartFunc(self):
		global Poty
		global Poty_1_ID
		global Poty_2_ID

		Potyt = self.Potyhpczas.GetText()
		Attack.hp_threshold = int(Potyt)  # Update the shared HP threshold
		maxhp = player.GetStatus(player.MAX_HP)
		aktualnehp = player.GetStatus(player.HP)
		if (float(aktualnehp) / float(maxhp)) * 100 < int(int(Potyt)):
			for i in xrange(player.INVENTORY_PAGE_SIZE*4):
				CzerwonePotki = player.GetItemIndex(i)
				if CzerwonePotki == (int(Poty_1_ID)):
					net.SendItemUsePacket(i)
					break

		Potyt1 = self.Potympczas.GetText()
		maxmp = player.GetStatus(player.MAX_SP)
		aktualnemp = player.GetStatus(player.SP)
		if (float(aktualnemp) / float(maxmp)) * 100 < int(int(Potyt1)):
			for i in xrange(player.INVENTORY_PAGE_SIZE*4):
				NiebieskiePotki = player.GetItemIndex(i)
				if NiebieskiePotki == (int(Poty_2_ID)):
					net.SendItemUsePacket(i)
					break


		self.Potyx = XX()
		self.Potyx.XX1(float(0.1))
		self.Potyx.XX2(self.PotyStartFunc)


	def PotyStopFunc(self):
		self.Potyx = XX()
		self.Potyx.XX1(float(99999999999999999))
		self.Potyx.XX2(self.PotyStopFunc)

	def Mobber_func(self, arg):
		if arg=='on':
			self.MobberStartFunc()
			self.Mobberek.SetText(Przyciskon+'Mantii On')
		elif arg=='off':
			self.MobberStopFunc()
			self.Mobberek.SetText(Przyciskoff+'Mantii OF')

	def MobberStartFunc(self):
		global Mobber_ID,cipa
		if self.Mobbercombo.GetCurrentText() == 'Normal':
			mobbert = self.Peleczas.GetText()
			self.Mobber=XX()
			self.Mobber.XX1(int(mobbert))
			self.Mobber.XX2(self.MobberStartFunc)
			for i in xrange(player.INVENTORY_PAGE_SIZE*4):
				Mobberitemx = player.GetItemIndex(i)
				if Mobberitemx == (int(Mobber_ID)):
					net.SendItemUsePacket(i)
					break

		if self.Mobbercombo.GetCurrentText() == 'Mega':
			self.Mobber=XX()
			self.Mobber.XX1(float(0.2))
			self.Mobber.XX2(self.MobberStartFunc)
			myVid = player.GetMainCharacterIndex()
			mobbert = self.Peleczas.GetText()
			for i in xrange(player.INVENTORY_PAGE_SIZE*4):
				Mobberitemx = player.GetItemIndex(i)
				if Mobberitemx == (int(Mobber_ID)):
					net.SendItemUsePacket(i)
					break
					i=i
			a=1
			while a:
				x, y, z = player.GetMainCharacterPosition()
				if cipa == 0:
					chr.SelectInstance(myVid)
					chr.SetPixelPosition(int(x) - 1200, int(y), int(z))
					player.SetSingleDIKKeyState(app.DIK_UP, TRUE)
					player.SetSingleDIKKeyState(app.DIK_UP, FALSE)
					net.SendItemUsePacket(i)
					net.SendItemUsePacket(i)
					a = 0
					cipa = cipa + 1
					continue
				if cipa == 1:
					chr.SelectInstance(myVid)
					chr.SetPixelPosition(int(x) - 1200, int(y), int(z))
					player.SetSingleDIKKeyState(app.DIK_UP, TRUE)
					player.SetSingleDIKKeyState(app.DIK_UP, FALSE)
					net.SendItemUsePacket(i)
					net.SendItemUsePacket(i)
					a = 0
					cipa = cipa + 1
					continue
				if cipa == 2:
					chr.SelectInstance(myVid)
					chr.SetPixelPosition(int(x) + 1200, int(y), int(z))
					player.SetSingleDIKKeyState(app.DIK_UP, TRUE)
					player.SetSingleDIKKeyState(app.DIK_UP, FALSE)
					net.SendItemUsePacket(i)
					net.SendItemUsePacket(i)
					a = 0
					cipa = cipa + 1
					continue
				if cipa == 3:
					chr.SelectInstance(myVid)
					chr.SetPixelPosition(int(x) + 1200, int(y), int(z))
					player.SetSingleDIKKeyState(app.DIK_UP, TRUE)
					player.SetSingleDIKKeyState(app.DIK_UP, FALSE)
					net.SendItemUsePacket(i)
					net.SendItemUsePacket(i)
					a = 0
					cipa = cipa + 1
					continue
				if cipa == 4:
					chr.SelectInstance(myVid)
					chr.SetPixelPosition(int(x), int(y) - 1200, int(z))
					player.SetSingleDIKKeyState(app.DIK_UP, TRUE)
					player.SetSingleDIKKeyState(app.DIK_UP, FALSE)
					net.SendItemUsePacket(i)
					net.SendItemUsePacket(i)
					a = 0
					cipa = cipa + 1
					continue
				if cipa == 5:
					chr.SelectInstance(myVid)
					chr.SetPixelPosition(int(x), int(y) - 1200, int(z))
					player.SetSingleDIKKeyState(app.DIK_UP, TRUE)
					player.SetSingleDIKKeyState(app.DIK_UP, FALSE)
					net.SendItemUsePacket(i)
					net.SendItemUsePacket(i)
					a = 0
					cipa = cipa + 1
					continue
				if cipa == 6:
					chr.SelectInstance(myVid)
					chr.SetPixelPosition(int(x), int(y) + 1200, int(z))
					player.SetSingleDIKKeyState(app.DIK_UP, TRUE)
					player.SetSingleDIKKeyState(app.DIK_UP, FALSE)
					net.SendItemUsePacket(i)
					net.SendItemUsePacket(i)
					a = 0
					cipa = cipa + 1
					continue
				if cipa == 7:
					chr.SelectInstance(myVid)
					chr.SetPixelPosition(int(x), int(y) + 1200, int(z))
					player.SetSingleDIKKeyState(app.DIK_UP, TRUE)
					player.SetSingleDIKKeyState(app.DIK_UP, FALSE)
					net.SendItemUsePacket(i)
					net.SendItemUsePacket(i)
					a = 0
					cipa = cipa + 1
					continue
				if cipa == 8:
					chr.SelectInstance(myVid)
					chr.SetPixelPosition(int(x) + 1200, int(y), int(z))
					player.SetSingleDIKKeyState(app.DIK_UP, TRUE)
					player.SetSingleDIKKeyState(app.DIK_UP, FALSE)
					net.SendItemUsePacket(i)
					net.SendItemUsePacket(i)
					a = 0
					cipa = cipa + 1
					continue
				if cipa == 9:
					chr.SelectInstance(myVid)
					chr.SetPixelPosition(int(x) + 1200, int(y), int(z))
					player.SetSingleDIKKeyState(app.DIK_UP, TRUE)
					player.SetSingleDIKKeyState(app.DIK_UP, FALSE)
					net.SendItemUsePacket(i)
					net.SendItemUsePacket(i)
					a = 0
					cipa = cipa + 1
					continue
				if cipa == 10:
					chr.SelectInstance(myVid)
					chr.SetPixelPosition(int(x) - 1200, int(y), int(z))
					player.SetSingleDIKKeyState(app.DIK_UP, TRUE)
					player.SetSingleDIKKeyState(app.DIK_UP, FALSE)
					net.SendItemUsePacket(i)
					net.SendItemUsePacket(i)
					a = 0
					cipa = cipa + 1
					continue
				if cipa == 11:
					chr.SelectInstance(myVid)
					chr.SetPixelPosition(int(x) - 1200, int(y), int(z))
					player.SetSingleDIKKeyState(app.DIK_UP, TRUE)
					player.SetSingleDIKKeyState(app.DIK_UP, FALSE)
					net.SendItemUsePacket(i)
					net.SendItemUsePacket(i)
					a = 0
					cipa = cipa + 1
					continue
				if cipa == 12:
					chr.SelectInstance(myVid)
					chr.SetPixelPosition(int(x), int(y) + 1200, int(z))
					player.SetSingleDIKKeyState(app.DIK_UP, TRUE)
					player.SetSingleDIKKeyState(app.DIK_UP, FALSE)
					net.SendItemUsePacket(i)
					net.SendItemUsePacket(i)
					a = 0
					cipa = cipa + 1
					continue
				if cipa == 13:
					chr.SelectInstance(myVid)
					chr.SetPixelPosition(int(x), int(y) + 1200, int(z))
					player.SetSingleDIKKeyState(app.DIK_UP, TRUE)
					player.SetSingleDIKKeyState(app.DIK_UP, FALSE)
					net.SendItemUsePacket(i)
					net.SendItemUsePacket(i)
					a = 0
					cipa = cipa + 1
					continue
				if cipa == 14:
					chr.SelectInstance(myVid)
					chr.SetPixelPosition(int(x), int(y) - 1200, int(z))
					player.SetSingleDIKKeyState(app.DIK_UP, TRUE)
					player.SetSingleDIKKeyState(app.DIK_UP, FALSE)
					net.SendItemUsePacket(i)
					net.SendItemUsePacket(i)
					a = 0
					cipa = cipa + 1
					continue
				if cipa == 15:
					chr.SelectInstance(myVid)
					chr.SetPixelPosition(int(x), int(y) - 1200, int(z))
					player.SetSingleDIKKeyState(app.DIK_UP, TRUE)
					player.SetSingleDIKKeyState(app.DIK_UP, FALSE)
					net.SendItemUsePacket(i)
					net.SendItemUsePacket(i)
					a = 0
					cipa = 0
					self.Mobber=XX()
					self.Mobber.XX1(int(mobbert))
					self.Mobber.XX2(self.MobberStartFunc)
					continue
	def MobberStopFunc(self):
		self.Mobber=XX()
		self.Mobber.XX1(100000000)
		self.Mobber.XX2(self.MobberStopFunc)
		a = 0
		cipa = 0


	def Kamera_func(self, arg):
		if arg=='on':
			x, y, z = player.GetMainCharacterPosition()
			app.SetCameraSetting(int(x), int(-y), int(z), 5000, 10, 30)
			self.Kamera.SetText(Przyciskon+'Camera Mod on')
		elif arg=='off':
			app.SetDefaultCamera()
			self.Kamera.SetText(Przyciskoff+'Camera Mod Off')

	def Snieg_func(self, arg):
		if arg=='on':
			background.EnableSnow(1)
			self.Snieg.SetText(Przyciskon+'Ninsoare')
		elif arg=='off':
			background.EnableSnow(0)
			self.Snieg.SetText(Przyciskoff+'Ninsoare')

	def Combo_func(self, arg):
		if arg=='on':
			chr.testSetComboType(2)
			self.Combo.SetText(Przyciskon+'Combo')

		elif arg=='off':
			chr.testSetComboType(0)
			self.Combo.SetText(Przyciskoff+'Combo')


	def AutoWstawanie_func(self, arg):
		if arg=='on':
			self.Autowstawaniestart()
			self.AutoWstawanie.SetText(Przyciskon+'Respawn')
		elif arg=='off':
			self.Autowstawaniestop()
			self.AutoWstawanie.SetText(Przyciskoff+'Respawn')

	def Autowstawaniestart(self):
		global autowtawanie_pozwo
		self.awfunc=XX()
		self.awfunc.XX1(15.0)
		self.awfunc.XX2(self.Autowstawaniestart)
		maxhp=player.GetStatus(player.MAX_HP)
		aktualnehp=player.GetStatus(player.HP)
		if autowtawanie_pozwo == 1 and float(aktualnehp) / float(maxhp) * 100 >= int(50):
			#oldSendChatPacket('Start')
			autowtawanie_pozwo = 0
		if float(aktualnehp) / float(maxhp) * 100 <= int(0):
			player.SetAttackKeyState(FALSE)
			#oldSendChatPacket('Stop')
			oldSendChatPacket('/restart_here')
			autowtawanie_pozwo = 1

	def Autowstawaniestop(self):
		self.awfunc=XX()
		self.awfunc.XX1(100000000.0)
		self.awfunc.XX2(self.Autowstawaniestop)


	def usunpotehp(self):
		global Poty_1_ID
		Poty_1_ID = 0
		self.PotySlotIcon.LoadImage('d:/ymir work/ui/public/Slot_Base.sub')

	def usunpotemp(self):
		global Poty_2_ID
		Poty_2_ID = 0
		self.PotySlotIcon2.LoadImage('d:/ymir work/ui/public/Slot_Base.sub')

	def usunpele(self):
		global Mobber_ID
		Mobber_ID = 0
		self.MobberSlotIcon.LoadImage('d:/ymir work/ui/public/Slot_Base.sub')


	def Skill3Buff_func(self, arg):
		if arg=='on':
			self.Skill3Buff()
		elif arg=='off':
			self.Skill3Buffstop()

	def Skill4Buff_func(self, arg):
		if arg=='on':
			self.Skill4Buff()
		elif arg=='off':
			self.Skill4Buffstop()

	def Skill5buff_func(self, arg):
		if arg=='on':
			self.Skill5buff()
		elif arg=='off':
			self.Skill5buffstop()

	def Skill6buff_func(self, arg):
		if arg=='on':
			self.Skill6buff()
		elif arg=='off':
			self.Skill6buffstop()

	def Skill6buff_func1(self, arg):
		if arg=='on':
			self.Skill6buff1()
		elif arg=='off':
			self.Skill6buffstop1()

	def horse(self):
		oldSendChatPacket('/ride')

	def Skil3(self):
		player.ClickSkillSlot(3)

	def Skil4(self):
		player.ClickSkillSlot(4)

	def Skil5(self):
		player.ClickSkillSlot(5)

	def Skil6(self):
		player.ClickSkillSlot(6)

	def Skill3Buff(self):
		if player.IsMountingHorse():
			oldSendChatPacket('/unmount')
			self.Skill30 = XX()
			self.Skill30.XX1(1)
			self.Skill30.XX2(self.Skil3)
			self.Skill31 = XX()
			self.Skill31.XX1(3)
			self.Skill31.XX2(self.horse)
			self.Skill32 = XX()
			self.Skill32.XX1(int(self.Skill2czas.GetText()))
			self.Skill32.XX2(self.Skill3Buff)
			self.Skill31.XX2(self.horse)
			self.Skill31.XX2(self.horse)
		else:
			self.Skill30 = XX()
			self.Skill30.XX1(1)
			self.Skill30.XX2(self.Skil3)
			self.Skill32 = XX()
			self.Skill32.XX1(int(self.Skill2czas.GetText()))
			self.Skill32.XX2(self.Skill3Buff)

	def Skill3Buffstop(self):
		self.Skill32 = XX()
		self.Skill32.XX1(int(9999999999999))
		self.Skill32.XX2(self.Skill3Buff)

	def Skill4Buff(self):
		if player.IsMountingHorse():
			oldSendChatPacket('/unmount')
			self.Skill40 = XX()
			self.Skill40.XX1(1)
			self.Skill40.XX2(self.Skil4)
			self.Skill41 = XX()
			self.Skill41.XX1(3)
			self.Skill41.XX2(self.horse)
			self.Skill42 = XX()
			self.Skill42.XX1(int(self.Skill1czas.GetText()))
			self.Skill42.XX2(self.Skill4Buff)
			self.Skill31.XX2(self.horse)
			self.Skill31.XX2(self.horse)
		else:
			self.Skill40 = XX()
			self.Skill40.XX1(1)
			self.Skill40.XX2(self.Skil4)
			self.Skill42 = XX()
			self.Skill42.XX1(int(self.Skill1czas.GetText()))
			self.Skill42.XX2(self.Skill4Buff)
	def Skill4Buffstop(self):
		self.Skill42 = XX()
		self.Skill42.XX1(int(9999999999999))
		self.Skill42.XX2(self.Skill4Buff)

	def Skill5buff(self):
		if player.IsMountingHorse():
			oldSendChatPacket('/unmount')
			self.Skill50 = XX()
			self.Skill50.XX1(1)
			self.Skill50.XX2(self.Skil5)
			self.Skill51 = XX()
			self.Skill51.XX1(3)
			self.Skill51.XX2(self.horse)
			self.Skill52 = XX()
			self.Skill52.XX1(int(self.Skill3czas.GetText()))
			self.Skill52.XX2(self.Skill5buff)
			self.Skill31.XX2(self.horse)
			self.Skill31.XX2(self.horse)
		else:
			self.Skill50 = XX()
			self.Skill50.XX1(1)
			self.Skill50.XX2(self.Skil5)
			self.Skill52 = XX()
			self.Skill52.XX1(int(self.Skill3czas.GetText()))
			self.Skill52.XX2(self.Skill5buff)
	def Skill5buffstop(self):
		self.Skill52 = XX()
		self.Skill52.XX1(int(9999999999))
		self.Skill52.XX2(self.Skill5buff)

	def Skill6buff(self):
		if player.IsMountingHorse():
			oldSendChatPacket('/unmount')
			self.Skill60 = XX()
			self.Skill60.XX1(1)
			self.Skill60.XX2(self.Skil6)
			self.Skill61 = XX()
			self.Skill61.XX1(3)
			self.Skill61.XX2(self.horse)
			self.Skill62 = XX()
			self.Skill62.XX1(int(self.Skill1czas.GetText()))
			self.Skill62.XX2(self.Skill6buff)
			self.Skill31.XX2(self.horse)
			self.Skill31.XX2(self.horse)
		else:
			self.Skill60 = XX()
			self.Skill60.XX1(1)
			self.Skill60.XX2(self.Skil6)
			self.Skill62 = XX()
			self.Skill62.XX1(int(self.Skill1czas.GetText()))
			self.Skill62.XX2(self.Skill6buff)

	def Skill6buffstop(self):
		self.Skill62 = XX()
		self.Skill62.XX1(int(999999999))
		self.Skill62.XX2(self.Skill6buff)

	def Skill6buff1(self):
		if player.IsMountingHorse():
			oldSendChatPacket('/unmount')
			self.Skill601 = XX()
			self.Skill601.XX1(1)
			self.Skill601.XX2(self.Skil6)
			self.Skill611 = XX()
			self.Skill611.XX1(3)
			self.Skill611.XX2(self.horse)
			self.Skill621 = XX()
			self.Skill621.XX1(int(self.Skill2czas.GetText()))
			self.Skill621.XX2(self.Skill6buff1)

		else:
			self.Skill60 = XX()
			self.Skill60.XX1(1)
			self.Skill60.XX2(self.Skil6)
			self.Skill62 = XX()
			self.Skill62.XX1(int(self.Skill1czas.GetText()))
			self.Skill62.XX2(self.Skill6buff)
	def Skill6buffstop1(self):
		self.Skill621 = XX()
		self.Skill621.XX1(int(999999999))
		self.Skill621.XX2(self.Skill6buff1)



	def Teleportuj_Do_Kordow_M_(self):
		global met,papec,Pozwolenie,teleport_mode,telestep,TeleportState, Debug, GetCurrentMapSize, TeleportToDest, DivideToFloat,ch
		self.Wykrywaczmetinow=XX()
		self.Wykrywaczmetinow.XX1(5)
		self.Wykrywaczmetinow.XX2(self.Teleportuj_Do_Kordow_M_)

		if Pozwolenie == 0:
			if os.path.exists('c:montes_cfg/farm_cords.cfg'):
				kordziki = open('c:montes_cfg/farm_cords.cfg', 'r').read().split()
			My_VID = player.GetMainCharacterIndex()
			x_coordinate1 = int(kordziki[papec])
			y_coordinate1 = int(kordziki[papec+1])
			Position_Xx = int(kordziki[papec])
			Position_Yx = int(kordziki[papec+1])
			myVid = player.GetMainCharacterIndex()
			chr.SelectInstance(myVid)
			self.TeleportToDest((Position_Xx+10)*100,(Position_Yx+10)*100)
			chr.MoveToDestPosition(int(myVid),Position_Xx*100,Position_Yx*100)
			self.Auto_Atak_M_x()
			(My_X1, My_Y1, My_Z1) = chr.GetPixelPosition(myVid)
			My_X1 = int(My_X1 / 100)
			My_Y1 = int(My_Y1 / 100)
			if (My_X1 >= (x_coordinate1)-20 and My_X1 <= (x_coordinate1)+20) and (My_Y1 >= (y_coordinate1)-20 and My_Y1 <= (y_coordinate1)+20):
				if papec != (len(kordziki)):
					papec += 2
				if papec == (len(kordziki)):
					papec = 0
			self.PozycjaText.SetText(str('Pozycja ')+str(papec/2)+str(' / ')+str((len(kordziki)/2)))
	def Teleportuj_Do_Kordow_M_Stop(self):
		self.Wykrywaczmetinow=XX()
		self.Wykrywaczmetinow.XX1(19999999999990)
		self.Wykrywaczmetinow.XX2(self.Teleportuj_Do_Kordow_M_)

	def Auto_Atak_M_x(self):
		global listatypow,TargetVid2,TargetVid,VidyPozwolenie,ListaVidow,ListaVidow2,listash2
		listatypow = []
		Odleglosc = 149
		Moja_Pozycja = player.GetMainCharacterIndex()
		My_VID1 = player.GetMainCharacterIndex()
		(My_X1, My_Y1, My_Z1) = chr.GetPixelPosition(My_VID1)
		My_X1 = int(My_X1 )
		My_Y1 = int(My_Y1 )
		o=player.GetMainCharacterIndex()
		for i in listash2:
			Typ = chr.GetInstanceType(i)
			if chr.HasInstance(i):
				if Typ==2:
					(xm,ym,zm)= chr.GetPixelPosition(i)
					chr.SelectInstance(My_VID1)
					self.TeleportToDest(xm,ym)

	def Auto_Atak_M_(self):
		global listatypow,Pozwolenie,TargetVid2,TargetVid,VidyPozwolenie,ListaVidow,ListaVidow2,listash2
		listatypow = []
		Odleglosc = 149
		Odleglosc1 = []
		teleportek = 1
		self.Wykrywaczmetinow44=XX()
		self.Wykrywaczmetinow44.XX1(1)
		self.Wykrywaczmetinow44.XX2(self.Auto_Atak_M_)
		Moja_Pozycja = player.GetMainCharacterIndex()
		My_VID1 = player.GetMainCharacterIndex()
		(My_X1, My_Y1, My_Z1) = chr.GetPixelPosition(My_VID1)
		My_X1 = int(My_X1 )
		My_Y1 = int(My_Y1 )
		o=player.GetMainCharacterIndex()


		for i in listash2:
			Typ = chr.GetInstanceType(i)
			if chr.HasInstance(i):
				listatypow.append(Typ)
				if listatypow.count(int(2)) == 1:
					Pozwolenie = 1
					teleportek = 0
				if	listatypow.count(int(2))== 0:
					Pozwolenie = 0
					player.SetAttackKeyState(FALSE)
					teleportek = 1
				if listatypow.count(int(2)) >= 2:
					teleportek = 1
					Pozwolenie = 1
			else:
				pass
			if Typ==2:
				dystans = player.GetCharacterDistance(i)
				b = (dystans,i)
				Odleglosc1.append(b)
				Odleglosc1.sort()
				if (Odleglosc1[0][0]) > 150:
					if teleportek == 0:
						x, y, z = chr.GetPixelPosition(Odleglosc1[0][1])
						self.TeleportToDest(int(x+10), int(y+10))
						player.SetAttackKeyState(FALSE)
				else:
					player.SetAttackKeyState(TRUE)


	def Auto_Atak_M_Stop(self):
		global Odleglosc
		self.Wykrywaczmetinow44=XX()
		self.Wykrywaczmetinow44.XX1(19999999999999999999)
		self.Wykrywaczmetinow44.XX2(self.Auto_Atak_M_)
		player.SetAttackKeyState(FALSE)


	def Rozdziel_func(self):
		global SlotDoDzielenia
		IloscStacku = (float(self.PoIleRozdzielicIn.GetText()))
		for i in xrange(player.INVENTORY_PAGE_SIZE*4):
			if player.GetItemIndex(i) == 0:
				net.SendItemMovePacket(SlotDoDzielenia, i, IloscStacku)


	def Ulepsz_Item(self):
		global SlotUlepszania
		if mouseModule.mouseController.isAttached():
			attachedSlotType = mouseModule.mouseController.GetAttachedType()
			attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
			attachedSlotVnum = mouseModule.mouseController.GetAttachedItemIndex()

			SlotUlepszania = mouseModule.mouseController.GetAttachedSlotNumber()
			item.SelectItem(attachedSlotVnum)

		item.SelectItem(int(attachedSlotVnum))
		self.UlepszanieSlotIcon.LoadImage(str(item.GetIconImageFileName()))
		mouseModule.mouseController.DeattachObject()

	def SlotUleDo(self):
		global SlotUlepszaniaDO
		if mouseModule.mouseController.isAttached():
			attachedSlotType = mouseModule.mouseController.GetAttachedType()
			attachedSlotPos = mouseModule.mouseController.GetAttachedSlotNumber()
			attachedSlotVnum = mouseModule.mouseController.GetAttachedItemIndex()

			SlotUlepszaniaDO = attachedSlotVnum
			item.SelectItem(attachedSlotVnum)

		item.SelectItem(int(attachedSlotVnum))
		self.UlepszanieSlotIconx.LoadImage(str(item.GetIconImageFileName()))
		mouseModule.mouseController.DeattachObject()


	def __OnScroll(self):
		board_count = len(self.gui['boards'])
		pos = int(self.gui['scrollbar'].GetPos() * (len(listaWysz) - 7))
		for l in xrange(board_count):
			realPos = l + pos
			self.gui['boards'][l].SetBoardInfo(realPos, listaWysz[realPos][0], listaWysz[realPos][2], listaWysz[realPos][1])

	def CreateScrollbar(self, parent, height, x, y):
		scrollbar = ui.ScrollBar()
		scrollbar.SetParent(parent)
		scrollbar.SetScrollBarSize(height)
		scrollbar.SetPosition(x, y)
		scrollbar.Show()
		return scrollbar



	def OpenWindowZap(self):
		if self.BoardZap.IsShow():
			self.BoardZap.Hide()
		else:
			self.BoardZap.Show()

	def CloseZap(self):
		self.BoardZap.Hide()


	def OpenWindow(self):
		global SPAM
		self.btns3.Hide()
		SPAM = TRUE
		if self.Board1.IsShow():
			self.Board1.Hide()
		else:
			self.Board1.Show()

	def OpenWindowUstawieniao(self):
		if self.Ustawieniao.IsShow():
			self.Ustawieniao.Hide()
		else:
			self.Ustawieniao.Show()

	def OpenWindowPogoda(self):
		if self.Pogoda.IsShow():
			self.Pogoda.Hide()
		else:
			self.Pogoda.Show()


	def AutoSkileWindow(self):
		self.Aurax.Hide()
		self.Berekx.Hide()
		self.Silnex.Hide()
		self.Ostrzex.Hide()
		self.Strachx.Hide()
		self.Zbrojax.Hide()
		self.Ochronkax.Hide()
		self.Duchx.Hide()
		self.Pomocx.Hide()
		self.Odbiciex.Hide()
		self.Blogox.Hide()
		self.Zwinnoscx.Hide()
		self.Atakx.Hide()
		self.Aurax1.Hide()
		self.Berekx1.Hide()
		self.Silnex1.Hide()
		self.Ostrzex1.Hide()
		self.Strachx1.Hide()
		self.Zbrojax1.Hide()
		self.Ochronkax1.Hide()
		self.Duchx1.Hide()
		self.Pomocx1.Hide()
		self.Blogox1.Hide()
		self.Odbiciex1.Hide()
		self.Zwinnoscx1.Hide()
		self.Atakx1.Hide()

		self.slotbar_Skill1czas.Hide()
		self.slotbar_Skill2czas.Hide()
		self.slotbar_Skill3czas.Hide()

		Race = net.GetMainActorRace()
		Group = net.GetMainActorSkillGroup()
		if (Race == 0 and Group == 1) or (Race == 4 and Group == 1):
			self.Aurax.Show()
			self.Berekx.Show()
			self.Aurax1.Show()
			self.Berekx1.Show()
			self.slotbar_Skill1czas.Show()
			self.slotbar_Skill2czas.Show()
			#self.AutoSkileBoard.SetSize(112, 110)
		if (Race == 0 and Group == 2) or (Race == 4 and Group == 2):
			self.Silnex.Show()
			self.Silnex1.Show()
			self.slotbar_Skill1czas.Show()
			#self.AutoSkileBoard.SetSize(72, 110)
		if (Race == 2 and Group == 1) or (Race == 6 and Group == 1):
			self.Ostrzex.Show()
			self.Strachx.Show()
			self.Zbrojax.Show()
			self.Ostrzex1.Show()
			self.Strachx1.Show()
			self.Zbrojax1.Show()
			self.slotbar_Skill1czas.Show()
			self.slotbar_Skill2czas.Show()
			self.slotbar_Skill3czas.Show()
			#self.AutoSkileBoard.SetSize(152, 110)
		if (Race == 2 and Group == 2) or (Race == 6 and Group == 2):
			self.Ochronkax.Show()
			self.Duchx.Show()
			self.Ochronkax1.Show()
			self.Duchx1.Show()
			self.slotbar_Skill1czas.Show()
			self.slotbar_Skill2czas.Show()
			#self.AutoSkileBoard.SetSize(112, 110)
		if (Race == 3 and Group == 1) or (Race == 7 and Group == 1):
			self.Blogox.Show()
			self.Odbiciex.Show()
			self.Pomocx.Show()
			self.Blogox1.Show()
			self.Odbiciex1.Show()
			self.Pomocx1.Show()
			self.slotbar_Skill1czas.Show()
			self.slotbar_Skill2czas.Show()
			self.slotbar_Skill3czas.Show()
		    #self.AutoSkileBoard.SetSize(152, 110)
		if (Race == 3 and Group == 2) or (Race == 7 and Group == 2):
			self.Zwinnoscx.Show()
			self.Atakx.Show()
			self.Zwinnoscx1.Show()
			self.Atakx1.Show()
			self.slotbar_Skill1czas.Show()
			self.slotbar_Skill3czas.Show()
			self.slotbar_Skill3czas.SetPosition(63, 75)
			#self.AutoSkileBoard.SetSize(112, 110)

	def OpenWindowInfo(self):
		if self.BoardInfo.IsShow():
			self.BoardInfo.Hide()
		else:
			self.BoardInfo.Show()


	def Close(self):
		global SPAM
		self.Board1.Hide()
		self.btns3.Show()
		SPAM = FALSE

	def Ustawieniaoclose(self):
		self.Ustawieniao.Hide()

	def ClosePogoda(self):
		self.Pogoda.Hide()

	def CloseInfo(self):
		self.BoardInfo.Hide()

class XX(ui.ScriptWindow):

	def __init__(self):
		ui.ScriptWindow.__init__(self)
		self.eventTimeOver = lambda *arg: None
		self.eventExit = lambda *arg: None

	def __del__(self):
		ui.ScriptWindow.__del__(self)

	def XX1(self, waitTime):
		curTime = time.clock()
		self.endTime = curTime + waitTime

		self.Show()

	def Close(self):
		self.Destroy()

	def Destroy(self):
		self.Hide()

	def XX2(self, event):
		self.eventTimeOver = ui.__mem_func__(event)

	def SAFE_SetExitEvent(self, event):
		self.eventExit = ui.__mem_func__(event)


	def OnUpdate(self):
		lastTime = max(0, self.endTime - time.clock())
		if 0 == lastTime:
			self.Close()
			self.eventTimeOver()

		else:
			return

class ExampleBoard(ui.ScriptWindow):
	def __init__(self, parent, x, y):
		ui.ScriptWindow.__init__(self)
	def Podglad(self):
		global listaWysz
		net.SendOnClickPacket(listaWysz[self.index][4])
	def Teleport(self):
		global listaWysz
		(XS, YS, ZS) = chr.GetPixelPosition(listaWysz[self.index][4])
		self.TeleportToDest(XS,YS)
	def DivideToFloat(self,x, y):
		try:
			return x * (y**-1)
		except:
			return 0
	def GetTmpTeleport(self,DestX, DestY):
		global DivideToFloat
		(PlayerX, PlayerY, PlayerZ) = player.GetMainCharacterPosition()
		DifX = DestX - PlayerX
		DifY = DestY - PlayerY
		Vektor = self.DivideToFloat(2000, math.sqrt(DifX**2 + DifY**2))
		TempX = PlayerX + Vektor*DifX
		TempY = PlayerY + Vektor*DifY
		Count = self.DivideToFloat((DestX - PlayerX), (Vektor*DifX))
		return (TempX, TempY, Count)
	def Debug(self):
		player.SetSingleDIKKeyState(app.DIK_UP, TRUE)
		player.SetSingleDIKKeyState(app.DIK_UP, FALSE)
	def TeleportToDest(self,aimx, aimy):
		global Debug, GetTmpTeleport, TeleportState
		(TmpX, TmpY, Count) = self.GetTmpTeleport(aimx, aimy)
		TmpCount = 0
		myVid = player.GetMainCharacterIndex()
		while TmpCount < Count:
			chr.SelectInstance(myVid)
			(TmpX, TmpY, Crap) = self.GetTmpTeleport(aimx, aimy)
			chr.SetPixelPosition(int(TmpX), int(TmpY))
			TmpCount += 1
			self.Debug()
		chr.SetPixelPosition(int(aimx), int(aimy))
		self.Debug()
		TeleportState = 1
		chr.SelectInstance(myVid)
	def SetBoardInfo(self, index, price, amount, nazwa):
		self.index = index
		self.gui['textlines']['cena'].SetText('Cena: %s Yang' % price)
		self.gui['textlines']['ilosc'].SetText('Ilosc: %s' % amount)
		self.gui['textlines']['Nume TP'].SetText('Nume TP: %s' % nazwa)
	def GetIndex(self):
		return self.index
	def HideBoard(self):
		self.board.Hide()
	def ShowBoard(self):
		self.board.Show()
	def CreateTextline(self, parent, x, y):
		textline = ui.TextLine()
		textline.SetParent(parent)
		textline.SetPosition(x,y)
		textline.Show()
		return textline
	def CreateBoard(self, parent, width, height, x, y):
		board = ui.Board()
		board.SetParent(parent)
		board.SetSize(width, height)
		board.SetPosition(x, y)
		board.SetTop()
		board.Show()
		return board
	def CreateButton(self, parent, x, y, text, event):
		button = ui.Button()
		button.SetParent(parent)
		button.SetPosition(x, y)
		button.SetUpVisual('d:/ymir work/ui/public/Middle_Button_01.sub')
		button.SetOverVisual('d:/ymir work/ui/public/Middle_Button_02.sub')
		button.SetDownVisual('d:/ymir work/ui/public/Middle_Button_03.sub')
		button.SetText(text)
		button.SetEvent(event)
		button.Show()
		return button
	def __del__(self):
		ui.ScriptWindow.__del__(self)
	def Destroy(self):
		self.Hide()


class Component:
	def Button(self, parent, buttonName, tooltipText, x, y, func, UpVisual, OverVisual, DownVisual):
		button = ui.Button()
		if parent != None:
			button.SetParent(parent)
		button.SetPosition(x, y)
		button.SetUpVisual(UpVisual)
		button.SetOverVisual(OverVisual)
		button.SetDownVisual(DownVisual)
		button.SetText(buttonName)
		button.SetToolTipText(tooltipText)
		button.Show()
		button.SetEvent(func)
		return button

	def ToggleButton(self, parent, buttonName, tooltipText, x, y, funcUp, funcDown, UpVisual, OverVisual, DownVisual):
		button = ui.ToggleButton()
		if parent != None:
			button.SetParent(parent)
		button.SetPosition(x, y)
		button.SetUpVisual(UpVisual)
		button.SetOverVisual(OverVisual)
		button.SetDownVisual(DownVisual)
		button.SetText(buttonName)
		button.SetToolTipText(tooltipText)
		button.Show()
		button.SetToggleUpEvent(funcUp)
		button.SetToggleDownEvent(funcDown)
		return button


	def EditLine(self, parent, editlineText, x, y, width, heigh, max):
		SlotBar = ui.SlotBar()
		if parent != None:
			SlotBar.SetParent(parent)
		SlotBar.SetSize(width, heigh)
		SlotBar.SetPosition(x, y)
		SlotBar.Show()
		Value = ui.EditLine()
		Value.SetParent(SlotBar)
		Value.SetSize(width, heigh)
		Value.SetPosition(5, 1)
		Value.SetMax(max)
		Value.SetLimitWidth(width)
		Value.SetMultiLine()
		Value.SetText(editlineText)
		Value.Show()
		return SlotBar, Value

	def TextLine(self, parent, textlineText, x, y, color,rozmiar):
		textline = ui.TextLine()
		textline.SetFontName(rozmiar)
		if parent != None:
			textline.SetParent(parent)
		textline.SetPosition(x, y)
		if color != None:
			textline.SetFontColor(color[0], color[1], color[2])
		textline.SetText(textlineText)
		textline.Show()
		return textline

	def SlotbarText(self, parent, editlineText, x, y, width, heigh):
		SlotBar = ui.SlotBar()
		SlotBar.SetParent(parent)
		SlotBar.SetSize(width, heigh)
		SlotBar.SetPosition(x, y)
		SlotBar.Show()
		TextLine = ui.TextLine()
		TextLine.SetParent(SlotBar)
		TextLine.SetText(editlineText)
		TextLine.SetHorizontalAlignCenter()
		TextLine.SetVerticalAlignCenter()
		TextLine.SetWindowHorizontalAlignCenter()
		TextLine.SetWindowVerticalAlignCenter()
		TextLine.Show()
		return SlotBar, TextLine


	def RGB(self, r, g, b):
		return (r*255, g*255, b*255)

	def SliderBar(self, parent, sliderPos, func, x, y):
		Slider = ui.SliderBar()
		if parent != None:
			Slider.SetParent(parent)
		Slider.SetPosition(x, y)
		Slider.SetSliderPos(sliderPos / 100)
		Slider.Show()
		Slider.SetEvent(func)
		return Slider

	def ExpandedImage(self, parent, x, y, img):
		image = ui.ExpandedImageBox()
		if parent != None:
			image.SetParent(parent)
		image.SetPosition(x, y)
		image.LoadImage(img)
		image.Show()
		return image

	def ComboBox(self, parent, text, x, y, width):
		combo = ui.ComboBox()
		if parent != None:
			combo.SetParent(parent)
		combo.SetPosition(x, y)
		combo.SetSize(width, 15)
		combo.SetCurrentItem(text)
		combo.Show()
		return combo

	def ThinBoard(self, parent, moveable, x, y, width, heigh, center):
		thin = ui.ThinBoard()
		if parent != None:
			thin.SetParent(parent)
		if moveable == TRUE:
			thin.AddFlag('movable')
			thin.AddFlag('float')
		thin.SetSize(width, heigh)
		thin.SetPosition(x, y)
		if center == TRUE:
			thin.SetCenterPosition()
		thin.Show()
		return thin

	def Gauge(self, parent, width, color, x, y):
		gauge = ui.Gauge()
		if parent != None:
			gauge.SetParent(parent)
		gauge.SetPosition(x, y)
		gauge.MakeGauge(width, color)
		gauge.Show()
		return gauge

	def ListBoxEx(self, parent, x, y, width, heigh, SVIC, event):
		bar = ui.Bar()
		if parent != None:
			bar.SetParent(parent)
		bar.SetPosition(x, y)
		bar.SetSize(width, heigh)
		bar.SetColor(0x77000000)
		bar.Show()
		ListBox=ui.ListBoxEx()
		ListBox.SetParent(bar)
		ListBox.SetPosition(0, 0)
		ListBox.SetSize(width, heigh)
		ListBox.SetViewItemCount(SVIC)
		ListBox.SetSelectEvent(event)
		ListBox.Show()
		scroll = ui.ScrollBar()
		scroll.SetParent(ListBox)
		scroll.SetPosition(width-15, 0)
		scroll.SetScrollBarSize(heigh)
		scroll.Show()
		ListBox.SetScrollBar(scroll)
		return bar, ListBox

	def ListBoxEx1(self, parent, x, y, width, heigh, SVIC, event):
		bar = ui.Bar()
		if parent != None:
			bar.SetParent(parent)
		bar.SetPosition(x, y)
		bar.SetSize(width, heigh)
		bar.SetColor(0x77000000)
		bar.Show()
		ListBox=ui.ListBoxEx()
		ListBox.SetParent(bar)
		ListBox.SetPosition(0, 0)
		ListBox.SetSize(width, heigh)
		ListBox.SetViewItemCount(SVIC)
		ListBox.SetSelectEvent(event)
		ListBox.Show()
		scroll = ui.ScrollBar()
		scroll.SetParent(ListBox)
		scroll.SetPosition(width-15, 0)
		scroll.SetScrollBarSize(heigh)
		scroll.Show()
		ListBox.SetScrollBar(scroll)
		return bar, ListBox

	def HorizontalBar(self, parent, x, y, Create):
		horizontalBar = ui.HorizontalBar()
		if parent != None:
			horizontalBar.SetParent(parent)
		horizontalBar.SetPosition(x, y)
		horizontalBar.Create(Create)
		horizontalBar.Show()
		return horizontalBar

	def GetCurrentText(self):
		return self.textLine.GetText()
	def OnSelectItem(self, index, name):
		self.SetCurrentItem(name)
		self.CloseListBox()
		self.event()
	ui.ComboBox.GetCurrentText = GetCurrentText
	ui.ComboBox.OnSelectItem = OnSelectItem

class Item(ui.ListBoxEx.Item):
	def __init__(self, text):
		ui.ListBoxEx.Item.__init__(self)
		self.canLoad=0
		self.text=text
		self.textLine=self.__CreateTextLine(text[:1000])
	def __del__(self):
		ui.ListBoxEx.Item.__del__(self)
	def GetText(self):
		return self.text
	def SetSize(self, width, height):
		ui.ListBoxEx.Item.SetSize(self,2*len(self.textLine.GetText()) + 35, height)
	def __CreateTextLine(self, text):
		textLine=ui.TextLine()
		textLine.SetParent(self)
		textLine.SetPosition(0, 0)
		textLine.SetText(text)
		textLine.Show()
		return textLine

	#########################################

	#############################################################
# Remade the functionality from iPUSH, optimized and changed by KolenMG - M2.Dev - Althaia Project. 
#############################################################

import sys, os, math, time


### Classes instead of packages & modules to fit all in one file ###
class SimpleRandom:
    def __init__(self, seed=12345):
        self.seed = seed

    def rand(self):
        a = 1664525
        c = 1013904223
        m = 2**32
        self.seed = (a * self.seed + c) % m
        return self.seed

    def randint(self, min_val, max_val):
        return min_val + self.rand() % (max_val - min_val + 1)

simple_random = SimpleRandom(seed=12345)  # Initialize with a specific seed
class Util:
    class File:
        @staticmethod
        def read(path):
            try:
                f = open(path, 'r')
                lines = f.readlines()
                f.close()
                return lines
            except:
                pass

        @staticmethod
        def write(path, name, id, time):
            f = open(path, 'a+')
            f.write("Name=%s\n" % (name))
            f.write("ID=%i\n" % (id))
            f.write("Time=%i\n\n" % (time))
            f.close()

        @classmethod
        def removeLastLines(cls, path, num):
            allLines = cls.read(path)
            f = open(path, "w+")
            f.writelines(allLines[:-num])
            f.close()

    class Module:
        @classmethod
        def find(cls, name, atr):
            for module_name in sys.modules:
                if name == '*' or name in module_name:
                    try:
                        module = __import__(module_name)
                        getattr(module, atr)
                        return module
                    except:
                        pass

            return cls.find('*', atr)

        @staticmethod
        def dump(path):
            for m in sys.modules:
                f = open(path, 'a+')
                
                try:
                    module = __import__(m)

                    f.write("\n# %s\nimport %s\n" % (m, m))

                    for atr in dir(module):
                        atr_name = atr
                        atr = getattr(module, atr)

                        if callable(atr):
                            f.write("%s.%s() # %s\n" % (m, atr_name, type(atr)))
                            for sub_atr in ['__code__', '__defaults__', '__kwdefaults__']:
                                if sub_atr == '__code__':
                                    for co_atr in ['co_argcount', 'co_varnames', 'co_nlocals', 'co_names', 'co_cellvars', 'co_consts', 'co_freevars', 'co_posonlyargcount', 'co_kwonlyargcount']:
                                        try:
                                            f.write("%s.%s.%s.%s = %s\n" % (m, atr_name, sub_atr, co_atr, getattr(getattr(atr, sub_atr), co_atr)))
                                        except:
                                            pass
                                else:
                                    try:
                                        f.write("%s.%s.%s = %s\n" % (m, atr_name, sub_atr, getattr(atr, sub_atr)))
                                    except:
                                        pass
                        else:
                            f.write("%s.%s # %s\n" % (m, atr_name, type(atr)))
                except:
                    f.write("\n# ImportError: No module named %s\n" % (m))

                f.close()

### Import Metin2 modules ###
app = Util.Module.find('app', 'GetRandom')
item = Util.Module.find('item', 'GetItemName')
chr = Util.Module.find('chr', 'SetRotation')
net = Util.Module.find('net', 'SendChatPacket')
player = Util.Module.find('player', 'SetAttackKeyState')
ui = Util.Module.find('ui', 'ThinBoard')
textTail = Util.Module.find('textTail', 'Pick')
chat = Util.Module.find('chat', 'AppendChat')
systemSetting = Util.Module.find('systemSetting', 'GetCurrentResolution')

class Mt2:
    class Event:
        def __init__(self, delay, task, name=None, repeat=False):
            self.delay = delay
            self.task = task
            self.name = name
            self.repeat = repeat
            self.over = False
            self.timeOfExec = time.clock() + delay

        def run(self, curTime):
            if not self.over and curTime >= self.timeOfExec:
                self.timeOfExec = curTime + self.delay
                self.task()
                self.over = True if not self.repeat else False

    class EventHandler(ui.Window):
        def __init__(self, layer = "UI"):
            ui.Window.__init__(self, layer)
            self.events = []
            self.add(10, self.clean, "Clean Events", True)
            self.Show()

        def __del__(self):
            ui.Window.__del__(self)

        def get(self, name):
            return next((e for e in self.events if e.name == name), None)

        def has(self, name):
            return eventHandler.get(name) is not None

        def add(self, delay, task, name=None, repeat=False):
            self.events.append(Mt2.Event(delay, task, name, repeat))

        def remove(self, name, task=None):
            self.events = [e for e in self.events if e.name != name]
            if task is not None: task()

        def pause(self, name, delay):
            event = self.get(name)
            if event is None: return
            self.remove(name)
            self.add(delay, lambda: self.add(event.delay, event.task, event.name, event.repeat))

        def clean(self):
            self.events = [e for e in self.events if not e.over]

        def OnUpdate(self):
            curTime = time.clock()
            for event in self.events: event.run(curTime)

    class Gui:
        @staticmethod
        def addThinBoard(parent, x, y, width, heigh):
            board = ui.ThinBoard()
            if parent is not None: board.SetParent(parent)
            board.AddFlag('movable')
            board.AddFlag('float')
            board.SetPosition(x, y)
            board.SetSize(width, heigh)
            board.Hide()
            return board

        @staticmethod
        def addEditLine(parent, x, y, width, heigh, text, charLimit):
            slotBar = ui.SlotBar()
            if parent is not None: slotBar.SetParent(parent)
            slotBar.SetSize(width, heigh)
            slotBar.SetPosition(x, y)
            slotBar.Show()
            editLine = ui.EditLine()
            editLine.SetParent(slotBar)
            editLine.SetSize(width, heigh)
            editLine.SetPosition(6, 2)
            editLine.SetMax(charLimit)
            editLine.SetNumberMode()
            editLine.SetText(text)
            editLine.Show()
            return slotBar, editLine


        @staticmethod
        def addToggleButton(parent, label, x, y, size, eventDown, eventUp=None, delay=None, closure=False):
            button = ui.ToggleButton()
            if parent is not None: button.SetParent(parent)
            button.SetPosition(x, y)
            button.SetUpVisual('d:/ymir work/ui/public/' + size + '_button_01.sub')
            button.SetOverVisual('d:/ymir work/ui/public/' + size + '_button_02.sub')
            button.SetDownVisual('d:/ymir work/ui/public/' + size + '_button_03.sub')
            button.SetText(label)
            button.SetToggleDownEvent(eventDown if delay is None else lambda: eventHandler.add(delay, eventDown() if closure else eventDown, label, True))
            button.SetToggleUpEvent(eventUp if delay is None else lambda: eventHandler.remove(label, eventUp))
            button.Show()
            return button

    class Api:
        targets = []
      
        
        @classmethod
        def getTargets(cls, Type):
            r = [0]  # Keeping track of the target search range
    
            def getTargetVIDs():
                # Check if the targets need to be refreshed
                if not cls.targets or (Type == 0 and not any(player.GetCharacterDistance(t) < 4000 for t in cls.targets[:5])) or all(player.GetCharacterDistance(t) == -1 for t in cls.targets[:5]):
                    del cls.targets[:]  # Clear previous targets
                    
                    # Ensure that r[0] stays within a manageable range
                    if r[0] > 10000000:
                        r[0] = 0
    
                    # Generate new targets
                    cls.targets.extend([
                        vid for vid in xrange(r[0], r[0] + 1000000, 3 if Type == 0 else 1)
                        if chr.GetInstanceType(vid) == Type and (Type != 0 or player.GetCharacterDistance(vid) < 6000)
                    ])
                    cls.targets.sort(key=lambda t: player.GetCharacterDistance(t))
                    r[0] += 1000000  # Increment the search range
    
                # Reset the search range if it exceeds the limit
                if r[0] > 100000000:
                    r[0] = 0
    
                return cls.targets or []  # Ensure it returns a list
            
            return getTargetVIDs


        @staticmethod
        def getDegree(vid):
            mobX, mobY, _ = chr.GetPixelPosition(vid)
            playerX, playerY, _ = player.GetMainCharacterPosition()
            try:
                rada = 180 * (math.acos((mobY - playerY) / math.sqrt((mobX - playerX)**2 + (mobY - playerY)**2))) / math.pi + 180
                if playerX >= mobX:
                    rada = 360 - rada
            except:
                rada = 0
            return rada
        ### Char move to position and dodge if get stucked ###
        @classmethod
        def moveToTarget(cls, vid):
            x, y, _ = chr.GetPixelPosition(vid)
            mX, mY, _ = player.GetMainCharacterPosition()
            
            # Adjust target coordinates slightly to avoid obstacles
            x = x + 135 if x < mX else x - 135
            y = y + 135 if y < mY else y - 135
    
            def moveToDestPosition(x, y):
                chr.MoveToDestPosition(player.GetMainCharacterIndex(), x, y)
                eventHandler.add(2, lambda: eventHandler.remove("moveToTarget"))
    
            # Add a timer for movement duration
            start_time = time.time()
            eventHandler.add(0, lambda x=x, y=y: moveToDestPosition(x, y), "moveToTarget")

        
            # Add logic to reset targets if the player is stuck
        #def check_if_stuck(self, vid):
        #    #current_distance = player.GetCharacterDistance(vid)
        #    # Check if the player is too far from the target and cannot reach it (stuck behind obstacles)
        #    if not self.canReachTarget(vid):  # Using self for instance method
        #    #if current_distance > 4000 or not self.canReachTarget(vid):  # Using self for instance method
        #        print("Player stuck or target unreachable, resetting.")
        #        del self.targets[:]  # Clear the current targets list
        #        self.getTargets(0)  # Reattempt to find new targets
            def check_if_stuck():
                current_distance = player.GetCharacterDistance(vid)
                elapsed_time = time.time() - start_time

                if player.SetAttackKeyState is True:
                   return
                if current_distance > 250 and current_distance < 380 and player.SetAttackKeyState is False:
                   cls.dodge(vid)
                   del cls.targets[:]

                # Check if the player is too far from the target or if the move is taking too long
                if current_distance > 500 or elapsed_time > 10:  # 30 seconds threshold
                   print("Player stuck or move taking too long, resetting.")
                   cls.dodge(vid)  # Trigger dodge behavior
                   del cls.targets[:]  # Clear the current targets list
                   cls.getTargets(0)  # Reattempt to find new targets
                
				      
            eventHandler.add(10, check_if_stuck)  # Check for stuck status after 5 seconds




#       @classmethod
#       def moveToTarget(cls, vid):
#           x, y, _ = chr.GetPixelPosition(vid)
#           mX, mY, _ = player.GetMainCharacterPosition()
#			
#           x = x + 135 if x < mX else x - 135
#           y = y + 135 if y < mY else y - 135
#
#           def moveToDestPosition(x, y):
#               chr.MoveToDestPosition(player.GetMainCharacterIndex(), x, y)
#               eventHandler.add(2, lambda: eventHandler.remove("moveToTarget"))
#               
#           if not eventHandler.has("moveToTarget"): eventHandler.add(0, lambda x=x, y=y: moveToDestPosition(x, y), "moveToTarget")
#
#           if not eventHandler.has("dodge"): eventHandler.add(5, lambda: cls.dodge(vid), "dodge")

        ### Char dodge into random direction ###

        @staticmethod
        def dodge(vid):
            if player.GetCharacterDistance(vid) < 400:
                return
    
            directions = [app.DIK_DOWN, app.DIK_RIGHT, app.DIK_LEFT, app.DIK_UP ]
            direction = directions[simple_random.randint(0, 3)]  # Fixed index range for four directions
    
            if player.IsMountingHorse():
                player.ClickSkillSlot(9)
    
            player.SetSingleDIKKeyState(direction, True)
    
            def stopDodge():
                player.SetSingleDIKKeyState(direction, False)
                eventHandler.remove("dodge")
    
            eventHandler.add(10, stopDodge)
    


   #     @staticmethod
   #     def dodge(vid):
   #         if player.GetCharacterDistance(vid) < 500: return
#
   #         directions = [ app.DIK_UP, app.DIK_DOWN, app.DIK_LEFT, app.DIK_RIGHT ]
   #         direction = directions[simple_random.randint(0, 8)]
#
   #         if player.IsMountingHorse(): player.ClickSkillSlot(9)
#
   #         player.SetSingleDIKKeyState(direction, True)
#
   #         def stopDodge():
   #             player.SetSingleDIKKeyState(direction, False)
   #             eventHandler.remove("dodge")
#
   #         eventHandler.add(2, stopDodge)

        ### Get VIDs of near targets ###


#        def getTargets(cls, Type):
#            targets = []
#            r = [0]
#
#            def getTargetVIDs():
#                if not targets or (Type == 0 and not any(player.GetCharacterDistance(t) < 5000 for t in targets[:5])) or all(player.GetCharacterDistance(t) == -1 for t in targets[:5]):
#                    del targets[:]
#                    targets.extend([ vid for vid in xrange(r[0], r[0]+1000000, 3 if Type == 0 else 1) if chr.GetInstanceType(vid) == Type and (Type != 0 or player.GetCharacterDistance(vid) < 10000) ])
#                    targets.sort(key=lambda t: player.GetCharacterDistance(t))
#                    r[0] += 1000000
#                    return targets
#                if targets or r[0] > 100000000:
#                    r[0] = 0
#                return targets
#            
#            return getTargetVIDs



class Bot:
    class Api:
        ### AutoAttack ###

        # Static variable to track the last wait time across multiple calls
        #last_wait_time = 0

        @staticmethod
        def autoAttack(Type):
            getTargets = Mt2.Api.getTargets(Type)

            def attack():
                # Access hp_threshold from the Attack class
                hp_threshold = Attack.hp_threshold
                current_hp_percentage = (float(player.GetStatus(player.HP)) / float(player.GetStatus(player.MAX_HP))) * 100

                # Check if HP is below the threshold
                if 0 < current_hp_percentage < hp_threshold:
                        chr.SetDirection(simple_random.randint(0, 8))
                        player.SetAttackKeyState(True)
                        player.PickCloseItem(True)

                        return  # Skip attacking while waiting
               
                # If HP is above the threshold, continue with normal attack logic
                targets = getTargets()
                target = next((t for t in targets if player.GetCharacterDistance(t) != -1), False)
                if not target: return
                Mt2.Api.moveToTarget(target)

                if 0 < player.GetCharacterDistance(target) < 300:

                    chr.SetRotation(Mt2.Api.getDegree(target))
                    player.SetAttackKeyState(True)
                    chr.testSetComboType(2)
                else:
                    player.SetAttackKeyState(False)

            return attack



#        @staticmethod
#        def autoAttack(Type):
#            getTargets = Mt2.Api.getTargets(Type)
#            def attack():
#                hp_threshold = Attack.hp_threshold
#                if 0 < (float(player.GetStatus(player.HP)) / (float(player.GetStatus(player.MAX_HP))) * 100) < hp_threshold:
#                    chr.SetDirection(simple_random.randint(0, 8))
#                    player.SetAttackKeyState(True)
#                    return				
#                targets = getTargets()
#                target = next((t for t in targets if player.GetCharacterDistance(t) != -1), False)
#                if not target: return
#                Mt2.Api.moveToTarget(target)
#                
#                if 0 < player.GetCharacterDistance(target) < 200:
#                    chr.SetRotation(Mt2.Api.getDegree(target))
#                    player.SetAttackKeyState(True)
#                else:
#                    player.SetAttackKeyState(False)
#            return attack

        ### Follow target ###
        @staticmethod
        def followTarget():
            targetVID = player.GetTargetVID()

            def follow():
                player.SetTarget(targetVID)
                if player.GetCharacterDistance(targetVID) > 1000:
                    Mt2.Api.moveToTarget(targetVID)

            return follow

    class Gui:
        def __init__(self):
            self.Board = ui.BoardWithTitleBar()
            self.mainBoard = Mt2.Gui.addThinBoard(None, 15, 100, 100, 240)

            self.autoAttackEditLine = Mt2.Gui.addEditLine(self.mainBoard, 0, 0, 0, 0, "0", 0)
            self.autoAttackBtn = Mt2.Gui.addToggleButton(self.Board, "Target Hunt", 25, 80, "large",
                lambda: Bot.Api.autoAttack(int(self.autoAttackEditLine[1].GetText())),
                lambda: player.SetAttackKeyState(False),
                1, True
            )
        def getAutoAttackComponents(self):
           	return self.autoAttackEditLine, self.autoAttackBtn


eventHandler = Mt2.EventHandler()
gui_instance = Bot.Gui()
gui = Bot.Gui()


Attack().Show()

#Bot.Gui.mainBoard.Show()