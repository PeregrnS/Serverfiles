import ui
import net
import localeInfo
import app

class RemoteShopDialog(ui.ScriptWindow):
    def __init__(self):
        ui.ScriptWindow.__init__(self)
        self.__LoadDialog()
        self.IsShow = False

    def __del__(self):
        ui.ScriptWindow.__del__(self)

    def __LoadDialog(self):
        try:
            pyScrLoader = ui.PythonScriptLoader()
            pyScrLoader.LoadScriptFile(self, "UIScript/RemoteShopDialog.py")
        except:
            import exception
            exception.Abort("RemoteShopDialog.__LoadDialog")

        REMOTE_FILE_NAME = "{}/remote_shop_names.txt".format(
            app.GetLocalePath())
        try:
            ShopData = new_open(REMOTE_FILE_NAME, "r").readlines()
        except IOError:
            import dbg
            dbg.LogBox("Can not read: ({})".format(REMOTE_FILE_NAME))
            app.Abort()

        cnt = len(ShopData)

        self.ParentBoard = self.GetChild("RemoteShopBoard")
        self.ChildBoard = self.GetChild("BlackBoard")
        self.GetChild("RemoteShopTitle").SetCloseEvent(
            ui.__mem_func__(self.Close))

        DlgWidht = 190
        BlackBoardHeight = 23*cnt + 5*(cnt-1) + 13
        DlgHeight = BlackBoardHeight + 75

        self.AcceptBtn = ui.MakeButton(self.ParentBoard, 13, DlgHeight - 33, "", "d:/ymir work/ui/public/",
                                       "middle_button_01.sub", "middle_button_02.sub", "middle_button_03.sub")
        self.AcceptBtn.SetText(localeInfo.UI_ACCEPT)
        self.AcceptBtn.SetEvent(ui.__mem_func__(self.AcceptButton))
        self.CloseBtn = ui.MakeButton(self.ParentBoard, DlgWidht - 73, DlgHeight - 33, "",
                                      "d:/ymir work/ui/public/", "middle_button_01.sub", "middle_button_02.sub", "middle_button_03.sub")
        self.CloseBtn.SetText(localeInfo.UI_CLOSE)
        self.CloseBtn.SetEvent(ui.__mem_func__(self.Close))

        self.ShopList = []
        for i in range(cnt):
            btn = ui.MakeButton(self.ChildBoard, 8, 6 + i*28, "", "d:/ymir work/ui/game/myshop_deco/",
                                "select_btn_01.sub", "select_btn_02.sub", "select_btn_03.sub")
            btn.SetText(ShopData[i])
            btn.SetEvent(ui.__mem_func__(self.__SelectShop), i)
            self.ShopList.append(btn)

        self.ParentBoard.SetSize(DlgWidht, DlgHeight)
        self.ChildBoard.SetSize(DlgWidht - 26, BlackBoardHeight)
        self.SetSize(DlgWidht, DlgHeight)

        self.UpdateRect()

    def __SelectShop(self, idx):
        if idx >= len(self.ShopList):
            return

        self.SelectedShopIndex = idx

        for btn in self.ShopList:
            btn.SetUp()
            btn.Enable()

        self.ShopList[idx].Down()
        self.ShopList[idx].Disable()

    def AcceptButton(self):
        net.SendRemoteShopPacket(self.SelectedShopIndex)

    def Show(self):
        if self.IsShowWindow():
            return
        ui.ScriptWindow.Show(self)
        self.__SelectShop(0)
        self.SetCenterPosition()
        self.SetTop()
        self.IsShow = True

    def Close(self):
        self.Hide()
        self.IsShow = False

    def OnPressEscapeKey(self):
        self.Close()
        return True

    def IsShowWindow(self):
        return self.IsShow
