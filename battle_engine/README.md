# Battle Engine Prototype (C++)

This is a standalone prototype for a narrative-driven deck-builder battle system.
It is written in standard C++17 to ensure portability and ease of verification, but structured to be easily adaptable to **Unreal Engine 5**.

## Structure

*   `include/`: Header files defining the core classes (`Card`, `Combatant`, `BattleManager`, etc.).
*   `src/`: Implementation files.
*   `data/`: Placeholder for data files (cards.json).
*   `main.cpp`: A console-based simulation loop to demonstrate the system.

## Unreal Engine Integration Guide

This codebase uses standard C++ types (`std::string`, `std::vector`, `std::shared_ptr`). To port this to Unreal Engine:

### 1. Type Replacements
*   `std::string` -> `FString`
*   `std::vector` -> `TArray`
*   `std::shared_ptr` -> `TStrongObjectPtr` or standard `UPROPERTY()` managed pointers.
*   `int` -> `int32`
*   `float` -> `float` (or `double`)

### 2. Class Definitions
*   Classes should inherit from `UObject` or `AActor`.
    *   `BattleManager` -> `AGameMode` or `AActor`.
    *   `Combatant` -> `UActorComponent` or `AActor` (if they have a physical presence).
    *   `Card` -> `UObject` (Data Asset) or just a `USTRUCT` if lightweight.
*   Add `UCLASS()`, `GENERATED_BODY()`.
*   Expose variables to Blueprints using `UPROPERTY(EditAnywhere, BlueprintReadWrite)`.
*   Expose functions using `UFUNCTION(BlueprintCallable)`.

### 3. Event System
*   The `MulticastDelegate` in `CoreTypes.h` is a mock. In Unreal, use `DECLARE_DYNAMIC_MULTICAST_DELEGATE` macros to create events that Blueprints can bind to.

### 4. JSON Loading
*   Unreal has `FJsonObject` and `FJsonSerializer`. The `CardFactory` logic can be adapted to parse JSON files into `UDataAsset` instances or runtime `UObject`s.

## building and Running

To build the simulation:

```bash
make
./battle_engine/run_battle
```

## Features Implemented

*   **Turn-based Loop**: Player -> Enemy -> Repeat.
*   **Card System**: Attack, Skill, Status effects.
*   **Deck Management**: Draw, Discard, Reshuffle.
*   **Status Effects**: Extensible buff/debuff system (Weak, Vulnerable, etc.).
*   **Enemy AI**: Simple Intent system (Attack, Defend, Debuff).
*   **Event Hooks**: Delegate system for StartTurn, EndTurn, CardPlayed.
