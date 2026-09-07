import ConfigParser

FC_NOOVERWRITE = 0
FC_OVERWRITE = 1
def IniRead(sFileName, sSection, sKey, sValue = ""):
    
    config = ConfigParser.RawConfigParser()
    config.read(sFileName)
    
    if config.has_option(sSection, sKey):
        sValue = config.get(sSection, sKey)
    
    return sValue
def IniWrite(sFileName, sSection, sKey, sValue):
    
    config = ConfigParser.RawConfigParser()
    config.read(sFileName)
    
    if not config.has_section(sSection):
        config.add_section(sSection)
    
    config.set(sSection, sKey, str(sValue))
    
    with open(sFileName, 'w') as configfile:
        config.write(configfile)
    
def IniReadSection(sFileName, sSection):
    
    config = ConfigParser.RawConfigParser()
    config.read(sFileName)
    
    aEntries = []
    aEntries.append([len(config.items(sSection)), ""])
    
    for (sKey, sValue) in config.items(sSection):
        aEntries.append([sKey, sValue])
        
    return aEntries
    
def IniDelete(sFileName, sSection, sKey = ""):
    
    config = ConfigParser.RawConfigParser()
    config.read(sFileName)
    
    isSuccess = 0
    
    if config.has_section(sSection):
    
        if sKey:
            if config.has_option(sSection, sKey):
                config.remove_option(sSection, sKey)
                isSuccess = 1
        else:
            config.remove_section(sSection)
            isSuccess = 1
    
    if isSuccess:
        with open(sFileName, 'w') as configfile:
            config.write(configfile)
            
    return isSuccess
    
def IniRenameSection(sFileName, sSection, sNewSection, flag = FC_NOOVERWRITE):
    
    config = ConfigParser.RawConfigParser()
    config.read(sFileName)
    
    if not config.has_section(sSection):
        return 0
    
    if config.has_section(sNewSection) and flag == FC_NOOVERWRITE:
        return 0
    elif config.has_section(sNewSection) and flag == FC_OVERWRITE:
        config.remove_section(sNewSection)
    
    aItems = config.items(sSection)
    
    config.add_section(sNewSection)
    
    for item in aItems:
        config.set(sNewSection, item[0], item[1])
    
    config.remove_section(sSection)
    
    with open(sFileName, 'w') as configfile:
        config.write(configfile)
    
    return 1
    
def IniReadSectionNames(sFileName):
    
    config = ConfigParser.RawConfigParser()
    config.read(sFileName)
    
    aSectionNames = config.sections()
    
    return aSectionNames
