import os
import app
import localeInfo
from constInfo import TextColor

STATE_NONE = "Offline"
STATE_DICT = {
	0 : "OFFLINE",
	1 : "ONLINE",
	2 : "ONLINE",
	3 : "ONLINE"
}

SRV1 = {
	"name":"BestStudio",
	"host":"192.168.1.94",
	"auth":30001,
	"ch1":30003,
	"ch2":30007,
	"ch3":30011,
	"ch4":30015,
	"ch5":30019,
	"ch6":30061,
	"ch7":30071,
}

DEV_SRV1 = {
	"name":"ServerIsmiYazin",
	"host":"192.168.1.94",
	"auth":30001,
	"ch1":30003,
	"ch2":30007,
	"ch3":30011,
	"ch4":30015,
	"ch5":30019,
	"ch6":30061,
	"ch7":30071,
}

SRV_LIST = (
	(
		(
			SRV1["name"],SRV1["auth"],
			(SRV1["host"],SRV1["ch1"], "10.tga", "10"),
			[
				("CH1",SRV1["host"],SRV1["ch1"],SRV1["ch1"],0),
				("CH2",SRV1["host"],SRV1["ch2"],SRV1["ch2"],0),
				("CH3",SRV1["host"],SRV1["ch3"],SRV1["ch3"],0),
				("CH4",SRV1["host"],SRV1["ch4"],SRV1["ch4"],0),
				("CH5",SRV1["host"],SRV1["ch5"],SRV1["ch5"],0),
				("CH6",SRV1["host"],SRV1["ch6"],SRV1["ch6"],0),
				("CH7",SRV1["host"],SRV1["ch7"],SRV1["ch7"],0),
			]
		),
	),
)

DEV_SRV_LIST = (
	(
		(
			DEV_SRV1["name"],DEV_SRV1["auth"],
			(DEV_SRV1["host"],DEV_SRV1["ch1"], "10.tga", "10"),
			[
				("CH1",DEV_SRV1["host"],DEV_SRV1["ch1"],DEV_SRV1["ch1"],0),
				("CH2",DEV_SRV1["host"],DEV_SRV1["ch2"],DEV_SRV1["ch2"],0),
				("CH3",DEV_SRV1["host"],DEV_SRV1["ch3"],DEV_SRV1["ch3"],0),
				("CH4",DEV_SRV1["host"],DEV_SRV1["ch4"],DEV_SRV1["ch4"],0),
				("CH5",DEV_SRV1["host"],DEV_SRV1["ch5"],DEV_SRV1["ch5"],0),
				("CH6",DEV_SRV1["host"],DEV_SRV1["ch6"],DEV_SRV1["ch6"],0),
				("CH7",DEV_SRV1["host"],DEV_SRV1["ch7"],DEV_SRV1["ch7"],0),
			]
		),
	),
)

REGION_DICT = {
	0 : {
		0 : { 'name' : SRV1["name"], 'channel' : SRV_LIST, },
		2 : { 'name' : DEV_SRV1["name"], 'channel' : DEV_SRV_LIST, },
	},
}

SERVER_STATE_TABLE = {}

if app.__BL_MULTI_LANGUAGE__:
	def ReloadVariables():
		global STATE_NONE
		STATE_NONE = TextColor(localeInfo.CHANNEL_STATUS_OFFLINE, "FF0000")

		global STATE_DICT
		STATE_DICT = {
			0: TextColor(localeInfo.CHANNEL_STATUS_OFFLINE, "FF0000"),
			1: TextColor(localeInfo.CHANNEL_STATUS_RECOMMENDED, "00ff00"),
			2: TextColor(localeInfo.CHANNEL_STATUS_BUSY, "ffff00"),
			3: TextColor(localeInfo.CHANNEL_STATUS_FULL, "ff8a08")
		}