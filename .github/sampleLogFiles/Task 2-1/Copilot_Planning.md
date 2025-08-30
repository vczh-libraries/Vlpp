# !!!PLANNING!!!

# UPDATES

# UPDATE
You don't have to do the complete section of `### 6. Update Existing Method Implementations` as it would be in the next task.

# Improvement Plan

## Analysis of Current Code Structure

Based on examination of the existing codebase, I need to understand the current implementation and how to transform it to use the injection pattern:

### Current Linux Implementation Analysis
The current `FileSystem.Linux.cpp` contains platform-specific implementations for:

1. **FilePath methods**:
   - `Initialize()` - Path normalization using `getcwd()` and Unix path handling
   - `IsFile()` - Uses `stat()` with `S_ISREG()` check
   - `IsFolder()` - Uses `stat()` with `S_ISDIR()` check  
   - `IsRoot()` - Simple comparison with "/"
   - `GetRelativePathFor()` - Custom Unix path relative calculation

2. **File methods**:
   - `Delete()` - Uses `unlink()` system call
   - `Rename()` - Uses `rename()` system call

3. **Folder methods**:
   - `GetFolders()` - Uses `opendir()/readdir()/closedir()` for directory enumeration
   - `GetFiles()` - Uses `opendir()/readdir()/closedir()` for file enumeration
   - `CreateNonRecursively()` - Uses `mkdir()` with 0777 permissions
   - `DeleteNonRecursively()` - Uses `rmdir()` system call
   - `Rename()` - Uses `rename()` system call

### Interface Requirements Analysis
From `FileSystem.h`, the `IFileSystemImpl` interface defines these methods that need implementation:

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

## Detailed Implementation Plan

### 1. Create LinuxFileSystemImpl Class

**Location**: `FileSystem.Linux.cpp`
**Purpose**: Encapsulate all Linux-specific file system operations in a class implementing `IFileSystemImpl`

**Implementation Strategy**:
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

**Why this approach**: This directly encapsulates all platform-specific logic in a single class, making it easy to inject alternative implementations for testing. The class inherits from both `Object` (following the pattern) and `IFileSystemImpl` to provide the required interface.

### 2. Migrate FilePath Methods

#### 2.1 Initialize Method
**Current code location**: `FilePath::Initialize()` in `FileSystem.Linux.cpp`
**New implementation**: `LinuxFileSystemImpl::Initialize(WString& fullPath)`

**Key changes needed**:
- Change from operating on `this->fullPath` to operating on parameter `fullPath`
- Extract all the existing logic without modification
- Maintain the exact same behavior for path normalization and `getcwd()` usage

**Migration strategy**: Copy the existing implementation and change the target variable from `this->fullPath` to the parameter `fullPath`.

#### 2.2 IsFile Method
**Current code location**: `FilePath::IsFile()` in `FileSystem.Linux.cpp`
**New implementation**: `LinuxFileSystemImpl::IsFile(const WString& fullPath)`

**Key changes needed**:
- Change from using `this->fullPath` to using the parameter `fullPath`
- Keep identical `stat()` call and `S_ISREG()` check
- Maintain same error handling (return false on stat failure)

#### 2.3 IsFolder Method
**Current code location**: `FilePath::IsFolder()` in `FileSystem.Linux.cpp`
**New implementation**: `LinuxFileSystemImpl::IsFolder(const WString& fullPath)`

**Key changes needed**:
- Change from using `this->fullPath` to using the parameter `fullPath`
- Keep identical `stat()` call and `S_ISDIR()` check
- Maintain same error handling

#### 2.4 IsRoot Method
**Current code location**: `FilePath::IsRoot()` in `FileSystem.Linux.cpp`
**New implementation**: `LinuxFileSystemImpl::IsRoot(const WString& fullPath)`

**Key changes needed**:
- Change from using `this->fullPath` to using the parameter `fullPath`
- Keep simple comparison `fullPath == L"/"`

#### 2.5 GetRelativePathFor Method
**Current code location**: `FilePath::GetRelativePathFor()` in `FileSystem.Linux.cpp`
**New implementation**: `LinuxFileSystemImpl::GetRelativePathFor(const WString& fromPath, const WString& toPath, bool fromIsFolder, bool toIsFolder)`

**Key changes needed**:
- Replace `this->fullPath` usage with `fromPath` parameter
- Replace `_filePath.fullPath` usage with `toPath` parameter
- The `fromIsFolder` and `toIsFolder` flags replace the current logic that determines if a path is a folder by calling `IsFolder()` and `GetFolder()`
- Update the path component processing to work with the new parameter structure
- Maintain the exact same relative path calculation algorithm

**Why these changes are needed**: The interface signature requires separate source and target paths with explicit folder flags rather than having access to FilePath objects with their methods.

### 3. Migrate File Methods

#### 3.1 FileDelete Method
**Current code location**: `File::Delete()` in `FileSystem.Linux.cpp`
**New implementation**: `LinuxFileSystemImpl::FileDelete(const FilePath& filePath)`

**Key changes needed**:
- Change from using `this->filePath.GetFullPath()` to `filePath.GetFullPath()`
- Keep identical `unlink()` call and error handling
- Maintain same return value logic (return true on success)

#### 3.2 FileRename Method
**Current code location**: `File::Rename()` in `FileSystem.Linux.cpp`
**New implementation**: `LinuxFileSystemImpl::FileRename(const FilePath& filePath, const WString& newName)`

**Key changes needed**:
- Change from using `this->filePath` to the parameter `filePath`
- Keep identical logic for constructing old and new file names
- Maintain same `rename()` system call usage

### 4. Migrate Folder Methods

#### 4.1 GetFolders Method
**Current code location**: `Folder::GetFolders()` in `FileSystem.Linux.cpp`
**New implementation**: `LinuxFileSystemImpl::GetFolders(const FilePath& folderPath, collections::List<Folder>& folders)`

**Key changes needed**:
- Change from using `this->filePath` to the parameter `folderPath`
- Remove the `Exists()` check since interface implementation shouldn't assume existence checking
- Keep identical `opendir()/readdir()/closedir()` logic
- Maintain same filtering logic for "." and ".." entries
- Keep same `IsFolder()` check for directory detection

**Why remove Exists() check**: The interface method should focus purely on the enumeration operation. Existence checking can be done at a higher level if needed.

#### 4.2 GetFiles Method
**Current code location**: `Folder::GetFiles()` in `FileSystem.Linux.cpp`
**New implementation**: `LinuxFileSystemImpl::GetFiles(const FilePath& folderPath, collections::List<File>& files)`

**Key changes needed**:
- Change from using `this->filePath` to the parameter `folderPath`
- Remove the `Exists()` check for the same reason as GetFolders
- Keep identical directory enumeration logic
- Maintain same `IsFile()` check for file detection

#### 4.3 CreateFolder Method
**Current code location**: `Folder::CreateNonRecursively()` in `FileSystem.Linux.cpp`
**New implementation**: `LinuxFileSystemImpl::CreateFolder(const FilePath& folderPath)`

**Key changes needed**:
- Change from using `this->filePath.GetFullPath()` to `folderPath.GetFullPath()`
- Keep identical `mkdir()` call with 0777 permissions
- Maintain same return value logic

#### 4.4 DeleteFolder Method
**Current code location**: `Folder::DeleteNonRecursively()` in `FileSystem.Linux.cpp`
**New implementation**: `LinuxFileSystemImpl::DeleteFolder(const FilePath& folderPath)`

**Key changes needed**:
- Change from using `this->filePath.GetFullPath()` to `folderPath.GetFullPath()`
- Keep identical `rmdir()` call
- Maintain same return value logic

#### 4.5 FolderRename Method
**Current code location**: `Folder::Rename()` in `FileSystem.Linux.cpp`
**New implementation**: `LinuxFileSystemImpl::FolderRename(const FilePath& folderPath, const WString& newName)`

**Key changes needed**:
- Change from using `this->filePath` to the parameter `folderPath`
- Keep identical logic for constructing old and new folder names
- Maintain same `rename()` system call usage

### 5. Add Global Instance and Getter Function

**Implementation**:
```cpp
LinuxFileSystemImpl osFileSystemImpl;

IFileSystemImpl* GetOSFileSystemImpl()
{
    return &osFileSystemImpl;
}
```

**Location**: At the end of `FileSystem.Linux.cpp`
**Purpose**: Provides the Linux implementation instance that can be retrieved by the injection system

**Why global static instance**: Following the same pattern as DateTime and Locale implementations. This ensures a single instance throughout the application lifetime and avoids dynamic allocation issues.

### 6. Update Existing Method Implementations (SKIPPED)

**Note**: As per the update instruction, the complete section of updating existing method implementations will be handled in the next task. This section would involve updating all existing FilePath, File, and Folder methods in `FileSystem.Linux.cpp` to use the injection pattern by calling `GetFileSystemImpl()` and delegating to the appropriate interface methods.

The redirection pattern would follow this structure:
- FilePath methods would call corresponding interface methods through `GetFileSystemImpl()`
- File methods would delegate to `FileDelete()` and `FileRename()` interface methods
- Folder methods would delegate to the appropriate folder interface methods

This ensures the same public API is maintained while enabling dependency injection.

### 7. Handle Special Cases and Edge Conditions

#### 7.1 GetRelativePathFor Parameter Adaptation
The current implementation calls `IsFolder()` and `GetFolder()` methods on FilePath objects, but the interface version receives boolean flags. The implementation needs to adapt:

**Current logic**:
```cpp
GetPathComponents(IsFolder() ? fullPath : GetFolder().GetFullPath(), srcComponents);
```

**New logic**:
```cpp
GetPathComponents(fromIsFolder ? fromPath : FilePath(fromPath).GetFolder().GetFullPath(), srcComponents);
```

**Why this change is needed**: The interface separates the concerns of path operations from folder detection to make the implementation more focused and testable.

#### 7.2 Error Handling Consistency
All error handling patterns from the original implementation must be preserved:
- `stat()` failure returns false for file/folder checks
- Directory operation failures return false
- System call return codes are checked identically

**Why preserve error handling**: Existing client code may depend on specific error behavior, so maintaining compatibility is crucial.

#### 7.3 String Conversion Handling
All `wtoa()` and `atow()` conversions for Linux system calls must be preserved exactly:
```cpp
AString path = wtoa(fullPath);
// Use path.Buffer() with system calls
WString childName = atow(AString(entry->d_name));
```

**Why preserve conversions**: Linux system calls expect char* strings, so the Unicode to ASCII conversions are essential for correct operation.

## Summary of Changes

The implementation will refactor `FileSystem.Linux.cpp` to:

1. **Extract platform logic** into `LinuxFileSystemImpl` class implementing `IFileSystemImpl`
2. **Preserve all existing behavior** through careful parameter adaptation  
3. **Enable dependency injection** while maintaining API compatibility
4. **Follow established patterns** used by DateTime and Locale implementations
5. **Maintain error handling** and system call usage exactly as before

The result will be a Linux file system implementation that supports dependency injection while preserving 100% behavioral compatibility with the existing code.

# Test Plan

## Overview

Since this task involves refactoring existing Linux implementation to use dependency injection without changing functionality, the test plan focuses on ensuring behavioral compatibility and correctness of the injection pattern implementation.

## Test Strategy

The testing approach will be primarily manual verification since:
1. No unit testing is required per the task specification
2. Linux implementation won't be consumed in Windows project build
3. The focus is on code correctness and pattern compliance rather than functional testing

## Test Categories

### 1. Code Structure and Pattern Compliance Tests

#### 1.1 Interface Implementation Verification
**Test Objective**: Verify that `LinuxFileSystemImpl` correctly implements `IFileSystemImpl`

**Test Steps**:
1. Review class declaration to ensure proper inheritance from `Object` and `IFileSystemImpl`
2. Verify all interface methods are implemented with correct signatures
3. Check that all methods are marked with `override` keyword
4. Validate const-correctness matches interface requirements

**Expected Results**:
- Class compiles without interface implementation errors
- All virtual methods from interface are implemented
- Method signatures exactly match interface specification
- Proper const-correctness maintained

#### 1.2 Global Instance and Getter Pattern Verification
**Test Objective**: Verify the global instance and getter function follow established patterns

**Test Steps**:
1. Check that `osFileSystemImpl` is declared as global static instance
2. Verify `GetOSFileSystemImpl()` function returns pointer to the static instance
3. Compare pattern with existing `GetOSDateTimeImpl()` implementation for consistency

**Expected Results**:
- Global instance properly declared and accessible
- Getter function returns correct pointer
- Pattern matches DateTime/Locale implementations exactly

### 2. Method Migration Correctness Tests

#### 2.1 FilePath Methods Migration Test
**Test Objective**: Verify all FilePath platform methods are correctly migrated to implementation class

**Test Methods to Verify**:
- `Initialize()` - Path normalization and `getcwd()` usage
- `IsFile()` - `stat()` with `S_ISREG()` check
- `IsFolder()` - `stat()` with `S_ISDIR()` check  
- `IsRoot()` - Simple "/" comparison
- `GetRelativePathFor()` - Relative path calculation

**Test Steps**:
1. Review each method implementation in `LinuxFileSystemImpl`
2. Compare logic with original implementations in platform-specific methods
3. Verify parameter adaptations are correct (e.g., `fullPath` parameter vs member variable)
4. Check that all Linux system calls are preserved identically

**Expected Results**:
- All logic identical to original implementation
- Parameter usage correctly adapted for interface signature
- System calls (`stat`, `getcwd`) used identically
- Error handling patterns preserved

#### 2.2 File Methods Migration Test
**Test Objective**: Verify File operations are correctly migrated

**Test Methods to Verify**:
- `FileDelete()` - `unlink()` system call usage
- `FileRename()` - `rename()` system call usage

**Test Steps**:
1. Compare `FileDelete()` implementation with original `File::Delete()`
2. Compare `FileRename()` implementation with original `File::Rename()`
3. Verify parameter extraction from `FilePath` objects is correct
4. Check return value handling matches original behavior

**Expected Results**:
- `unlink()` and `rename()` calls identical to original
- Parameter handling correctly extracts full paths
- Error return values preserved exactly

#### 2.3 Folder Methods Migration Test
**Test Objective**: Verify Folder operations are correctly migrated

**Test Methods to Verify**:
- `GetFolders()` - Directory enumeration with filtering
- `GetFiles()` - File enumeration
- `CreateFolder()` - `mkdir()` with proper permissions
- `DeleteFolder()` - `rmdir()` system call
- `FolderRename()` - `rename()` system call

**Test Steps**:
1. Review directory enumeration logic in `GetFolders()` and `GetFiles()`
2. Verify `opendir()/readdir()/closedir()` usage is identical
3. Check filtering logic for "." and ".." entries
4. Verify `mkdir()` permissions (0777) are preserved
5. Check folder creation and deletion system calls

**Expected Results**:
- Directory enumeration logic identical to original
- System calls used with same parameters
- Filtering and type checking logic preserved
- File/folder creation uses correct permissions

### 3. Parameter Adaptation Tests

#### 3.1 String Parameter Conversion Test
**Test Objective**: Verify correct parameter conversions between method signatures

**Test Focus Areas**:
- `WString& fullPath` parameter vs member variable usage
- `FilePath` parameter extraction to string paths
- Collection parameter passing for folder/file enumeration

**Test Steps**:
1. Verify `Initialize()` operates on parameter rather than member variable
2. Check that `FilePath` parameters are correctly converted to strings via `GetFullPath()`
3. Verify collection parameters maintain reference semantics

**Expected Results**:
- Parameter conversions are correct and safe
- No unintended copying of large objects
- Reference semantics preserved where needed

#### 3.2 GetRelativePathFor Special Case Test
**Test Objective**: Verify complex parameter adaptation for relative path calculation

**Test Steps**:
1. Review how `fromIsFolder` and `toIsFolder` flags replace `IsFolder()` calls
2. Check that path component extraction works with new signature
3. Verify relative path calculation algorithm remains identical

**Expected Results**:
- Boolean flags correctly replace folder detection calls
- Path calculation algorithm produces identical results
- Edge cases handled consistently

### 4. Compilation and Integration Tests

#### 4.1 Compilation Verification Test
**Test Objective**: Ensure all code changes compile successfully

**Test Steps**:
1. Attempt compilation of modified `FileSystem.Linux.cpp`
2. Check for any interface implementation errors
3. Verify no linking issues with global functions

**Expected Results**:
- Clean compilation with no errors
- No interface implementation warnings
- All includes and dependencies resolved

#### 4.2 Pattern Consistency Test
**Test Objective**: Verify implementation follows established patterns exactly

**Test Steps**:
1. Compare implementation structure with `LinuxDateTimeImpl`
2. Verify naming conventions match established patterns
3. Check that global variable and getter function naming is consistent

**Expected Results**:
- Implementation structure matches DateTime pattern
- Naming conventions consistent with codebase
- Pattern usage identical to other injection implementations

## Test Execution Plan

### Phase 1: Static Code Review
1. Review all method migrations for correctness
2. Verify interface compliance
3. Check pattern consistency

### Phase 2: Compilation Testing
1. Attempt compilation of modified files
2. Resolve any compilation errors
3. Verify clean build

### Phase 3: Logic Verification
1. Trace through complex methods like `GetRelativePathFor()`
2. Verify system call usage preservation
3. Check error handling paths

## Success Criteria

The implementation will be considered successful when:

1. **All interface methods implemented** - `LinuxFileSystemImpl` provides complete implementation of `IFileSystemImpl`
2. **Behavioral compatibility maintained** - All operations produce identical results to original implementation
3. **Pattern compliance achieved** - Implementation follows DateTime/Locale injection patterns exactly
4. **Clean compilation** - Code compiles without errors or warnings
5. **System call preservation** - All Linux-specific API usage remains identical
6. **Parameter handling correct** - All parameter conversions between signatures work properly

## Risk Mitigation

### Risk: Parameter Conversion Errors
**Mitigation**: Careful review of each parameter adaptation, especially for complex methods like `GetRelativePathFor()`

### Risk: System Call Compatibility Issues
**Mitigation**: Preserve exact system call usage including error handling patterns

### Risk: Memory Management Problems
**Mitigation**: Follow established patterns for global instance management

### Risk: Interface Signature Mismatches
**Mitigation**: Verify each method signature against interface definition during implementation

This comprehensive test plan ensures that the Linux FileSystem injection implementation maintains full compatibility while properly implementing the dependency injection pattern.

# !!!FINISHED!!!
