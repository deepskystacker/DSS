# define source files in intended IDE groupings
   
set (
   SOURCE_FILES 
      
   stdafx.cpp
   
   AquireImage.cpp                  
   AquireImage.h
   Background.h                     
   BuildNo.h
   Color.cpp
   Color.h                          
   cursor.cpp                       
   cursor.h
   DecodeCharset.cpp                
   Defaults.cpp
   Defaults.h                       
   drawcontext.cpp
   drawcontext.h                    
   DrawContext.inl
   DrawTiledBitmap.cpp              
   EnableCoolTips.cpp
   FocusControl.h                   
   FontDef.h
   FontInfo.cpp                     
   FontInfo.h
   FormDisplayElementABC.h          
   GetColourFromString.cpp
   GetDisplayWorkArea.cpp           
   GetFontSize.cpp
   GetFontSizeAsPixels.cpp          
   GetNumberParameter.cpp
   GotoUrl.cpp                      
   Gradient.cpp
   Gradient.h                       
   GradientFill.cpp
   GradientFill.h                   
   HtmlAnchor.cpp
   HTMLAnchor.h                     
   htmlblockquote.cpp
   HTMLBlockQuote.h                 
   HTMLBreak.cpp
   HTMLBreak.h                      
   HTMLButtonSection.cpp
   HTMLButtonSection.h              
   HTMLComboSection.cpp
   HTMLComboSection.h               
   HTMLControl.cpp
   HTMLControl.h                    
   HTMLControlSection.cpp
   HTMLControlSection.h             
   HTMLDefinitionList.cpp
   HTMLDefinitionList.h             
   HTMLDefinitionListItem.cpp
   HTMLDefinitionListItem.h         
   htmldocument.cpp
   HTMLDocument.h                   
   HTMLDocumentObject.cpp
   HTMLDocumentObject.h             
   HTMLEditSection.cpp
   HTMLEditSection.h                
   HTMLFontDef.h
   HTMLForm.cpp                     
   HTMLForm.h
   HTMLFormField.h                  
   HTMLFormInput.cpp
   HTMLFormInput.h                  
   HTMLFormObjectABC.cpp
   HTMLFormObjectABC.h              
   HTMLGetAlignmentFromString.cpp
   HTMLHorizontalRule.cpp           
   HTMLHorizontalRule.h
   HTMLHorizontalRuleSection.cpp    
   HTMLHorizontalRuleSection.h
   HTMLImage.cpp                    
   HTMLImage.h
   HTMLImageSection.cpp             
   HTMLImageSection.h
   HTMLLink.h                       
   HtmlList.cpp
   HTMLList.h                       
   HTMLListboxSection.cpp
   HTMLListboxSection.h             
   HTMLListItem.cpp
   HTMLListItem.h                   
   HTMLOption.cpp
   HTMLOption.h                     
   HTMLParagraph.cpp
   HTMLParagraph.h                  
   HTMLParagraphObject.cpp
   HTMLParagraphObject.h            
   HTMLParse.cpp
   HTMLParse.h                      
   HTMLParseBase.cpp
   HTMLParseBase.h                  
   HTMLPrint.cpp
   HTMLSection.cpp                  
   HTMLSection.h
   HTMLSectionABC.cpp               
   HTMLSectionABC.h
   HTMLSectionCreator.cpp           
   HTMLSectionCreator.h
   HTMLSectionCreator2.cpp          
   HTMLSectionLink.cpp
   HtmlSectionLink.h                
   HTMLSelect.cpp
   HTMLSelect.h                     
   HTMLTable.cpp
   HTMLTable.h                      
   HTMLTableCell.cpp
   HTMLTableCell.h                  
   htmltablesection.cpp
   htmltablesection.h               
   HTMLTextArea.cpp
   HTMLTextArea.h                   
   HTMLTextBlock.cpp
   HTMLTextBlock.h                  
   HTMLTextSection.cpp
   HTMLTextSection.h                
   LoadTextFile.cpp                 
   MessageReflector.cpp
   MessageReflector.h               
   Mouse.cpp
   Palette.cpp
   Palette.h                        
   ParentSection.cpp
   ParentSection.h                  
   ParseStyles.cpp
   ParseStyles.h                    
   ParseWindowStyle.cpp
   qhtm.cpp                         
   QHTM.h                           
   QHTMControlSection.cpp
   QHTMControlSection.h             
   QHTMImage.cpp
   QHTMImage.h                      
   QHTM_GetHTMLHeight.cpp
   QHTM_Includes.h                  
   QHTM_Initialize.cpp
   QHTM_MessageBox.cpp              
   QHTM_RenderHTML.cpp
   QHTM_SetHTMLButton.cpp           
   QHTM_SetHTMLListbox.cpp
   QHTM_SetResources.cpp            
   QHTM_Trace.cpp
   QHTM_Trace.h                     
   QHTM_Types.h
   QHTM_Uninitialize.cpp            
   QHTM_WndProc.cpp
   RegisterWindow.cpp
   RemoveFilename.cpp               
   ResourceLoader.cpp
   ResourceLoader.h                 
   resstring.cpp
   resstring.h                      
   ScrollContainer.cpp
   ScrollContainer.h                
   sectionabc.cpp
   sectionabc.h                     
   SimpleString.cpp
   SimpleString.h                   
   SizeAttribute.cpp
   SizeAttribute.h                  
   smallstringhash.h
   StaticString.h                   
   striremove.cpp
   stristr.cpp                      
   strremove.cpp
   SubClassControl.cpp
   tablelayout.cpp                  
   tablelayout.h
   TextABC.h
   TipWindow.cpp                    
   TipWindow.h
   UnicodeHelpers.h
   Utils.h                          
)

set (
   HEADER_FILES 
   
   stdafx.h

   ../reuse/map.h
   ../reuse/map.inl
   ../reuse/mapiter.h
   ../reuse/mapiter.inl
   ../reuse/stack.h
   ../reuse/textabc.h
   ../reuse/winhelper.h
   ../reuse/windowtext.h
   
   config.h
   resource.h
   targetver.h
)

set (
   RESOURCE_FILES 

   res/Hand.cur
   res/hand1.cur
   res/noimage.bmp
   QHTM.rc
)

# define the Visual Studio IDE source file groupings
   
source_group ("Source Files"   FILES ${SOURCE_FILES})
source_group ("Header Files"   FILES ${HEADER_FILES})
source_group ("Resource Files" FILES ${RESOURCE_FILES})

# define the source files used for building the library target
# these will all appear in the IDE

add_library (
   qhtm 
   
   STATIC
   
   ${SOURCE_FILES}
   ${HEADER_FILES}
   ${RESOURCE_FILES}
)

# Organise the library in the IDE

set_target_properties (
   qhtm 

   PROPERTIES

   FOLDER "Libraries/QHTML Libs") 

# define the header file search paths

target_include_directories (
   qhtm 

   PRIVATE

   .
   ..
)

# define preprocessor variables

target_compile_definitions (
   qhtm 
   
   PRIVATE 
   
   WIN32
   NDEBUG
   _CRT_SECURE_NO_WARNINGS

   _AFXDLL
   _USRDLL
   _WINDOWS
   QHTM_LINK_STATIC_LIBS
   QHTM_BUILDLIB
   
##   _UNICODE
##   UNICODE
)

# Name the precompiled header and generator source files

set_target_precompiled_header (
   qhtm 
   
   stdafx.h 
   stdafx.cpp
)
