# !!!TASK!!!

# PROBLEM DESCRIPTION

### Task 2-1: Implement Linux FileSystem with Injection Pattern

Rewrite `FileSystem.Linux.cpp` to extract platform-specific implementations into a class that implements `IFileSystemImpl`, and redirect the existing methods to use the injectable implementation.

**What to be done:**
- Create `LinuxFileSystemImpl` class implementing `IFileSystemImpl`
- Move platform-specific logic from standalone functions into the implementation class methods
- Add `GetOSFileSystemImpl()` function returning the Linux implementation
- Update existing FilePath, File, and Folder methods to use dependency injection pattern
- Maintain exact same functionality and behavior as the current implementation

**What to test in Unit Test:**
- No unit testing required as specified in the original request
- This file won't be consumed in the Windows project build

**What to test manually:**
- Code review to ensure platform-specific logic is correctly extracted
- Verify all Linux-specific API calls (stat, opendir, mkdir, etc.) are properly encapsulated
- Check that method signatures match the interface requirements

**Reasons for this task:**
- Enables dependency injection for Linux file system operations
- Maintains code organization with clear separation of platform-specific logic
- Follows the established pattern used by DateTime and Locale Linux implementations
- Required for completeness of the injection system

**Support evidence:**
- Existing Linux implementation provides all necessary functionality that needs to be wrapped
- `LinuxDateTimeImpl` in the codebase shows the exact pattern to follow
- Platform-specific methods are clearly identified from the current Linux implementation

# DESIGN DOCUMENT

## Overview

This design document outlines the implementation of the Linux file system injection pattern for task 2-1. The goal is to refactor the existing `FileSystem.Linux.cpp` to use the dependency injection pattern established by the `IFileSystemImpl` interface, following the same approach used by `DateTime` and `Locale` implementations.

## Analysis of Current Linux Implementation

From examining `..\..\Source\FileSystem.Linux.cpp`, the following platform-specific methods are currently implemented:

### FilePath Operations
- `Initialize()` - Uses `getcwd()`, handles Unix path normalization with "/" as delimiter
- `IsFile()` - Uses `stat()` with `S_ISREG()` to check for regular files
- `IsFolder()` - Uses `stat()` with `S_ISDIR()` to check for directories  
- `IsRoot()` - Checks if path equals "/"
- `GetRelativePathFor()` - Custom implementation for Unix path relatives

### File Operations
- `Delete()` - Uses `unlink()` system call
- `Rename()` - Uses `rename()` system call

### Folder Operations
- `GetFolders()` - Uses `opendir()`, `readdir()`, `closedir()` to enumerate subdirectories
- `GetFiles()` - Uses `opendir()`, `readdir()`, `closedir()` to enumerate files
- `CreateNonRecursively()` - Uses `mkdir()` with mode 0777
- `DeleteNonRecursively()` - Uses `rmdir()` system call
- `Rename()` - Uses `rename()` system call

## Design Analysis Based on DateTime Pattern

From examining the DateTime injection pattern in `..\..\Import\Vlpp.Linux.cpp`, the established pattern includes:

1. **Implementation Class**: `LinuxDateTimeImpl` implements `IDateTimeImpl`
2. **Global Instance**: A static instance `osDateTimeImpl` 
3. **Getter Function**: `GetOSDateTimeImpl()` returns pointer to the static instance
4. **Method Implementation**: All virtual methods from the interface are implemented

## Updated Interface Requirements

Based on the interface defined in task 1-1 (`IFileSystemImpl` in `..\..\Source\FileSystem.h`), the Linux implementation must provide:

```cpp
class IFileSystemImpl : public virtual Interface
{
public:
    // FilePath operations
    virtual void Initialize(WString& fullPath) const = 0;
    virtual bool IsFile(const WString& fullPath) const = 0;
    virtual bool IsFolder(const WString& fullPath) const = 0;
    virtual bool IsRoot(const WString& fullPath) const = 0;
    virtual WString GetRelativePathFor(const WString& fromPath, const WString& toPath, bool fromIsFolder, bool toIsFolder) const = 0;
    
    // File operations - Updated to use FilePath
    virtual bool FileDelete(const FilePath& filePath) const = 0;
    virtual bool FileRename(const FilePath& filePath, const WString& newName) const = 0;
    
    // Folder operations - Updated to use FilePath
    virtual bool GetFolders(const FilePath& folderPath, collections::List<Folder>& folders) const = 0;
    virtual bool GetFiles(const FilePath& folderPath, collections::List<File>& files) const = 0;
    virtual bool CreateFolder(const FilePath& folderPath) const = 0;
    virtual bool DeleteFolder(const FilePath& folderPath) const = 0;
    virtual bool FolderRename(const FilePath& folderPath, const WString& newName) const = 0;
};
```

## Implementation Strategy

### 1. Linux Implementation Class Design

Create `LinuxFileSystemImpl` class that implements all methods from `IFileSystemImpl`:

```cpp
class LinuxFileSystemImpl : public Object, public virtual IFileSystemImpl
{
public:
    // FilePath operations
    void Initialize(WString& fullPath) const override;
    bool IsFile(const WString& fullPath) const override;
    bool IsFolder(const WString& fullPath) const override;
    bool IsRoot(const WString& fullPath) const override;
    WString GetRelativePathFor(const WString& fromPath, const WString& toPath, bool fromIsFolder, bool toIsFolder) const override;
    
    // File operations
    bool FileDelete(const FilePath& filePath) const override;
    bool FileRename(const FilePath& filePath, const WString& newName) const override;
    
    // Folder operations
    bool GetFolders(const FilePath& folderPath, collections::List<Folder>& folders) const override;
    bool GetFiles(const FilePath& folderPath, collections::List<File>& files) const override;
    bool CreateFolder(const FilePath& folderPath) const override;
    bool DeleteFolder(const FilePath& folderPath) const override;
    bool FolderRename(const FilePath& folderPath, const WString& newName) const override;
};
```

### 2. Method Migration Strategy

Each method will be migrated from the current implementation with minimal changes:

#### FilePath Operations
- `Initialize()`: Move logic from current `FilePath::Initialize()`, adapt to take `WString&` parameter
- `IsFile()`, `IsFolder()`, `IsRoot()`: Move logic with minimal modification for parameter types
- `GetRelativePathFor()`: Adapt current logic to handle the new signature with boolean flags

#### File/Folder Operations  
- All file and folder operations will extract `FilePath::GetFullPath()` and apply existing logic
- Return types and logic remain identical to current implementation

### 3. Global Instance and Getter Function

Following the DateTime pattern:

```cpp
LinuxFileSystemImpl osFileSystemImpl;

IFileSystemImpl* GetOSFileSystemImpl()
{
    return &osFileSystemImpl;
}
```

### 4. Method Redirection Pattern

Current platform methods will be updated to delegate to the injectable implementation:

```cpp
void FilePath::Initialize()
{
    GetFileSystemImpl()->Initialize(fullPath);
}

bool FilePath::IsFile() const
{
    return GetFileSystemImpl()->IsFile(fullPath);
}

// Similar pattern for all other methods
```

## Parameter Adaptation Strategy

### FilePath Methods
- Current methods operate on member variable `fullPath`
- Interface methods take `WString` parameters
- Implementation will convert between these as needed

### File/Folder Methods
- Current methods operate on member variable `filePath` of type `FilePath`
- Interface methods take `FilePath` parameters directly
- Implementation will call `GetFullPath()` on the parameter to get string representation

## Error Handling and Compatibility

### Maintain Existing Behavior
- All Linux system call error handling remains identical
- Return values and error conditions preserved exactly
- No changes to public API contracts

### String Conversions
- Continue using `wtoa()` and `atow()` for Linux system call compatibility
- Maintain existing UTF-8/Unicode handling patterns

## Platform-Specific Considerations

### Unix Path Handling
- Maintain "/" as path delimiter in all operations
- Preserve existing root path detection ("/" comparison)
- Keep current path normalization logic for relative paths

### System Call Patterns
- All `stat()`, `opendir()`, `mkdir()`, etc. calls remain unchanged
- Error code handling patterns preserved
- File permissions and modes maintained

## Implementation Benefits

### Code Organization
- Clear separation between interface contracts and platform implementation
- Easier to maintain and test platform-specific code
- Consistent with established patterns in the codebase

### Testability
- Enables mock implementations for unit testing
- Allows behavior verification without file system dependencies
- Supports integration testing with different file system behaviors

### Future Extensibility
- Enables alternative implementations (e.g., memory-based, network-based)
- Supports different Unix-like platforms with minimal changes
- Allows runtime behavior modification through injection

## Migration Validation

### Functional Verification
- All existing unit tests should continue to pass
- File system operations should behave identically
- Error conditions and edge cases preserved

### Code Review Focus Areas
1. Parameter conversion correctness (WString ↔ FilePath)
2. System call parameter passing accuracy
3. Error code propagation integrity
4. Memory management in collection operations
5. Path delimiter handling consistency

## Conclusion

This design provides a clean migration path from the current Linux implementation to the injection pattern while maintaining 100% behavioral compatibility. The approach follows established patterns in the codebase and enables future testing and extensibility improvements.

# !!!FINISHED!!!
