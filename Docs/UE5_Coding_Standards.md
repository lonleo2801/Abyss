# UE5 Coding Standards & Agent Memory

This file serves as a reference for the required coding standards for the "Abyss" project. All future code modifications must adhere to these rules.

## Formatting & Style
- **Brace Style**: Allman Style (Braces always on a new line).
  ```cpp
  // Correct
  void MyFunction()
  {
      if (bCondition)
      {
          DoSomething();
      }
  }
  
  // Incorrect
  void MyFunction() {
      if (bCondition) {
          DoSomething();
      }
  }
  ```
- **Indentation**: Standard UE indentation (Tab or 4 spaces, per project settings).
- **Line Endings**: Windows (CRLF).

## Naming Conventions
- **PascalCase**: Used for implementation class names, functions, and variables.
- **Prefixes**:
  - `U` for `UObject` derived classes (e.g., `UAbyssInventoryComponent`).
  - `A` for `AActor` derived classes (e.g., `AAbyssCharacter`).
  - `F` for Structs and raw C++ classes (e.g., `FAbyssData`, `FMyClass`).
  - `E` for Enums (e.g., `EWeaponSlot`).
  - `I` for Interfaces (e.g., `IAbyssInteractable`).
  - `T` for Templates (e.g., `TArray`, `TSubclassOf`).
  - `b` for Booleans (e.g., `bIsDead`, `bHasWeapon`).
- **Const Correctness**: rigorous usage of `const` for methods that do not modify state and for input parameters references.

## Comments
- **Documentation Comments**: Use JavaDoc style `/** ... */` for all classes, functions, and public members.
  - Use `@param Name Description` for parameters.
  - Use `@return Description` for return values.
  - **Note**: Comments can be in Chinese or English.
  ```cpp
  /**
   * 执行某种操作
   * @param Value 输入值
   * @return 若成功返回 true
   */
  bool DoAction(float Value);
  ```
- **Inline Comments**: Use `//` for implementation details or simple member descriptions.
- **Separators**: Use `// --- Section Name ---` or `// ========================== ` to separate logical blocks.

## C++ Best Practices
- **Null Pointers**: Always use `nullptr`, never `NULL`.
- **Strings**: Use `TEXT("String")` macro for literals.
- **Casts**: Use `Cast<T>(Obj)` for UObjects, avoid C-style casts.
- **Includes**: Minimal includes in `.h`, forward declarations where possible. `CoreMinimal.h` as the first include in headers (usually).

## Specific Instructions for Agent
- Always verify braces are on new lines for any code generation.
- Respect existing indentation style if it conflicts (but prefer maintaining Allman braces).
- Ensure new functions have proper `/** */` documentation.
