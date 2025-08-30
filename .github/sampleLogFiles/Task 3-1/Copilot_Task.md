# !!!TASK!!!

# PROBLEM DESCRIPTION

### Task 3-1: Implement Windows FileSystem and Injectable Integration

Rewrite `FileSystem.Windows.cpp` to use the injection pattern and complete `FileSystem.Injectable.cpp` with the injection infrastructure. These must be completed together to maintain build integrity.

**What to be done:**
- Create `WindowsFileSystemImpl` class implementing `IFileSystemImpl` in Windows file
- Move Windows-specific logic (Win32 API calls) into the implementation class
- Add `GetOSFileSystemImpl()` function returning the Windows implementation
- Complete `FileSystem.Injectable.cpp` with:
  - Global variable for injected implementation
  - `InjectFileSystemImpl()` function
  - `GetFileSystemImpl()` function that checks injection first
  - All redirection implementations for platform-specific methods
- Update FilePath, File, and Folder methods to use `GetFileSystemImpl()`

**What to test in Unit Test:**
- Existing unit tests should continue to pass without modification
- Tests validate that the injection system maintains behavioral compatibility
- No additional tests needed as the functionality remains identical

**What to test manually:**
- Full project build verification
- Run existing unit tests to ensure no regressions
- Verify that default behavior (without injection) remains identical
- Test injection mechanism by temporarily injecting a test implementation

**Reasons for this task:**
- Completes the injection system for Windows platform
- Must be done together with Injectable.cpp to maintain build integrity
- Provides the core infrastructure for dependency injection testing
- Enables the same testability patterns as DateTime and Locale

**Support evidence:**
- Existing Windows implementation shows all Win32 API usage that needs wrapping
- `WindowsDateTimeImpl` provides the exact implementation pattern
- Current unit tests demonstrate expected behavior that must be preserved
- Injectable pattern is well-established in the codebase

## UPDATE

Just to mention that, in the original implementation, FilePath or File will call methods of itself. If it is a non-public static method, remember to add the friend class. If it is a injectable method, just call the one in the new class.

# DESIGN DOCUMENT

## Overview

This design document outlines the implementation of Task 3-1, which completes the FileSystem injection pattern by implementing the Windows-specific file system operations and the injectable integration infrastructure. This task builds upon the interface defined in Task 1-1 and must be completed together with `FileSystem.Injectable.cpp` to maintain build integrity.

## Analysis of Current Windows Implementation

From examining `..\..\Source\FileSystem.Windows.cpp`, the following platform-specific methods are currently implemented:

### FilePath Operations
- `Initialize()` - Uses `GetCurrentDirectoryW()`, `GetFullPathNameW()` for Windows path resolution
- `IsFile()` - Uses `GetFileAttributesW()` with attribute checking for files
- `IsFolder()` - Uses `GetFileAttributesW()` with `FILE_ATTRIBUTE_DIRECTORY` checking
- `IsRoot()` - Checks for drive root patterns (e.g., "C:\")
- `GetRelativePathFor()` - Custom implementation using Windows path semantics

### File Operations
- `Delete()` - Uses `DeleteFileW()` Win32 API
- `Rename()` - Uses `MoveFileW()` Win32 API

### Folder Operations
- `GetFolders()` - Uses `FindFirstFileW()`, `FindNextFileW()`, `FindClose()` APIs
- `GetFiles()` - Uses `FindFirstFileW()`, `FindNextFileW()`, `FindClose()` APIs
- `CreateNonRecursively()` - Uses `CreateDirectoryW()` Win32 API
- `DeleteNonRecursively()` - Uses `RemoveDirectoryW()` Win32 API
- `Rename()` - Uses `MoveFileW()` Win32 API

## Analysis of Injection Pattern

From examining the existing DateTime and Locale injection implementations, the established pattern includes:

### Injectable Infrastructure Components
1. **Interface Implementation**: Platform-specific class implementing the interface
2. **Global Variable**: Static pointer to hold injected implementation
3. **Injection Function**: Function to set the global variable
4. **Getter Function**: Function that checks injection first, falls back to OS implementation
5. **OS Implementation Getter**: Platform-specific function returning default implementation

## Interface Requirements Analysis

Based on the interface defined in `IFileSystemImpl` from `..\..\Source\FileSystem.h`, the Windows implementation must provide:

```cpp
class IFileSystemImpl : public virtual Interface
{
public:
    // FilePath operations
    virtual void Initialize(WString& fullPath) const = 0;
    virtual bool IsFile(const WString& fullPath) const = 0;
    virtual bool IsFolder(const WString& fullPath) const = 0;
    virtual bool IsRoot(const WString& fullPath) const = 0;
    virtual WString GetRelativePathFor(const WString& fromPath, const WString& toPath) const = 0;
    
    // File operations
    virtual bool FileDelete(const FilePath& filePath) const = 0;
    virtual bool FileRename(const FilePath& filePath, const WString& newName) const = 0;
    
    // Folder operations
    virtual bool GetFolders(const FilePath& folderPath, collections::List<Folder>& folders) const = 0;
    virtual bool GetFiles(const FilePath& folderPath, collections::List<File>& files) const = 0;
    virtual bool CreateFolder(const FilePath& folderPath) const = 0;
    virtual bool DeleteFolder(const FilePath& folderPath) const = 0;
    virtual bool FolderRename(const FilePath& folderPath, const WString& newName) const = 0;
};
```

## Design Strategy

### 1. Windows Implementation Class Design

Create `WindowsFileSystemImpl` class following the same pattern as `WindowsDateTimeImpl`:

```cpp
class WindowsFileSystemImpl : public Object, public virtual IFileSystemImpl
{
public:
    // FilePath operations
    void Initialize(WString& fullPath) const override;
    bool IsFile(const WString& fullPath) const override;
    bool IsFolder(const WString& fullPath) const override;
    bool IsRoot(const WString& fullPath) const override;
    WString GetRelativePathFor(const WString& fromPath, const WString& toPath) const override;
    
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

### 2. Injectable Infrastructure Design

Following the established pattern in `DateTime` and `Locale`, the injectable infrastructure will include:

```cpp
// In FileSystem.Injectable.cpp
IFileSystemImpl* injectedFileSystemImpl = nullptr;

void InjectFileSystemImpl(IFileSystemImpl* impl)
{
    injectedFileSystemImpl = impl;
}

IFileSystemImpl* GetFileSystemImpl()
{
    return injectedFileSystemImpl ? injectedFileSystemImpl : GetOSFileSystemImpl();
}
```

### 3. Method Migration Strategy for Windows

Each Windows method will be migrated with minimal changes to fit the interface:

#### FilePath Operations
- `Initialize()`: Adapt current logic to modify the passed `WString&` parameter instead of member variable
- `IsFile()`, `IsFolder()`, `IsRoot()`: Move logic with parameter type adaptation
- `GetRelativePathFor()`: Adapt to use the new signature

#### File/Folder Operations
- All operations will extract `FilePath::GetFullPath()` from the parameter and apply existing Win32 API logic
- Maintain identical error handling and return value semantics

### 4. Method Redirection Implementation and Friend Class Considerations

Current platform methods will be updated to delegate to the injectable implementation. Special attention must be paid to method calling patterns within the classes:

```cpp
// In FilePath class methods
void FilePath::Initialize()
{
    GetFileSystemImpl()->Initialize(fullPath);
}

bool FilePath::IsFile() const
{
    return GetFileSystemImpl()->IsFile(fullPath);
}

// Similar pattern for File and Folder methods
bool File::Delete() const
{
    return GetFileSystemImpl()->FileDelete(filePath);
}
```

#### Friend Class Requirements

When the original implementation has FilePath, File, or Folder calling non-public static methods of itself, the `WindowsFileSystemImpl` class must be added as a friend class to maintain access. This is necessary because:

1. **Self-method calls**: In the original implementation, public methods may call private/protected static methods within the same class
2. **Access preservation**: The new implementation class needs the same level of access to maintain functionality
3. **Encapsulation maintenance**: Friend classes provide controlled access without breaking encapsulation

For example, if `FilePath` has private static helper methods used by platform-specific operations:

```cpp
class FilePath : public Object
{
    friend class WindowsFileSystemImpl;  // Add this line
    friend class LinuxFileSystemImpl;    // For Linux compatibility
    
    // ...existing code...
private:
    static void SomePrivateHelperMethod();
    
    // ...existing code...
};
```

#### Injectable vs Non-Injectable Method Distinction

The redirection strategy differs based on whether a method is injectable:

1. **Injectable methods** (platform-specific): Redirect to `GetFileSystemImpl()->MethodName()`
2. **Non-injectable methods** (in FileSystem.cpp): Continue calling methods directly within the class

This ensures that:
- Platform-specific behavior can be injected and tested
- Platform-agnostic behavior remains direct and efficient
- The injection boundary is clearly defined and consistent

## Windows-Specific Implementation Details

### Win32 API Handling
- All existing Win32 API calls (`GetFileAttributesW`, `DeleteFileW`, `CreateDirectoryW`, etc.) remain unchanged
- Error handling patterns preserved exactly
- Unicode string handling maintained with existing `WString` usage

### Path Handling Specifics
- Windows path delimiter (`\`) handling preserved
- Drive root detection logic maintained
- UNC path support continued if currently supported

### File Enumeration Patterns
- `FindFirstFile`/`FindNextFile`/`FindClose` patterns maintained exactly
- File attribute filtering logic preserved
- Directory traversal behavior unchanged

## Parameter Adaptation Strategy

### String Parameter Handling
- Current methods operate on member variables (`fullPath`, `filePath`)
- Interface methods take `WString` or `FilePath` parameters
- Implementation will appropriately convert between these representations

### Collection Parameters
- Methods like `GetFolders()` and `GetFiles()` take `collections::List<>` references
- Implementation will populate these collections exactly as current implementation does

## Error Handling and Compatibility

### Win32 Error Handling
- All Win32 API error checking preserved exactly
- `GetLastError()` usage patterns maintained where applicable
- Return value semantics unchanged

### Exception Handling
- Any existing exception throwing behavior preserved
- Error conditions and edge cases maintained exactly

## Build Integration Considerations

### Compilation Order
- `FileSystem.Injectable.cpp` must provide the infrastructure
- `FileSystem.Windows.cpp` must provide the Windows implementation
- Both files must be completed together to avoid linking issues

### Header Dependencies
- Ensure proper inclusion of Windows headers where needed
- Maintain existing include patterns for compatibility

### Friend Class Declaration Strategy
- Add friend class declarations for both Windows and Linux implementations to maintain cross-platform compatibility
- Ensure friend declarations are added consistently across FilePath, File, and Folder classes where needed

## Testing and Validation Strategy

### Behavioral Preservation
- All existing unit tests must continue to pass without modification
- File system operations must behave identically to current implementation
- Error conditions and edge cases preserved exactly

### Injection Mechanism Validation
- Verify that `InjectFileSystemImpl(nullptr)` restores default behavior
- Confirm that injection works correctly for test implementations
- Ensure no memory leaks or resource issues with injection switching

### Method Access Verification
- Verify that all previously accessible methods remain accessible through friend classes
- Ensure no compilation errors due to access violations
- Confirm that encapsulation is maintained while preserving functionality

## Implementation Benefits

### Testability Improvements
- Enables mock file system implementations for unit testing
- Allows testing of file system error conditions without real file system manipulation
- Supports isolation testing of components that depend on file system operations

### Code Organization
- Clear separation between Win32 API usage and business logic
- Consistent with established patterns in the codebase
- Easier maintenance and platform-specific debugging

### Future Extensibility
- Enables alternative implementations (e.g., memory-based, cloud-based)
- Supports different Windows API strategies if needed
- Allows runtime behavior modification for testing scenarios

## Risk Mitigation

### Build Integrity
- Complete both files in single task to avoid broken builds
- Thorough testing of compilation after each major change
- Verification that existing unit tests continue to pass

### Behavioral Compatibility
- Line-by-line verification of logic migration
- Careful handling of parameter conversions
- Preservation of all error handling semantics

### Access Control Integrity
- Careful review of friend class requirements
- Verification that all necessary access patterns are maintained
- Ensuring no security implications from friend class additions

## Conclusion

This design provides a comprehensive approach to completing the FileSystem injection pattern for Windows while maintaining 100% behavioral compatibility. The approach follows established patterns in the codebase and enables significant testing and extensibility improvements while preserving all existing functionality. Special attention to friend class requirements and method redirection patterns ensures that the migration maintains all existing access patterns and calling relationships.

# !!!FINISHED!!!
