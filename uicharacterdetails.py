
import ui
import player
import item
import net
import app
import localeInfo
import constInfo

from uiToolTip import ItemToolTip

IMG_DIR = "d:/ymir work/ui/game/characterdetails/"

bonus_list = [
    [localeInfo.BONUS_TABLE_TITLE_1,
        [
            [item.APPLY_ATTBONUS_MONSTER,       item.GetApplyPoint(item.APPLY_ATTBONUS_MONSTER)],
            [item.APPLY_ATTBONUS_STONE,         item.GetApplyPoint(item.APPLY_ATTBONUS_STONE)],
            [item.APPLY_ATTBONUS_BOSS,          item.GetApplyPoint(item.APPLY_ATTBONUS_BOSS)],
            [item.APPLY_ATTBONUS_UNDEAD,        item.GetApplyPoint(item.APPLY_ATTBONUS_UNDEAD)],
            [item.APPLY_ATTBONUS_DEVIL,         item.GetApplyPoint(item.APPLY_ATTBONUS_DEVIL)],
            [item.APPLY_ATTBONUS_ORC,           item.GetApplyPoint(item.APPLY_ATTBONUS_ORC)],
            [item.APPLY_ATTBONUS_ANIMAL,        item.GetApplyPoint(item.APPLY_ATTBONUS_ANIMAL)],
            [item.APPLY_ATTBONUS_MILGYO,        item.GetApplyPoint(item.APPLY_ATTBONUS_MILGYO)],
            [item.APPLY_STEAL_HP,               item.GetApplyPoint(item.APPLY_STEAL_HP)],
            [item.APPLY_HP_REGEN,               item.GetApplyPoint(item.APPLY_HP_REGEN)],
            [item.APPLY_EXP_DOUBLE_BONUS,       item.GetApplyPoint(item.APPLY_EXP_DOUBLE_BONUS)],
            [item.APPLY_GOLD_DOUBLE_BONUS,      item.GetApplyPoint(item.APPLY_GOLD_DOUBLE_BONUS)],
            [item.APPLY_ITEM_DROP_BONUS,        item.GetApplyPoint(item.APPLY_ITEM_DROP_BONUS)]
        ]
    ],
    [localeInfo.BONUS_TABLE_TITLE_2,
        [
            [item.APPLY_ATTBONUS_HUMAN,         item.GetApplyPoint(item.APPLY_ATTBONUS_HUMAN)],
            [item.APPLY_ATTBONUS_WARRIOR,       item.GetApplyPoint(item.APPLY_ATTBONUS_WARRIOR)],
            [item.APPLY_ATTBONUS_ASSASSIN,      item.GetApplyPoint(item.APPLY_ATTBONUS_ASSASSIN)],
            [item.APPLY_ATTBONUS_SURA,          item.GetApplyPoint(item.APPLY_ATTBONUS_SURA)],
            [item.APPLY_ATTBONUS_SHAMAN,        item.GetApplyPoint(item.APPLY_ATTBONUS_SHAMAN)],
            [item.APPLY_RESIST_WARRIOR,         item.GetApplyPoint(item.APPLY_RESIST_WARRIOR)],
            [item.APPLY_RESIST_ASSASSIN,        item.GetApplyPoint(item.APPLY_RESIST_ASSASSIN)],
            [item.APPLY_RESIST_SURA,            item.GetApplyPoint(item.APPLY_RESIST_SURA)],
            [item.APPLY_RESIST_SHAMAN,          item.GetApplyPoint(item.APPLY_RESIST_SHAMAN)],
            [item.APPLY_RESIST_HUMAN,           item.GetApplyPoint(item.APPLY_RESIST_HUMAN)],
            [item.APPLY_RESIST_SWORD,           item.GetApplyPoint(item.APPLY_RESIST_SWORD)],
            [item.APPLY_RESIST_TWOHAND,         item.GetApplyPoint(item.APPLY_RESIST_TWOHAND)],
            [item.APPLY_RESIST_DAGGER,          item.GetApplyPoint(item.APPLY_RESIST_DAGGER)],
            [item.APPLY_RESIST_BELL,            item.GetApplyPoint(item.APPLY_RESIST_BELL)],
            # [item.APPLY_RESIST_BELL,             item.GetApplyPoint(item.APPLY_RESIST_FAN)],
            [item.APPLY_RESIST_BOW,             item.GetApplyPoint(item.APPLY_RESIST_BOW)],
            [item.APPLY_RESIST_MAGIC,           item.GetApplyPoint(item.APPLY_RESIST_MAGIC)]
        ]
    ],
    [localeInfo.BONUS_TABLE_TITLE_3,
        [
            [item.APPLY_SKILL_DAMAGE_BONUS,     item.GetApplyPoint(item.APPLY_SKILL_DAMAGE_BONUS)],
            [item.APPLY_NORMAL_HIT_DAMAGE_BONUS,item.GetApplyPoint(item.APPLY_NORMAL_HIT_DAMAGE_BONUS)],
            [item.APPLY_CRITICAL_PCT,           item.GetApplyPoint(item.APPLY_CRITICAL_PCT)],
            [item.APPLY_PENETRATE_PCT,          item.GetApplyPoint(item.APPLY_PENETRATE_PCT)],
            [item.APPLY_POISON_PCT,             item.GetApplyPoint(item.APPLY_POISON_PCT)],
            [item.APPLY_SLOW_PCT,               item.GetApplyPoint(item.APPLY_SLOW_PCT)],
            [item.APPLY_STUN_PCT,               item.GetApplyPoint(item.APPLY_STUN_PCT)],
            [item.APPLY_CAST_SPEED,             item.GetApplyPoint(item.APPLY_CAST_SPEED)]
        ]
    ],
    [localeInfo.BONUS_TABLE_TITLE_4,
        [
            [item.APPLY_SKILL_DEFEND_BONUS,     item.GetApplyPoint(item.APPLY_SKILL_DEFEND_BONUS)],
            [item.APPLY_NORMAL_HIT_DEFEND_BONUS,item.GetApplyPoint(item.APPLY_NORMAL_HIT_DEFEND_BONUS)],
            [item.APPLY_ANTI_CRITICAL_PCT,      item.GetApplyPoint(item.APPLY_ANTI_CRITICAL_PCT)],
            [item.APPLY_ANTI_PENETRATE_PCT,     item.GetApplyPoint(item.APPLY_ANTI_PENETRATE_PCT)],
            [item.APPLY_BLOCK,                  item.GetApplyPoint(item.APPLY_BLOCK)],
            [item.APPLY_REFLECT_MELEE,          item.GetApplyPoint(item.APPLY_REFLECT_MELEE)],
            [item.APPLY_DODGE,                  item.GetApplyPoint(item.APPLY_DODGE)],
            [item.APPLY_POISON_REDUCE,          item.GetApplyPoint(item.APPLY_POISON_REDUCE)],
            [item.APPLY_RESIST_FIRE,            item.GetApplyPoint(item.APPLY_RESIST_FIRE)],
            [item.APPLY_RESIST_ELEC,            item.GetApplyPoint(item.APPLY_RESIST_ELEC)],
            [item.APPLY_RESIST_WIND,            item.GetApplyPoint(item.APPLY_RESIST_WIND)]
        ]
    ]
]

class CharacterDetailsUI(ui.ScriptWindow):
    def Destroy(self):
        self.ClearDictionary()
        self.isPacketLoaded = False

    def __init__(self):
        ui.ScriptWindow.__init__(self)
        self.Destroy()
        self.__LoadScript()

    def __del__(self):
        ui.ScriptWindow.__del__(self)

    def Show(self):
        ui.ScriptWindow.Show(self)
        self.SetTop()
        self.Refresh()

    def Close(self):
        self.Hide()

    def AdjustPosition(self, x, y):
        self.SetPosition(x + self.GetWidth(), y)

    def __ClickRadioButton(self, buttonList, buttonIndex):
        try:
            radioBtn = buttonList[buttonIndex]
        except IndexError:
            return

        for eachButton in buttonList:
            eachButton.SetUp()
        radioBtn.Down()

    def __LoadScript(self):
        try:
            pyScrLoader = ui.PythonScriptLoader()
            pyScrLoader.LoadScriptFile(self, "UIScript/CharacterDetailsWindow.py")

            self.GetChild("bonus_button").SAFE_SetEvent(self.SetCategory, 0)
        except:
            import exception
            exception.Abort("CharacterDetailsUI.__LoadScript")

        elementList = self.ElementDictionary

        bonusWindow = self.GetChild("bonus_window")

        searchBoard = SearchSlotBoard()
        searchBoard.SetParent(bonusWindow)
        searchBoard.SetPosition((bonusWindow.GetWidth()/2)-(129/2),11)
        searchBoard.SetSize(129,23)
        searchBoard.Show()
        elementList["searchBoard"] = searchBoard

        itemSearch = ui.EditLine()
        itemSearch.SetParent(searchBoard)
        itemSearch.SetMax(40)
        itemSearch.SetPosition(5,5)
        itemSearch.SetSize(searchBoard.GetWidth(), searchBoard.GetHeight())
        itemSearch.OnIMEUpdate = ui.__mem_func__(self.__OnValueUpdate)
        itemSearch.Show()
        elementList["itemSearch"] = itemSearch

        searchBtn = ui.Button()
        searchBtn.SetParent(searchBoard)
        searchBtn.SetUpVisual(IMG_DIR+"search_btn_0.tga")
        searchBtn.SetOverVisual(IMG_DIR+"search_btn_1.tga")
        searchBtn.SetDownVisual(IMG_DIR+"search_btn_2.tga")
        searchBtn.SetEvent(self.__OnValueUpdate)
        searchBtn.SetPosition(itemSearch.GetWidth()-searchBtn.GetWidth(),2)
        searchBtn.Show()
        elementList["searchBtn"] = searchBtn

        scrollBar = ScrollBarNew()
        scrollBar.SetParent(bonusWindow)
        scrollBar.SetPosition((bonusWindow.GetWidth()-scrollBar.GetWidth())-3,43)
        scrollBar.SetScrollBarSize((bonusWindow.GetHeight()-46)-5)
        scrollBar.SetScrollEvent(ui.__mem_func__(self.Refresh))
        scrollBar.Show()
        elementList["scrollBar"] = scrollBar

        for i in xrange(len(bonus_list)):
            bonus_data = bonus_list[i][1]

            category = ui.ExpandedImageBox()
            category.SetParent(bonusWindow)
            category.LoadImage(IMG_DIR+"category_item.tga")
            category.buttonStatus = 0
            category.SetEvent(ui.__mem_func__(self.SetBonusCategory),"mouse_click", i)
            category.Show()
            elementList["%d_category"%i] = category

            categoryText = ui.TextLine()
            categoryText.SetParent(category)
            categoryText.SetHorizontalAlignLeft()
            categoryText.SetPosition(8,3)
            categoryText.SetText("|Eemoji/plus|e "+bonus_list[i][0])
            categoryText.Show()
            elementList["%d_categoryText"%i] = categoryText

            for j in xrange(len(bonus_data)):
                bonusImage = ui.ExpandedImageBox()
                bonusImage.SetParent(bonusWindow)
                bonusImage.LoadImage(IMG_DIR+"bonus_item.tga")
                bonusImage.Show()
                elementList["%d_%d_image"%(i,j)] = bonusImage

                bonusText = ItemToolTip.AFFECT_DICT[bonus_data[j][0]](0)
                
                disabledStr = ["%","+"]
                newText = ""
                for x in bonusText:
                    if x in disabledStr:
                        continue
                    if x.isdigit():
                        continue
                    newText+=x
                bonusText=newText
                bonusImage.SAFE_SetStringEvent("mouse_over_in",self.OverInBonus, bonusText)
                bonusImage.SAFE_SetStringEvent("mouse_over_out",self.OverOutBonus)
                
                bonusName = ui.TextLine()
                bonusName.AddFlag("not_pick")
                bonusName.SetParent(bonusImage)
                bonusName.SetHorizontalAlignCenter()
                bonusName.SetPosition(72, 3)
                bonusName.SetFontName("Tahoma:13")
                bonusName.SetText(bonusText)

                newText = ""
                if bonusName.GetTextSize()[0] > 134:
                    for o in xrange(100):
                        if bonusName.GetTextSize()[0] > 134:
                            newText = bonusName.GetText()[:len(bonusName.GetText())-2]+"..."
                            bonusName.SetText(bonusName.GetText()[:len(bonusName.GetText())-2])
                        else:
                            break
                
                if newText != "":
                    bonusName.SetText(newText)

                bonusName.Show()
                elementList["%d_%d_name"%(i,j)] = bonusName

                bonusValue = ui.TextLine()
                bonusValue.AddFlag("not_pick")
                bonusValue.SetParent(bonusImage)
                bonusValue.SetHorizontalAlignCenter()
                bonusValue.SetPosition(170,3)
                bonusValue.SetFontName("Tahoma:13")
                bonusValue.SetText("0")
                bonusValue.Show()
                elementList["%d_%d_value"%(i,j)] = bonusValue

        self.SetCategory(0)
    
    def OverOutBonus(self):
        interface = constInfo.GetInterfaceInstance()
        if interface:
            if interface.tooltipItem:
                interface.tooltipItem.HideToolTip()

    def OverInBonus(self, bonusName):
        interface = constInfo.GetInterfaceInstance()
        if interface:
            if interface.tooltipItem:
                interface.tooltipItem.ClearToolTip()
                interface.tooltipItem.AppendTextLine(bonusName)
                interface.tooltipItem.ShowToolTip()

    def OnRunMouseWheel(self, nLen):
        if self.ElementDictionary.has_key("scrollBar"):
            scrollBar = self.ElementDictionary["scrollBar"]
            if scrollBar.IsShow():
                if nLen > 0:
                    scrollBar.OnUp()
                else:
                    scrollBar.OnDown()
                return True
        return False

    def __OnValueUpdate(self):
        elementList = self.ElementDictionary
        itemSearch = elementList["itemSearch"]
        ui.EditLine.OnIMEUpdate(itemSearch)
        bonusWindow = self.GetChild("bonus_window")
        searchText = itemSearch.GetText()
        board = elementList["searchBoard"]
        totalWidth = 129

        if len(searchText) > 0:
            self.ElementDictionary["scrollBar"].SetPos(0, True)
            totalWidth-=25
            windowHeight = bonusWindow.GetWidth()-15
            textSize = itemSearch.GetTextSize()[0]
            if textSize >= totalWidth:
                if textSize >= windowHeight:
                    totalWidth = windowHeight
                else:
                    totalWidth = textSize
            totalWidth+=25
            if totalWidth >= windowHeight:
                totalWidth = windowHeight

        board.SetSize(totalWidth,23)
        itemSearch.SetSize(totalWidth,23)
        board.SetPosition((bonusWindow.GetWidth()/2)-(board.GetWidth()/2),11)
        searchBtn = elementList["searchBtn"]
        searchBtn.SetPosition(itemSearch.GetWidth()-searchBtn.GetWidth(),2)
        self.Refresh()

    def SetBonusCategory(self, emptyArg, categoryIndex):
        category = self.ElementDictionary["%d_category"%categoryIndex]
        category.buttonStatus = not category.buttonStatus
        if category.buttonStatus == 0:
            self.ElementDictionary["%d_categoryText"%categoryIndex].SetText("|Eemoji/plus|e "+bonus_list[categoryIndex][0])
        else:
            self.ElementDictionary["%d_categoryText"%categoryIndex].SetText("|Eemoji/negative|e "+bonus_list[categoryIndex][0])

        self.Refresh()

    def Refresh(self):
        elementList = self.ElementDictionary
        (X_POS,Y_POS) = (3, 43)
        CATEGORY_Y_RANGE = 25
        CATEGORY_Y_FIRST_BONUS_RANGE = 29
        BONUS_X = 13

        bonusWindow = self.GetChild("bonus_window")
        scrollBar = self.GetChild("scrollBar")
        (basePos,windowHeight) = (0,bonusWindow.GetHeight()-46)

        searchText = elementList["itemSearch"].GetText().lower()

        maxHeight = 0
        if len(searchText) > 0:
            for i in xrange(len(bonus_list)):
                bonus_data = bonus_list[i][1]
                for j in xrange(len(bonus_data)):
                    text = elementList["%d_%d_name"%(i,j)].GetText().lower()
                    if text.find(searchText) != -1:
                        maxHeight += CATEGORY_Y_FIRST_BONUS_RANGE
        else:
            for i in xrange(len(bonus_list)):
                bonus_data = bonus_list[i][1]
                if elementList.has_key("%d_category"%i):
                    categoryBtn = elementList["%d_category"%i]
                    if categoryBtn.buttonStatus == 0:
                        maxHeight += CATEGORY_Y_RANGE
                    else:
                        maxHeight += CATEGORY_Y_FIRST_BONUS_RANGE
                        for j in xrange(len(bonus_data)):
                            maxHeight += CATEGORY_Y_FIRST_BONUS_RANGE

        if maxHeight > windowHeight:
            scrollLen = maxHeight-windowHeight
            basePos = int(scrollBar.GetPos()*scrollLen)
            stepSize = 1.0 / (scrollLen/100.0)
            scrollBar.SetScrollStep(stepSize)
            scrollBar.SetMiddleBarSize(float(windowHeight-5)/float(maxHeight))
            scrollBar.Show()
        else:
            scrollBar.Hide()

        textLines = []
        images = []
        _wy = bonusWindow.GetGlobalPosition()[1]+43

        if len(searchText) > 0:
            for i in xrange(len(bonus_list)):
                bonus_data = bonus_list[i][1]
                if elementList.has_key("%d_category"%i):
                    elementList["%d_category"%i].Hide()
                    for j in xrange(len(bonus_data)):
                        text = elementList["%d_%d_name"%(i,j)].GetText().lower()
                        if text.find(searchText) != -1:
                            elementList["%d_%d_image"%(i,j)].Show()
                            elementList["%d_%d_image"%(i,j)].SetPosition(BONUS_X, Y_POS - basePos, True)
                            elementList["%d_%d_value"%(i,j)].SetText(str(player.GetStatus(bonus_data[j][1])))
                            Y_POS += CATEGORY_Y_FIRST_BONUS_RANGE
                            images.append(elementList["%d_%d_image"%(i,j)])
                            textLines.append(elementList["%d_%d_name"%(i,j)])
                            textLines.append(elementList["%d_%d_value"%(i,j)])
                        else:
                            elementList["%d_%d_image"%(i,j)].Hide()
        else:
            for i in xrange(len(bonus_list)):
                bonus_data = bonus_list[i][1]
                if elementList.has_key("%d_category"%i):
                    categoryBtn = elementList["%d_category"%i]
                    categoryBtn.SetPosition(X_POS, Y_POS - basePos, True)
                    categoryBtn.Show()

                    images.append(categoryBtn)
                    textLines.append(elementList["%d_categoryText"%i])

                    if categoryBtn.buttonStatus == 0:
                        Y_POS += CATEGORY_Y_RANGE
                        for j in xrange(len(bonus_data)):
                            elementList["%d_%d_image"%(i,j)].Hide()
                    else:
                        Y_POS += CATEGORY_Y_FIRST_BONUS_RANGE
                        for j in xrange(len(bonus_data)):
                            elementList["%d_%d_image"%(i,j)].Show()
                            elementList["%d_%d_image"%(i,j)].SetPosition(BONUS_X, Y_POS - basePos, True)
                            elementList["%d_%d_value"%(i,j)].SetText(str(player.GetStatus(bonus_data[j][1])))
                            Y_POS += CATEGORY_Y_FIRST_BONUS_RANGE

                            images.append(elementList["%d_%d_image"%(i,j)])
                            textLines.append(elementList["%d_%d_name"%(i,j)])
                            textLines.append(elementList["%d_%d_value"%(i,j)])

        for childItem in textLines:
            (_x,_y) = childItem.GetGlobalPosition()
            if _y < _wy:
                if childItem.IsShow():
                    childItem.Hide()
            elif _y > (_wy+windowHeight-20):
                if childItem.IsShow():
                    childItem.Hide()
            else:
                if not childItem.IsShow():
                    childItem.Show()

        for childItem in images:
            childHeight = childItem.GetHeight()
            (_x,_y) = childItem.GetGlobalPosition()
            if _y < _wy:
                childItem.SetRenderingRect(0,ui.calculateRect(childHeight-abs(_y-_wy),childHeight),0,0)
            elif _y+childHeight > (_wy+windowHeight-4):
                calculate = (_wy+windowHeight-4) - (_y+childHeight)
                if calculate == 0:
                    return
                f = ui.calculateRect(childHeight-abs(calculate),childHeight)
                childItem.SetRenderingRect(0,0,0,f)
            else:
                childItem.SetRenderingRect(0,0,0,0)

    def SetCategory(self, categoryIndex):
        self.__ClickRadioButton([self.GetChild("bonus_button")], categoryIndex)
        self.ElementDictionary["itemSearch"].KillFocus()
        self.GetChild("bonus_window").Show()
        self.Refresh()

    # def OnPressEscapeKey(self):
    #     # self.Close()
    #     return True

    def OnScroll(self):
        pass


class ScrollBarNew(ui.Window):
    SCROLLBAR_WIDTH = 7
    SCROLL_BTN_XDIST = 0
    SCROLL_BTN_YDIST = 0

    class MiddleBar(ui.DragButton):
        def __init__(self):
            ui.DragButton.__init__(self)
            self.AddFlag("movable")
            self.SetWindowName("scrollbar_middlebar")

        def MakeImage(self):
            top = ui.ExpandedImageBox()
            top.SetParent(self)
            top.LoadImage(IMG_DIR+"scrollbar/scrollbar_top.tga")
            top.AddFlag("not_pick")
            top.Show()
            topScale = ui.ExpandedImageBox()
            topScale.SetParent(self)
            topScale.SetPosition(0, top.GetHeight())
            topScale.LoadImage(IMG_DIR+"scrollbar/scrollbar_scale.tga")
            topScale.AddFlag("not_pick")
            topScale.Show()
            bottom = ui.ExpandedImageBox()
            bottom.SetParent(self)
            bottom.LoadImage(IMG_DIR+"scrollbar/scrollbar_bottom.tga")
            bottom.AddFlag("not_pick")
            bottom.Show()
            bottomScale = ui.ExpandedImageBox()
            bottomScale.SetParent(self)
            bottomScale.LoadImage(IMG_DIR+"scrollbar/scrollbar_scale.tga")
            bottomScale.AddFlag("not_pick")
            bottomScale.Show()
            middle = ui.ExpandedImageBox()
            middle.SetParent(self)
            middle.LoadImage(IMG_DIR+"scrollbar/scrollbar_mid.tga")
            middle.AddFlag("not_pick")
            middle.Show()
            self.top = top
            self.topScale = topScale
            self.bottom = bottom
            self.bottomScale = bottomScale
            self.middle = middle

        def SetSize(self, height):
            minHeight = self.top.GetHeight() + self.bottom.GetHeight() + self.middle.GetHeight()
            height = max(minHeight, height)
            ui.DragButton.SetSize(self, 10, height)
            scale = (height - minHeight) / 2 
            extraScale = 0
            if (height - minHeight) % 2 == 1:
                extraScale = 1
            self.topScale.SetRenderingRect(0, 0, 0, scale - 1)
            self.middle.SetPosition(0, self.top.GetHeight() + scale)
            self.bottomScale.SetPosition(0, self.middle.GetBottom())
            self.bottomScale.SetRenderingRect(0, 0, 0, scale - 1 + extraScale)
            self.bottom.SetPosition(0, height - self.bottom.GetHeight())

    def __init__(self):
        ui.Window.__init__(self)
        self.pageSize = 1
        self.curPos = 0.0
        self.eventScroll = None
        self.eventArgs = None
        self.lockFlag = False
        self.CreateScrollBar()
        self.SetScrollBarSize(0)
        self.scrollStep = 0.4
        self.SetWindowName("NONAME_ScrollBar")

    def __del__(self):
        ui.Window.__del__(self)

    def CreateScrollBar(self):
        topImage = ui.ExpandedImageBox()
        topImage.SetParent(self)
        topImage.AddFlag("not_pick")
        topImage.LoadImage(IMG_DIR+"scrollbar/scroll_top.tga")
        topImage.Show()
        bottomImage = ui.ExpandedImageBox()
        bottomImage.SetParent(self)
        bottomImage.AddFlag("not_pick")
        bottomImage.LoadImage(IMG_DIR+"scrollbar/scroll_bottom.tga")
        bottomImage.Show()
        middleImage = ui.ExpandedImageBox()
        middleImage.SetParent(self)
        middleImage.AddFlag("not_pick")
        middleImage.SetPosition(0, topImage.GetHeight())
        middleImage.LoadImage(IMG_DIR+"scrollbar/scroll_mid.tga")
        middleImage.Show()
        self.topImage = topImage
        self.bottomImage = bottomImage
        self.middleImage = middleImage
        middleBar = self.MiddleBar()
        middleBar.SetParent(self)
        middleBar.SetMoveEvent(ui.__mem_func__(self.OnMove))
        middleBar.Show()
        middleBar.MakeImage()
        middleBar.SetSize(12)
        self.middleBar = middleBar

    def Destroy(self):
        self.eventScroll = None
        self.eventArgs = None

    def SetScrollEvent(self, event, *args):
        self.eventScroll = event
        self.eventArgs = args

    def SetMiddleBarSize(self, pageScale):
        self.middleBar.SetSize(int(pageScale * float(self.GetHeight() - (self.SCROLL_BTN_YDIST*2))))
        realHeight = self.GetHeight() - (self.SCROLL_BTN_YDIST*2) - self.middleBar.GetHeight()
        self.pageSize = realHeight

    def SetScrollBarSize(self, height):
        self.SetSize(self.SCROLLBAR_WIDTH, height)
        self.pageSize = height - self.SCROLL_BTN_YDIST*2 - self.middleBar.GetHeight()
        middleImageScale = float((height - self.SCROLL_BTN_YDIST*2) - self.middleImage.GetHeight()) / float(self.middleImage.GetHeight())
        self.middleImage.SetRenderingRect(0, 0, 0, middleImageScale)
        self.bottomImage.SetPosition(0, height - self.bottomImage.GetHeight())
        self.middleBar.SetRestrictMovementArea(self.SCROLL_BTN_XDIST, self.SCROLL_BTN_YDIST, \
            self.middleBar.GetWidth(), height - self.SCROLL_BTN_YDIST * 2)
        self.middleBar.SetPosition(self.SCROLL_BTN_XDIST, self.SCROLL_BTN_YDIST)

    def SetScrollStep(self, step):
        self.scrollStep = step

    def OnUp(self):
        self.SetPos(self.curPos-self.scrollStep)

    def OnDown(self):
        self.SetPos(self.curPos+self.scrollStep)

    def GetScrollStep(self):
        return self.scrollStep

    def GetPos(self):
        return self.curPos

    def OnUp(self):
        self.SetPos(self.curPos-self.scrollStep)

    def OnDown(self):
        self.SetPos(self.curPos+self.scrollStep)

    def SetPos(self, pos, moveEvent = True):
        pos = max(0.0, pos)
        pos = min(1.0, pos)
        newPos = float(self.pageSize) * pos
        self.middleBar.SetPosition(self.SCROLL_BTN_XDIST, int(newPos) + self.SCROLL_BTN_YDIST)
        if moveEvent == True:
            self.OnMove()

    def OnMove(self):
        if self.lockFlag:
            return
        if 0 == self.pageSize:
            return
        (xLocal, yLocal) = self.middleBar.GetLocalPosition()
        self.curPos = float(yLocal - self.SCROLL_BTN_YDIST) / float(self.pageSize)
        if self.eventScroll:
            apply(self.eventScroll, self.eventArgs)

    def OnMouseLeftButtonDown(self):
        (xMouseLocalPosition, yMouseLocalPosition) = self.GetMouseLocalPosition()
        newPos = float(yMouseLocalPosition) / float(self.GetHeight())
        self.SetPos(newPos)

    def LockScroll(self):
        self.lockFlag = True

    def UnlockScroll(self):
        self.lockFlag = False

class SearchSlotBoard(ui.Window):
    CORNER_WIDTH = 7
    CORNER_HEIGHT = 7
    LINE_WIDTH = 7
    LINE_HEIGHT = 7
    LT = 0
    LB = 1
    RT = 2
    RB = 3
    L = 0
    R = 1
    T = 2
    B = 3

    def __init__(self):
        ui.Window.__init__(self)
        self.MakeBoard()
        self.MakeBase()

    def MakeBoard(self):
        cornerPath = IMG_DIR+"board/corner_"
        linePath = IMG_DIR+"board/"
        CornerFileNames = [ cornerPath+dir+".tga" for dir in ("left_top", "left_bottom", "right_top", "right_bottom") ]
        LineFileNames = [ linePath+dir+".tga" for dir in ("left", "right", "top", "bottom") ]
        self.Corners = []
        for fileName in CornerFileNames:
            Corner = ui.ExpandedImageBox()
            Corner.AddFlag("not_pick")
            Corner.LoadImage(fileName)
            Corner.SetParent(self)
            Corner.SetPosition(0, 0)
            Corner.Show()
            self.Corners.append(Corner)
        self.Lines = []
        for fileName in LineFileNames:
            Line = ui.ExpandedImageBox()
            Line.AddFlag("not_pick")
            Line.LoadImage(fileName)
            Line.SetParent(self)
            Line.SetPosition(0, 0)
            Line.Show()
            self.Lines.append(Line)
        self.Lines[self.L].SetPosition(0, self.CORNER_HEIGHT)
        self.Lines[self.T].SetPosition(self.CORNER_WIDTH, 0)

    def MakeBase(self):
        self.Base = ui.ExpandedImageBox()
        self.Base.AddFlag("not_pick")
        self.Base.LoadImage(IMG_DIR+"board/base.tga")
        self.Base.SetParent(self)
        self.Base.SetPosition(self.CORNER_WIDTH, self.CORNER_HEIGHT)
        self.Base.Show()

    def __del__(self):
        ui.Window.__del__(self)

    def Destroy(self):
        self.Base=0
        self.Corners=0
        self.Lines=0
        self.CORNER_WIDTH = 0
        self.CORNER_HEIGHT = 0
        self.LINE_WIDTH = 0
        self.LINE_HEIGHT = 0
        self.LT = 0
        self.LB = 0
        self.RT = 0
        self.RB = 0
        self.L = 0
        self.R = 0
        self.T = 0
        self.B = 0

    def SetSize(self, width, height):
        width = max(self.CORNER_WIDTH*2, width)
        height = max(self.CORNER_HEIGHT*2, height)
        ui.Window.SetSize(self, width, height)
        self.Corners[self.LB].SetPosition(0, height - self.CORNER_HEIGHT)
        self.Corners[self.RT].SetPosition(width - self.CORNER_WIDTH, 0)
        self.Corners[self.RB].SetPosition(width - self.CORNER_WIDTH, height - self.CORNER_HEIGHT)
        self.Lines[self.R].SetPosition(width - self.CORNER_WIDTH, self.CORNER_HEIGHT)
        self.Lines[self.B].SetPosition(self.CORNER_HEIGHT, height - self.CORNER_HEIGHT)
        verticalShowingPercentage = float((height - self.CORNER_HEIGHT*2) - self.LINE_HEIGHT) / self.LINE_HEIGHT
        horizontalShowingPercentage = float((width - self.CORNER_WIDTH*2) - self.LINE_WIDTH) / self.LINE_WIDTH
        self.Lines[self.L].SetRenderingRect(0, 0, 0, verticalShowingPercentage)
        self.Lines[self.R].SetRenderingRect(0, 0, 0, verticalShowingPercentage)
        self.Lines[self.T].SetRenderingRect(0, 0, horizontalShowingPercentage, 0)
        self.Lines[self.B].SetRenderingRect(0, 0, horizontalShowingPercentage, 0)
        if self.Base:
            self.Base.SetRenderingRect(0, 0, horizontalShowingPercentage, verticalShowingPercentage)
