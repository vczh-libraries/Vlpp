# !!!EXECUTION!!!

# Improvement Plan

## 1. Create LinuxFileSystemImpl Class in FileSystem.Linux.cpp

Add the following class declaration:

```cpp
class LinuxFileSystemImpl : public Object, public virtual IFileSystemImpl
{
public:
    // FilePath operations implementation
    void Initialize(WString& fullPath) const override;
    bool IsFile(const WString& fullPath) const override;
    bool IsFolder(const WString& fullPath) const override;
    bool IsRoot(const WString& fullPath) const override;
    WString GetRelativePathFor(const WString& fromPath, const WString& toPath, bool fromIsFolder, bool toIsFolder) const override;
    
    // File operations implementation
    bool FileDelete(const FilePath& filePath) const override;
    bool FileRename(const FilePath& filePath, const WString& newName) const override;
    
    // Folder operations implementation
    bool GetFolders(const FilePath& folderPath, collections::List<Folder>& folders) const override;
    bool GetFiles(const FilePath& folderPath, collections::List<File>& files) const override;
    bool CreateFolder(const FilePath& folderPath) const override;
    bool DeleteFolder(const FilePath& folderPath) const override;
    bool FolderRename(const FilePath& folderPath, const WString& newName) const override;
};
```

## 2. Implement LinuxFileSystemImpl::Initialize Method

Migrate from existing FilePath::Initialize() to:

```cpp
void LinuxFileSystemImpl::Initialize(WString& fullPath) const
{
    // Copy existing logic from FilePath::Initialize() but operate on fullPath parameter instead of this->fullPath
}
```

## 3. Implement LinuxFileSystemImpl::IsFile Method

Migrate from existing FilePath::IsFile() to:

```cpp
bool LinuxFileSystemImpl::IsFile(const WString& fullPath) const
{
    // Copy existing logic from FilePath::IsFile() but use fullPath parameter instead of this->fullPath
}
```

## 4. Implement LinuxFileSystemImpl::IsFolder Method

Migrate from existing FilePath::IsFolder() to:

```cpp
bool LinuxFileSystemImpl::IsFolder(const WString& fullPath) const
{
    // Copy existing logic from FilePath::IsFolder() but use fullPath parameter instead of this->fullPath
}
```

## 5. Implement LinuxFileSystemImpl::IsRoot Method

Migrate from existing FilePath::IsRoot() to:

```cpp
bool LinuxFileSystemImpl::IsRoot(const WString& fullPath) const
{
    return fullPath == L"/";
}
```

## 6. Implement LinuxFileSystemImpl::GetRelativePathFor Method

Migrate from existing FilePath::GetRelativePathFor() to:

```cpp
WString LinuxFileSystemImpl::GetRelativePathFor(const WString& fromPath, const WString& toPath, bool fromIsFolder, bool toIsFolder) const
{
    // Copy existing logic from FilePath::GetRelativePathFor() but:
    // - Replace this->fullPath with fromPath parameter
    // - Replace _filePath.fullPath with toPath parameter  
    // - Use fromIsFolder and toIsFolder flags instead of calling IsFolder() and GetFolder()
}
```

## 7. Implement LinuxFileSystemImpl::FileDelete Method

Migrate from existing File::Delete() to:

```cpp
bool LinuxFileSystemImpl::FileDelete(const FilePath& filePath) const
{
    // Copy existing logic from File::Delete() but use filePath.GetFullPath() instead of this->filePath.GetFullPath()
}
```

## 8. Implement LinuxFileSystemImpl::FileRename Method

Migrate from existing File::Rename() to:

```cpp
bool LinuxFileSystemImpl::FileRename(const FilePath& filePath, const WString& newName) const
{
    // Copy existing logic from File::Rename() but use filePath parameter instead of this->filePath
}
```

## 9. Implement LinuxFileSystemImpl::GetFolders Method

Migrate from existing Folder::GetFolders() to:

```cpp
bool LinuxFileSystemImpl::GetFolders(const FilePath& folderPath, collections::List<Folder>& folders) const
{
    // Copy existing logic from Folder::GetFolders() but:
    // - Use folderPath parameter instead of this->filePath
    // - Remove the Exists() check
    // - Keep identical opendir()/readdir()/closedir() logic
}
```

## 10. Implement LinuxFileSystemImpl::GetFiles Method

Migrate from existing Folder::GetFiles() to:

```cpp
bool LinuxFileSystemImpl::GetFiles(const FilePath& folderPath, collections::List<File>& files) const
{
    // Copy existing logic from Folder::GetFiles() but:
    // - Use folderPath parameter instead of this->filePath
    // - Remove the Exists() check
    // - Keep identical directory enumeration logic
}
```

## 11. Implement LinuxFileSystemImpl::CreateFolder Method

Migrate from existing Folder::CreateNonRecursively() to:

```cpp
bool LinuxFileSystemImpl::CreateFolder(const FilePath& folderPath) const
{
    // Copy existing logic from Folder::CreateNonRecursively() but use folderPath.GetFullPath() instead of this->filePath.GetFullPath()
}
```

## 12. Implement LinuxFileSystemImpl::DeleteFolder Method

Migrate from existing Folder::DeleteNonRecursively() to:

```cpp
bool LinuxFileSystemImpl::DeleteFolder(const FilePath& folderPath) const
{
    // Copy existing logic from Folder::DeleteNonRecursively() but use folderPath.GetFullPath() instead of this->filePath.GetFullPath()
}
```

## 13. Implement LinuxFileSystemImpl::FolderRename Method

Migrate from existing Folder::Rename() to:

```cpp
bool LinuxFileSystemImpl::FolderRename(const FilePath& folderPath, const WString& newName) const
{
    // Copy existing logic from Folder::Rename() but use folderPath parameter instead of this->filePath
}
```

## 14. Add Global Instance and Getter Function

Add at the end of FileSystem.Linux.cpp:

```cpp
LinuxFileSystemImpl osFileSystemImpl;

IFileSystemImpl* GetOSFileSystemImpl()
{
    return &osFileSystemImpl;
}
```

# Test Plan

## Compilation Verification
- Ensure LinuxFileSystemImpl class compiles without errors
- Verify all interface methods are correctly implemented with proper signatures
- Check that global instance and getter function are properly declared

# !!!FINISHED!!!
