# !!!TASK!!!

# PROBLEM DESCRIPTION

### Task 1-1: Define IFileSystemImpl Interface and Injection Infrastructure

Create the core interface `IFileSystemImpl` and injection infrastructure in `FileSystem.h` following the same pattern as `IDateTimeImpl` and `ILocaleImpl`. This includes defining all platform-specific methods that need to be injectable and the injection function.

**What to be done:**
- Declare `IFileSystemImpl` interface with pure virtual methods corresponding to all platform-specific operations
- Add forward declaration of `InjectFileSystemImpl` function 
- Add the interface declaration following the same structure as existing injection interfaces

**What to test in Unit Test:**
- No unit testing required for this task as it only involves interface declarations
- Compilation verification will ensure correct syntax

**What to test manually:**
- Verify the project still compiles after adding interface declarations
- Ensure no linking errors are introduced by the interface additions

**Reasons for this task:**
- Establishes the foundation for the entire injection system
- Follows established patterns in the codebase for consistency
- Required before any implementation work can begin
- Separates interface design from implementation concerns

**Support evidence:**
- `IDateTimeImpl` and `ILocaleImpl` interfaces in existing codebase provide the pattern to follow
- Analysis shows clear separation between platform-specific methods (in Windows/Linux files) and common methods (in main file)

## UPDATE

Looking at `File` and `Folder`, they only have a `FilePath` private member, so in `IFileSystemImpl`, functions for them should actually taking a `FilePath` instead of a `WString`.

# DESIGN DOCUMENT

## Overview

This design document outlines the implementation of dependency injection for the FileSystem module following the established patterns used by `DateTime` and `Locale` classes. The goal is to enable testability and platform abstraction for file system operations while maintaining backward compatibility.

## Analysis of Existing Injection Pattern

From examining `ILocaleImpl` in `..\..\Source\Locale.h`, the established injection pattern follows these principles:

1. **Interface Definition**: A pure virtual interface (`ILocaleImpl`) defines all platform-specific operations
2. **Injection Function**: A global injection function (`InjectLocaleImpl`) allows replacing the default implementation
3. **Default Implementation**: Each platform provides a `GetOSLocaleImpl()` function returning the OS-specific implementation
4. **Transparent Redirection**: Client code continues to call methods on the main classes, which internally redirect to the injectable implementation

## Platform-Specific Methods Analysis

Based on examination of `..\..\Source\FileSystem.Windows.cpp` and `..\..\Source\FileSystem.Linux.cpp`, the following methods are platform-specific and require injection:

### FilePath Class (from FilePath in FileSystem.h)
- `Initialize()` - Platform-specific path normalization and resolution
- `IsFile()` - Uses Win32 API `GetFileAttributesEx` vs Unix `stat`
- `IsFolder()` - Uses Win32 API `GetFileAttributesEx` vs Unix `stat`  
- `IsRoot()` - Different root concepts (empty string vs "/")
- `GetRelativePathFor()` - Uses Win32 API `PathRelativePathTo` vs custom implementation

### File Class (from File in FileSystem.h)
- `Delete()` - Uses Win32 API `DeleteFile` vs Unix `unlink`
- `Rename()` - Uses Win32 API `MoveFile` vs Unix `rename`

### Folder Class (from Folder in FileSystem.h)
- `GetFolders()` - Uses Win32 API `FindFirstFile`/`FindNextFile` vs Unix `opendir`/`readdir`
- `GetFiles()` - Uses Win32 API `FindFirstFile`/`FindNextFile` vs Unix `opendir`/`readdir`
- `CreateNonRecursively()` - Uses Win32 API `CreateDirectory` vs Unix `mkdir`
- `DeleteNonRecursively()` - Uses Win32 API `RemoveDirectory` vs Unix `rmdir`
- `Rename()` - Uses Win32 API `MoveFile` vs Unix `rename`

## Updated Design Based on Code Analysis

### 1. Revised Interface Design

After examining the `File` and `Folder` classes in `..\..\Source\FileSystem.h`, it's clear that both classes only contain a `FilePath` private member. This means the interface methods should accept `FilePath` objects rather than `WString` objects for better type safety and consistency with the existing design.

The updated `IFileSystemImpl` interface will be declared in `..\..\Source\FileSystem.h` with the following structure:

```cpp
/// <summary>Platform-specific file system implementation interface.</summary>
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

### 2. Design Rationale for Parameter Types

#### FilePath vs WString Decision

1. **File Operations**: Since `File` class contains only a `FilePath` member, it's more natural and type-safe to pass `FilePath` objects to the interface methods.

2. **Folder Operations**: Similarly, `Folder` class contains only a `FilePath` member, making `FilePath` the appropriate parameter type.

3. **FilePath Operations**: These methods operate on the raw path string and need to modify it, so `WString&` is appropriate for `Initialize()` and `const WString&` for query operations.

#### Return Type Considerations

For `GetFolders()` and `GetFiles()`, the methods should return collections of `Folder` and `File` objects respectively, matching the signatures of the existing public methods. This maintains consistency with the current API and simplifies the implementation.

### 3. Injection Infrastructure

Following the pattern from `ILocaleImpl`, these declarations will be added to `..\..\Source\FileSystem.h`:

```cpp
extern IFileSystemImpl* GetDefaultFileSystemImpl();
extern void InjectFileSystemImpl(IFileSystemImpl* impl);
```

### 4. Implementation Strategy

The implementation will follow the established pattern:

1. **Global Variables**: A global variable to hold the injected implementation
2. **Getter Function**: `GetDefaultFileSystemImpl()` checks injection first, falls back to OS implementation
3. **Platform Implementations**: Each platform file provides `GetOSFileSystemImpl()`
4. **Method Redirection**: Existing public methods redirect to the injectable implementation

### 5. Updated Method Signatures and Rationale

#### Why These Signatures Work Better

1. **Type Safety**: Using `FilePath` objects ensures type safety and prevents confusion between different string representations.

2. **Consistency**: The interface methods mirror the public API more closely, reducing the need for complex parameter conversions.

3. **Performance**: Passing `FilePath` objects directly avoids unnecessary string conversions and temporary object creation.

4. **Maintainability**: The interface is more intuitive and easier to implement correctly.

### 6. Backward Compatibility

The design ensures complete backward compatibility:
- All existing public APIs remain unchanged
- Default behavior (without injection) remains identical
- No changes required in client code
- Existing unit tests should continue to pass without modification

## Implementation Approach

### Phase 1: Interface Declaration (This Task)
- Add `IFileSystemImpl` interface to `..\..\Source\FileSystem.h`
- Add injection function declarations
- Ensure compilation succeeds

### Future Phases:
- Phase 2: Linux implementation (`..\..\Source\FileSystem.Linux.cpp`)
- Phase 3: Windows implementation and integration (`..\..\Source\FileSystem.Windows.cpp` and `..\..\Source\FileSystem.Injectable.cpp`)

## Design Rationale

### Why This Updated Approach

1. **Better Type Safety**: Using `FilePath` parameters prevents type confusion and makes the interface more robust
2. **API Consistency**: The interface methods closely mirror the public API signatures
3. **Reduced Complexity**: Less parameter conversion needed in implementations
4. **Performance**: Direct object passing avoids unnecessary string operations
5. **Maintainability**: More intuitive interface that's easier to implement correctly

### Alternative Approaches Considered

1. **String-Only Interface**: Would require more parameter conversions and be less type-safe
2. **Mixed Parameters**: Would be inconsistent and confusing
3. **Template-Based Approach**: Overly complex for this use case

The updated design with `FilePath` parameters provides the best balance of type safety, performance, and consistency with the existing codebase architecture.

# !!!FINISHED!!!
