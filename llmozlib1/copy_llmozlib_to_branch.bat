@rem ===== destination dir =====
@set DEST_DIR="c:\branches\release"

@rem =============== instructions ===============
@echo This batch file copies files from a build of LLMozLib to a
@echo Second Life client source tree.
@echo.
@echo IMPORTANT: You will need a full Second Life tree and a
@echo a version of LLMozLib that has been built in release and 
@echo debug first.
@echo.
@echo The files will be copied to the:
@echo.
@echo   %DEST_DIR% 
@echo.
@echo directory. 
@echo.
@pause

@echo ========== copying header file ==========
xcopy llmozlib.h %DEST_DIR%\libraries\include\ /y

@echo ========== copying debug library ==========
xcopy debug\llmozlib.lib %DEST_DIR%\libraries\i686-win32\lib_debug\ /y

@echo ========== copying release library ==========
xcopy release\llmozlib.lib %DEST_DIR%\libraries\i686-win32\lib_release\ /y

@echo ========== copying debug DLLs and exe ==========
xcopy libraries\i686-win32\runtime_debug\*.dll %DEST_DIR%\libraries\i686-win32\lib_debug\ /y
xcopy libraries\i686-win32\runtime_debug\*.exe %DEST_DIR%\libraries\i686-win32\lib_debug\ /y

@echo ========== copying release DLLs ==========
xcopy libraries\i686-win32\runtime_release\*.dll %DEST_DIR%\libraries\i686-win32\lib_release\ /y

@echo ========== copying debug runtime files ==========
xcopy libraries\i686-win32\runtime_debug\chrome\*.*			%DEST_DIR%\indra\newview\app_settings\mozilla_debug\chrome\ /s /y
xcopy libraries\i686-win32\runtime_debug\components\*.*		%DEST_DIR%\indra\newview\app_settings\mozilla_debug\components\ /s /y
xcopy libraries\i686-win32\runtime_debug\greprefs\*.*		%DEST_DIR%\indra\newview\app_settings\mozilla_debug\greprefs\ /s /y
xcopy libraries\i686-win32\runtime_debug\plugins\*.*		%DEST_DIR%\indra\newview\app_settings\mozilla_debug\plugins\ /s /y
xcopy libraries\i686-win32\runtime_debug\res\*.*			%DEST_DIR%\indra\newview\app_settings\mozilla_debug\res\ /s /y

@echo ========== copying release runtime files ==========
xcopy libraries\i686-win32\runtime_release\chrome\*.*		%DEST_DIR%\indra\newview\app_settings\mozilla\chrome\ /s /y
xcopy libraries\i686-win32\runtime_release\components\*.*	%DEST_DIR%\indra\newview\app_settings\mozilla\components\ /s /y
xcopy libraries\i686-win32\runtime_release\greprefs\*.*		%DEST_DIR%\indra\newview\app_settings\mozilla\greprefs\ /s /y
xcopy libraries\i686-win32\runtime_release\plugins\*.*		%DEST_DIR%\indra\newview\app_settings\mozilla\plugins\ /s /y
xcopy libraries\i686-win32\runtime_release\res\*.*			%DEST_DIR%\indra\newview\app_settings\mozilla\res\ /s /y

@echo ========== copying debug chrome updates ==========
xcopy %DEST_DIR%\indra\newview\mozilla-theme\classic.jar	  %DEST_DIR%\indra\newview\app_settings\mozilla_debug\chrome\ /s /y
xcopy %DEST_DIR%\indra\newview\mozilla-theme\classic.manifest %DEST_DIR%\indra\newview\app_settings\mozilla_debug\chrome\ /s /y

@echo ========== copying release chrome updates ==========
xcopy %DEST_DIR%\indra\newview\mozilla-theme\classic.jar	  %DEST_DIR%\indra\newview\app_settings\mozilla\chrome\ /s /y
xcopy %DEST_DIR%\indra\newview\mozilla-theme\classic.manifest %DEST_DIR%\indra\newview\app_settings\mozilla\chrome\ /s /y

@echo ========== finished ==========
@pause