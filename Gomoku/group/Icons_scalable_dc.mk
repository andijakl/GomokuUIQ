# ============================================================================
#  Name     : Icons_scalable_dc.mk
#  Description : This is file for creating .mif file (scalable icons)
#  Version     : 
# ==============================================================================

ifeq (WINS,$(findstring WINS, $(PLATFORM)))
ZDIR=$(EPOCROOT)epoc32\release\$(PLATFORM)\$(CFG)\Z
else
ZDIR=$(EPOCROOT)epoc32\data\z
endif

TARGETDIR=$(ZDIR)\resource\apps
HEADERDIR=$(EPOCROOT)epoc32\include

ICONTARGETFILENAMES=$(TARGETDIR)\Gomoku.mif
ICONHEADERFILENAMES=$(HEADERDIR)\Gomoku.mbg

do_nothing :
	@rem do_nothing

MAKMAKE : do_nothing

BLD : do_nothing

CLEAN : 
	erase $(ICONHEADERFILENAMES) $(ICONTARGETFILENAMES) 2>>nul

LIB : do_nothing

CLEANLIB : do_nothing

RESOURCE :
	mifconv $(ICONTARGETFILENAMES)  \
		 /H$(ICONHEADERFILENAMES)  \
		 /c32,8 ..\gfx\iconGomoku.svg \
		 ..\gfx\iconHuman.svg  \
		 ..\gfx\iconAi.svg  \


FREEZE : do_nothing

SAVESPACE : do_nothing

RELEASABLES :
	@echo $(ICONTARGETFILENAMES) 

FINAL : do_nothing

