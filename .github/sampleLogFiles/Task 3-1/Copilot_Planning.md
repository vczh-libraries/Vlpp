# !!!PLANNING!!!

# Task 3-1: Implement Windows FileSystem and Injectable Integration

## Overview

This planning document details the implementation of Task 3-1, which completes the FileSystem injection pattern by implementing the Windows-specific file system operations and the injectable integration infrastructure. This task must complete both `FileSystem.Windows.cpp` and `FileSystem.Injectable.cpp` together to maintain build integrity.

## Improvement Plan

### Background Analysis

From analyzing the existing codebase and DateTime injection pattern in `..\..\Import\Vlpp.cpp`, I can see the established injection pattern consists of:

1. **Injectable Infrastructure** (in `.Injectable.cpp`):
   - Global variable: `IFileSystemImpl* injectedFileSystemImpl = nullptr;`
   - Injection function: `void InjectFileSystemImpl(IFileSystemImpl* impl)`
   - Getter function: `IFileSystemImpl* GetFileSystemImpl()` with fallback logic
   - All method redirections for platform-specific operations

2. **Platform Implementation** (in `.Windows.cpp`):
   - Implementation class: `WindowsFileSystemImpl` implementing `IFileSystemImpl`
   - OS implementation getter: `IFileSystemImpl* GetOSFileSystemImpl()`
   - All Win32 API logic moved into the implementation class

3. **Method Redirection** (both files):
   - Platform-specific methods updated to use `GetFileSystemImpl()`
   - Friend class declarations where needed for access to private methods

### Current State Analysis

#### FileSystem.h Interface Requirements
The interface `IFileSystemImpl` is already defined with these methods:
```cpp
virtual void Initialize(WString& fullPath) const = 0;
virtual bool IsFile(const WString& fullPath) const = 0;
virtual bool IsFolder(const WString& fullPath) const = 0;
virtual bool IsRoot(const WString& fullPath) const = 0;
virtual WString GetRelativePathFor(const WString& fromPath, const WString& toPath) const = 0;
virtual bool FileDelete(const FilePath& filePath) const = 0;
virtual bool FileRename(const FilePath& filePath, const WString& newName) const = 0;
virtual bool GetFolders(const FilePath& folderPath, collections::List<Folder>& folders) const = 0;
virtual bool GetFiles(const FilePath& folderPath, collections::List<File>& files) const = 0;
virtual bool CreateFolder(const FilePath& folderPath) const = 0;
virtual bool DeleteFolder(const FilePath& folderPath) const = 0;
virtual bool FolderRename(const FilePath& folderPath, const WString& newName) const = 0;
```

Also declared: `extern IFileSystemImpl* GetDefaultFileSystemImpl();` and `extern void InjectFileSystemImpl(IFileSystemImpl* impl);`

#### Current Windows Implementation Analysis
From `FileSystem.Windows.cpp`, the following platform-specific methods need migration:

**FilePath Operations:**
- `Initialize()` - Uses `GetCurrentDirectoryW()`, `GetFullPathNameW()`, path resolution logic
- `IsFile()` - Uses `GetFileAttributesEx()` with file attribute checking  
- `IsFolder()` - Uses `GetFileAttributesEx()` with `FILE_ATTRIBUTE_DIRECTORY`
- `IsRoot()` - Simple empty string check: `return fullPath == L"";`
- `GetRelativePathFor()` - Uses `PathRelativePathTo()` Win32 API

**File Operations:**
- `Delete()` - Uses `DeleteFileW()` Win32 API
- `Rename()` - Uses `MoveFileW()` Win32 API

**Folder Operations:**
- `GetFolders()` - Uses `FindFirstFileW()`, `FindNextFileW()`, `FindClose()` APIs with special logic for root drives
- `GetFiles()` - Uses `FindFirstFileW()`, `FindNextFileW()`, `FindClose()` APIs
- `CreateNonRecursively()` - Uses `CreateDirectoryW()` Win32 API
- `DeleteNonRecursively()` - Uses `RemoveDirectoryW()` Win32 API
- `Rename()` - Uses `MoveFileW()` Win32 API

### Detailed Implementation Plan

#### 1. Complete FileSystem.Injectable.cpp

**1.1 Injectable Infrastructure Implementation:**
```cpp
namespace vl::filesystem
{
    IFileSystemImpl* injectedFileSystemImpl = nullptr;

    void InjectFileSystemImpl(IFileSystemImpl* impl)
    {
        injectedFileSystemImpl = impl;
    }

    IFileSystemImpl* GetFileSystemImpl()
    {
        return injectedFileSystemImpl ? injectedFileSystemImpl : GetOSFileSystemImpl();
    }
}
```

This follows the exact pattern established by DateTime injection system. The getter function provides the core fallback logic that checks for injection first, then falls back to OS implementation.

**1.2 Method Redirection Implementation:**
All platform-specific methods in FilePath, File, and Folder classes need to be updated to delegate to `GetFileSystemImpl()`. This requires analyzing method signatures and updating them accordingly.

For FilePath methods:
- `void Initialize()` becomes call to `GetFileSystemImpl()->Initialize(fullPath)`
- `bool IsFile() const` becomes call to `GetFileSystemImpl()->IsFile(fullPath)`
- `bool IsFolder() const` becomes call to `GetFileSystemImpl()->IsFolder(fullPath)`
- `bool IsRoot() const` becomes call to `GetFileSystemImpl()->IsRoot(fullPath)`
- `WString GetRelativePathFor(const FilePath& _filePath) const` becomes call to `GetFileSystemImpl()->GetRelativePathFor(fullPath, _filePath.GetFullPath())`

For File methods:
- `bool Delete() const` becomes call to `GetFileSystemImpl()->FileDelete(filePath)`
- `bool Rename(const WString& newName) const` becomes call to `GetFileSystemImpl()->FileRename(filePath, newName)`

For Folder methods:
- `bool GetFolders(collections::List<Folder>& folders) const` becomes call to `GetFileSystemImpl()->GetFolders(filePath, folders)`
- `bool GetFiles(collections::List<File>& files) const` becomes call to `GetFileSystemImpl()->GetFiles(filePath, files)`
- `bool CreateNonRecursively() const` becomes call to `GetFileSystemImpl()->CreateFolder(filePath)`
- `bool DeleteNonRecursively() const` becomes call to `GetFileSystemImpl()->DeleteFolder(filePath)`
- `bool Rename(const WString& newName) const` becomes call to `GetFileSystemImpl()->FolderRename(filePath, newName)`

**Why this approach is necessary:** The redirection enables dependency injection while maintaining the same public API. All existing code continues to work, but the underlying implementation can be swapped for testing or alternative file system backends.

#### 2. Implement WindowsFileSystemImpl in FileSystem.Windows.cpp

**2.1 Class Declaration:**
```cpp
class WindowsFileSystemImpl : public Object, public virtual IFileSystemImpl
{
public:
    // All IFileSystemImpl methods implemented
    void Initialize(WString& fullPath) const override;
    bool IsFile(const WString& fullPath) const override;
    bool IsFolder(const WString& fullPath) const override;
    bool IsRoot(const WString& fullPath) const override;
    WString GetRelativePathFor(const WString& fromPath, const WString& toPath) const override;
    bool FileDelete(const FilePath& filePath) const override;
    bool FileRename(const FilePath& filePath, const WString& newName) const override;
    bool GetFolders(const FilePath& folderPath, collections::List<Folder>& folders) const override;
    bool GetFiles(const FilePath& folderPath, collections::List<File>& files) const override;
    bool CreateFolder(const FilePath& folderPath) const override;
    bool DeleteFolder(const FilePath& folderPath) const override;
    bool FolderRename(const FilePath& folderPath, const WString& newName) const override;
};
```

**2.2 Method Migration Strategy:**
Each method will be migrated with minimal logic changes:

- **FilePath::Initialize()**: Adapt to modify the passed `WString&` parameter instead of member variable. All existing logic with `GetCurrentDirectoryW()`, `GetFullPathNameW()`, path resolution, and `GetShortPathName()`/`GetLongPathName()` normalization preserved exactly.

- **FilePath::IsFile/IsFolder()**: Move existing `GetFileAttributesEx()` logic with identical attribute checking. The only change is operating on the passed `WString` parameter instead of member variable.

- **FilePath::IsRoot()**: Move simple `return fullPath == L"";` logic unchanged.

- **FilePath::GetRelativePathFor()**: Adapt existing `PathRelativePathTo()` logic to use two `WString` parameters instead of member variables and parameter.

- **File operations**: Extract `filePath.GetFullPath()` to work with existing `DeleteFileW()` and `MoveFileW()` logic. Path construction for rename operations preserved.

- **Folder operations**: Extract `filePath.GetFullPath()` and maintain all existing `FindFirstFileW()`/`FindNextFileW()` enumeration logic, drive detection for root, `CreateDirectoryW()`, `RemoveDirectoryW()`, and `MoveFileW()` operations.

**Why preserve exact logic:** The current Windows implementation is proven and handles many edge cases (UNC paths, drive roots, short/long path names, etc.). Changing this logic could introduce bugs or regressions.

**2.3 OS Implementation Getter:**
```cpp
WindowsFileSystemImpl osFileSystemImpl;

IFileSystemImpl* GetOSFileSystemImpl()
{
    return &osFileSystemImpl;
}
```

This follows the exact pattern used by `WindowsDateTimeImpl` - a global instance that gets returned by the OS getter function.

#### 3. Friend Class Requirements Analysis

**3.1 Access Pattern Analysis:**
From examining the code, the original implementation shows methods within FilePath, File, and Folder classes calling other methods in the same class. Some of these are static helper methods that may be private or protected.

**3.2 Friend Class Declarations Needed:**
The `WindowsFileSystemImpl` class will need to be added as a friend class to:
- `FilePath` class - to access any private static helpers used by platform-specific methods
- `File` class - to access the `filePath` member and any private helpers
- `Folder` class - to access the `filePath` member and any private helpers

This is necessary because the implementation class needs the same level of access that the original methods had.

**3.3 Cross-Platform Compatibility:**
Friend class declarations should be added for both Windows and Linux implementations to maintain consistency:
```cpp
class FilePath : public Object
{
    friend class WindowsFileSystemImpl;
    friend class LinuxFileSystemImpl;
    // ... existing code ...
};
```

**Why friend classes are needed:** The injection pattern moves platform-specific logic outside the original classes, but this logic still needs access to private members and helpers. Friend classes provide controlled access without breaking encapsulation.

#### 4. Header Dependencies and Build Integration

**4.1 Include Dependencies:**
- `FileSystem.Injectable.cpp` needs `#include "FileSystem.h"`
- `FileSystem.Windows.cpp` maintains existing includes for Win32 headers (`<Windows.h>`, `<Shlwapi.h>`)

**4.2 Compilation Order:**
Both files must be completed together because:
- `FileSystem.Injectable.cpp` provides `GetFileSystemImpl()` function
- `FileSystem.Windows.cpp` provides `GetOSFileSystemImpl()` function
- Platform-specific methods in all files call `GetFileSystemImpl()`

Without both files complete, linking will fail due to missing symbols.

### Implementation Sequence

1. **First**: Add friend class declarations to FilePath, File, and Folder classes in `FileSystem.h`
2. **Second**: Implement injectable infrastructure in `FileSystem.Injectable.cpp`
3. **Third**: Implement `WindowsFileSystemImpl` class in `FileSystem.Windows.cpp`
4. **Fourth**: Update all platform-specific method implementations to use injection pattern
5. **Fifth**: Add `GetOSFileSystemImpl()` function in `FileSystem.Windows.cpp`
6. **Finally**: Verify compilation and test

**Why this sequence:** Friend declarations must come first to avoid compilation errors. Injectable infrastructure must be in place before method redirections. Implementation class must exist before OS getter function can reference it.

## Test Plan

### 1. Compilation Verification

**1.1 Build System Integration:**
- Verify that both `FileSystem.Injectable.cpp` and `FileSystem.Windows.cpp` compile successfully together
- Ensure no linking errors occur due to missing symbols
- Confirm all friend class declarations resolve correctly

**1.2 Header Dependency Check:**
- Verify all necessary Win32 headers are included
- Check that `IFileSystemImpl` interface is properly accessible
- Ensure no circular dependency issues

### 2. Behavioral Preservation Testing

**2.1 Existing Unit Test Compatibility:**
The design document specifies that existing unit tests should continue to pass without modification. This validates that:
- All file system operations behave identically to current implementation
- Error handling and edge cases are preserved exactly
- Return value semantics remain unchanged
- Exception throwing behavior is maintained

**2.2 Functionality Regression Testing:**
Key areas to verify through existing unit tests:
- **Path Resolution**: Windows path normalization, UNC paths, drive root handling
- **File Operations**: Creation, deletion, renaming, existence checking
- **Folder Operations**: Enumeration, creation, deletion, recursive operations
- **Edge Cases**: Empty paths, invalid characters, long paths, permission issues

**2.3 Performance Impact Assessment:**
- Verify that the extra indirection through `GetFileSystemImpl()` does not cause measurable performance degradation
- Ensure that method calls are inlined appropriately in release builds

### 3. Injection Mechanism Validation

**3.1 Default Behavior Testing:**
- Verify that without injection, all operations use Windows implementation
- Confirm that `GetFileSystemImpl()` returns `GetOSFileSystemImpl()` when no injection is active
- Test that `InjectFileSystemImpl(nullptr)` restores default behavior

**3.2 Injection Functionality Testing:**
- Create simple test implementation of `IFileSystemImpl`
- Verify that injection works correctly for all interface methods
- Test injection switching between different implementations
- Confirm no memory leaks occur during injection changes

**3.3 Interface Completeness Testing:**
- Verify all platform-specific methods are properly redirected through interface
- Ensure no platform-specific methods bypass the injection system
- Test that interface covers all necessary operations

### 4. Integration Testing

**4.1 Cross-Component Testing:**
- Test file system operations across FilePath, File, and Folder classes
- Verify that complex operations involving multiple classes work correctly
- Ensure that operations like recursive folder deletion work through injection

**4.2 Error Condition Testing:**
- Test injection with implementations that throw exceptions
- Verify error handling when implementations return failure codes
- Ensure graceful degradation when injection fails

### 5. Manual Testing Scenarios

**5.1 Basic Functionality Verification:**
- Create, read, write, and delete files
- Create and remove directories
- Enumerate directory contents
- Rename files and folders

**5.2 Platform-Specific Features:**
- Test Windows drive enumeration in root folder
- Verify UNC path handling
- Test Windows path delimiter handling
- Verify case sensitivity behavior on Windows

**5.3 Injection Testing:**
- Temporarily inject custom implementation during manual testing
- Verify that injected implementation receives correct parameters
- Test switching between implementations during runtime

### 6. Edge Case Testing

**6.1 Path Edge Cases:**
- Empty paths, root paths, very long paths
- Paths with special characters, Unicode characters
- Relative vs absolute path handling

**6.2 Permission and Access Testing:**
- Operations on read-only files and folders
- Access denied scenarios
- Network path operations

**6.3 Resource Management:**
- File handle management in enumeration operations
- Memory allocation during path operations
- Win32 API resource cleanup

### Test Success Criteria

1. **Zero Compilation Errors**: Both files compile successfully together
2. **All Existing Unit Tests Pass**: No regression in existing functionality
3. **Injection Mechanism Works**: Custom implementations can be successfully injected
4. **Default Behavior Preserved**: Operations work identically without injection
5. **No Memory Leaks**: Resource management remains sound with injection
6. **Performance Maintained**: No significant performance degradation
7. **API Compatibility**: All existing code continues to work unchanged

The test plan ensures that the injection system provides the intended testability benefits while maintaining 100% compatibility with existing code and preserving all current functionality and behavior.

# !!!FINISHED!!!
