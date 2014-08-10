IDIR =../src
CC=gcc

CFLAGS=-I$(IDIR) -I. -O2 `sdl2-config --cflags` `freetype-config --cflags`

CPP=g++
CPPFLAGS=-I$(IDIR) -I. -O2 -Wno-write-strings -Wno-conversion-null `freetype-config --cflags` `sdl2-config --cflags` -I/usr/local/include/bass/ 

SRCDIR=../src

ODIR=obj

LIBS=`sdl2-config --libs` `freetype-config --libs` -lbass -lfreeimage -L. -lSDL2

_OBJC = ConvertUTF.o DirectoryOperations.o FileOperations.o
OBJC = $(patsubst %,$(ODIR)/%,$(_OBJC))

_OBJCPP =  AdActor.o AdActorDir.o AdEntity.o AdGame.o AdInventoryBox.o AdInventory.o AdItem.o AdLayer.o AdNodeState.o AdObject.o AdPath.o AdPathPoint.o AdRegion.o AdResponseBox.o AdResponseContext.o AdResponse.o AdRotLevel.o AdScaleLevel.o AdScene.o AdSceneNode.o AdSceneState.o AdSentence.o AdSpriteSet.o AdTalkDef.o AdTalkHolder.o AdTalkNode.o AdWaypointGroup.o BActiveRect.o BBase.o BDebugger.o BDiskFile.o BDynBuffer.o BEvent.o BFader.o BFile.o BFileEntry.o BFileManager.o BFontBitmap.o BFont.o BFontStorage.o BFontTT.o BFrame.o BGame.o BImage.o BKeyboardState.o BNamedObject.o BObject.o BPackage.o BParser.o BPersistMgr.o BPkgFile.o BPoint.o BQuickMsg.o BRegion.o BRegistry.o BRenderer.o BRenderSDL.o BResourceFile.o BResources.o BSaveThumbFile.o BSaveThumbHelper.o BScriptable.o BScriptHolder.o BSoundBuffer.o BSound.o BSoundMgr.o BSprite.o BStringTable.o BSubFrame.o BSurface.o BSurfaceSDL.o BSurfaceStorage.o BTransitionMgr.o BViewport.o crc.o FontGlyphCache.o main.o MathUtil.o Matrix4.o PartEmitter.o PartForce.o PartParticle.o PathUtil.o PlatformSDL.o ScEngine.o ScScript.o ScStack.o ScValue.o SdlUtil.o StringUtil.o SXArray.o SXDate.o SXFile.o SXMath.o SXMemBuffer.o SxObject.o SXStore.o SXString.o SysClass.o SysClassRegistry.o SysInstance.o tinyxml.o tinyxmlerror.o tinyxmlparser.o UIButton.o UIEdit.o UIEntity.o UIObject.o UIText.o UITiledImage.o UIWindow.o utils.o Vector2.o 
# VidDataSource.o VidManager.o VidTheoraPlayer.o
OBJCPP = $(patsubst %,$(ODIR)/%,$(_OBJCPP))

wmelite: $(OBJC) $(OBJCPP)
	g++ -o $@ $^ $(CFLAGS) $(LIBS)

$(ODIR)/ConvertUTF.o: $(SRCDIR)/ConvertUTF.c
	mkdir -p $(ODIR)
	$(CC) -c -o $@ $< $(CFLAGS)

$(ODIR)/DirectoryOperations.o: $(SRCDIR)/DirectoryOperations.c
	mkdir -p $(ODIR)
	$(CC) -c -o $@ $< $(CFLAGS)

$(ODIR)/FileOperations.o: $(SRCDIR)/FileOperations.c
	mkdir -p $(ODIR)
	$(CC) -c -o $@ $< $(CFLAGS)

$(ODIR)/%.o: $(SRCDIR)/%.cpp
	mkdir -p $(ODIR)
	$(CPP) -c -o $@ $< $(CPPFLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ wmelite

