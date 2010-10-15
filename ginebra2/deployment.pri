include($$PWD/../flags.pri)

# localpages

    !contains(br_layout, maemolayout) {
    	localpages.sources =    ./chrome/localpages/*.htm* \
                            ./chrome/localpages/*.js \
                            ./chrome/localpages/*.css \
                            ./chrome/localpages/*.jpg \
                            ./chrome/localpages/*.png \
                            ./chrome/localpages/*.dat
    	localpages.path = ./localpages
    	DEPLOYMENT += localpages
	  }
	  
	  contains(br_layout, maemolayout) {
	  	localpages.sources =    ./chrome/localpagesmaemo/*.htm* \
                            ./chrome/localpagesmaemo/*.js \
                            ./chrome/localpagesmaemo/*.css \
                            ./chrome/localpagesmaemo/*.jpg \
                            ./chrome/localpagesmaemo/*.png
    	localpages.path = ./localpagesmaemo
    	DEPLOYMENT += localpages
	  }
# mifs
# FIXME : is this really needed
    mifs.sources += /resource/apps/NokiaBrowser.mif
    mifs.path = /resource/apps
#    DEPLOYMENT += mifs
             	  
# backup restore file 
    backuprestore.sources = ./data/backup_registration.xml 
    backuprestore.path = ./
    DEPLOYMENT += backuprestore 
    
# Gesture lib
    qstmgesturelib.sources = qstmgesturelib.dll
    qstmgesturelib.path = /sys/bin
    DEPLOYMENT += qstmgesturelib

# bedrockprovisioning
    bedrockprovisioning.sources = BedrockProvisioning.dll
    bedrockprovisioning.path = /sys/bin
    DEPLOYMENT += bedrockprovisioning
    provisioningtemplate.sources = ../bedrockProvisioning/template/200267EA.ini
    provisioningtemplate.path = /data/.config/Nokia
    DEPLOYMENT += provisioningtemplate

# browsercore
    browsercore.sources = BrowserCore.dll
    browsecore.path = /sys/bin
    DEPLOYMENT += browsercore

contains(browser_addon, no) {
# browserrfsplugin
    browserrfsplugindll.sources = browserrfsplugin.dll
    browserrfsplugindll.path = /sys/bin
    browserrfs.sources = /resource/browserrfs.txt 
    browserrfs.path = /resource
    browserrfsplugin.sources = browserrfsplugin.rsc
    browserrfsplugin.path = /resource/plugins
#    DEPLOYMENT += browserrfsplugindll browserrfsplugin browserrfs

# hsbookmarkwidgetplugin    
    plugins.path = /private/20022F35/import/widgetregistry/200267BE
    plugins.sources = hsbookmarkwidgetplugin.dll 
    DEPLOYMENT += plugins
    widgetResources.path    = /private/20022F35/import/widgetregistry/200267BE
    widgetResources.sources += ../homescreen/widgetplugins/hsbookmarkwidgetplugin/resource/hsbookmarkwidgetplugin.xml    
    widgetResources.sources += ../homescreen/widgetplugins/hsbookmarkwidgetplugin/resource/hsbookmarkwidgetplugin.manifest
    widgetResources.sources += ../homescreen/widgetplugins/hsbookmarkwidgetplugin/resource/nokiabrowser-bm-icon_70x70_ico.png
    widgetResources.sources += ../homescreen/widgetplugins/hsbookmarkwidgetplugin/resource/nokiabrowser-bm-icon_70x70_no_ico.png
    widgetResources.sources += ../homescreen/widgetplugins/hsbookmarkwidgetplugin/resource/nokiabrowser-bm-icon_50x50.png
    DEPLOYMENT += widgetResources
}

contains(DEFINES, ENABLE_PERF_TRACE) {
    brperftrace.sources = brperftrace.dll
    brperftrace.path = /sys/bin
    DEPLOYMENT += brperftrace
}

# from mw/browser
    bookmarksapi.sources = bookmarksapi.dll
    bookmarksapi.path = /sys/bin
    DEPLOYMENT += bookmarksapi

# bookmarks
    bookmarks.sources += ../../../mw/browser/bookmarks/conf/bookmarks.xml"
    bookmarks.path = ./
    DEPLOYMENT += bookmarks

# Not used
#    BookMarksClientlibs.sources = BookMarksClient.dll
#    BookMarksClientlibs.path = /sys/bin
#    DEPLOYMENT += BookMarksClientlibs

# browsercontentdll    
    browsercontentdll.sources = browsercontentdll.dll
    browsercontentdll.path = /sys/bin
    DEPLOYMENT += browsercontentdll
# end from mw/browser 

contains(browser_addon, ninetwo) {
    brserviceipcclient.sources = BrServiceIPCClient.dll
    brserviceipcclient.path = /sys/bin
    DEPLOYMENT += brserviceipcclient

    brdownloadmgr.sources = BrDownloadMgr.dll
    brdownloadmgr.path = /sys/bin
    DEPLOYMENT += brdownloadmgr 
    
#    qtsystemInfo.sources = qtsystemInfo.dll
#    qtsystemInfo.path = /sys/bin
#    DEPLOYMENT += qtsystemInfo 
}
