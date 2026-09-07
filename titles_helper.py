import localeInfo

def GetSpecialTitle(itemValue):
	if itemValue == 0:
		return ""
	else:
		return localeInfo.GetTitleAchievement(itemValue)

def GetTitlesLength():
	length = 0
	for x in xrange(1, 100):
		try:
			GetSpecialTitle(x)
			length += 1
		except Exception:
			break

	return length
