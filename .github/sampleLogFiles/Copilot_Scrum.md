# !!!SCRUM!!!

# DESIGN REQUEST

Read about `DateTime` vs `InjectDateTimeImpl`, and `Locale` vs `InjectLocaleImpl`. You will design a solution about `InjectFileSystemImpl` for `File`, `Folder` and `FilePath`. Not every method need (or can) be injected, only inject methods that defined in `FileSystem.Windows.cpp` and `FileSystem.Linux.cpp`. Every methods in `FileSystem.cpp` is not for injection.

In `FileSystem.h`, you will need to declare `InjectFileSystemImpl` and `IFileSystemImpl`.

In the prepared almost empty `FileSystem.Injectable.cpp`, you will need to implement `InjectFileSystemImpl` and all redirections. In each file there will be a `GetOSFileSystemImpl` function, just like `GetOSDateTimeImpl` and `GetOSLocaleImpl`. Please be aware that, only one of them will be selected into build (obviously one for Windows and one for Linux), so it is completely OK to provide `GetOSFileSystemImpl` in each file, there will not be linking issues.

I proposed a task splitting like this:

1. Complete the declaration of `IFileSystemImpl` and `InjectFileSystemImpl`. No test plan needed.
2. Rewrite `FileSystem.Linux.cpp`. This file will not be consumed in this project so building the project cannot verify it, try your best to make it correct. No test plan needed.
3. Rewrite `FileSystem.Windows.cpp`, complete `FileSystem.Injectable.cpp`, you have to complete these 2 files in one task, otherwise the project won't build. No test plan needed as the unit test is already available.

## Analysis

Based on the examination of existing injection patterns for `DateTime` and `Locale`, and analysis of the FileSystem codebase, I need to implement a similar injection pattern for the FileSystem classes. The key insights are:

1. **Injection Pattern**: Follow the same pattern as `DateTime` and `Locale` where there's an interface (`IFileSystemImpl`), an injection function (`InjectFileSystemImpl`), and a global variable that gets checked before falling back to the OS implementation.

2. **Scope of Injection**: Only methods implemented in platform-specific files (`FileSystem.Windows.cpp` and `FileSystem.Linux.cpp`) need injection. Methods in `FileSystem.cpp` are platform-agnostic and should not be injected.

3. **Platform-Specific Methods**: From analysis, the following methods are platform-specific and need injection:
   - **FilePath**: `Initialize()`, `IsFile()`, `IsFolder()`, `IsRoot()`, `GetRelativePathFor()`
   - **File**: `Delete()`, `Rename()`
   - **Folder**: `GetFolders()`, `GetFiles()`, `CreateNonRecursively()`, `DeleteNonRecursively()`, `Rename()`

4. **Architecture**: Each platform file will provide a `GetOSFileSystemImpl()` function, and there will be an injectable implementation system with a global variable that can override the OS implementation.

## Phase 1: Interface and Infrastructure Design

This phase focuses on establishing the foundational interfaces and injection infrastructure that mirrors the existing patterns used by DateTime and Locale.

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

## Phase 2: Linux Platform Implementation

This phase implements the Linux-specific file system operations using the new injection pattern.

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

## Phase 3: Windows Platform Implementation and Integration

This phase completes the injection system by implementing the Windows-specific operations and integrating everything together.

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

# !!!FINISHED!!!
