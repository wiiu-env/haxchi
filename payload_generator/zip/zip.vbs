Set fso = CreateObject("Scripting.FileSystemObject")
' tweak for the 3 args, info-zip style input that the makefile uses
If WScript.Arguments.Count = 3 then
    targetPath = fso.GetAbsolutePathName(WScript.Arguments.Item(2))
' otherwise 1st argument is source, second is zip
Else
    targetPath = fso.GetAbsolutePathName(WScript.Arguments.Item(0))
End If

ZipFile = fso.GetAbsolutePathName(WScript.Arguments.Item(1))

' Create empty ZIP file.
CreateObject("Scripting.FileSystemObject").CreateTextFile(ZipFile, True).Write "PK" & Chr(5) & Chr(6) & String(18, vbNullChar)

' get base name and make a temporary folder
tempDir = fso.GetParentFolderName(targetPath) & "\" & fso.GetBaseName(targetPath)
tempDir = tempDir & "_TEMP"

' create a temp folder and append random letters to its name
' if necessary
Randomize
Const LETTERS = "abcdefghijklmnopqrstuvwxyz"
While fso.FolderExists(tempDir & "\")
    tempDir = tempDir & Mid( LETTERS, Int(26*Rnd+1), 1 )
WEnd
fso.CreateFolder( tempDir)

' if source is a folder, create an additional container one
If fso.FolderExists(targetPath) Then
    containerFolder = tempDir & "\" & fso.GetBaseName(targetPath)
    fso.CreateFolder containerFolder
    fso.CopyFolder targetPath, containerFolder, True
Else
    fso.CopyFile targetPath, tempDir & "/", True
End If

Set objShell = CreateObject("Shell.Application")
' set the temp folder as the source to be compressed
set source = objShell.NameSpace(tempDir).Items

objShell.NameSpace(ZipFile).CopyHere(source)
' Keep script waiting until compression is done
Do Until objShell.NameSpace( ZipFile ).Items.Count = objShell.NameSpace( tempDir ).Items.Count
    WScript.Sleep 200
Loop
' Clean up
fso.DeleteFolder tempDir, true
