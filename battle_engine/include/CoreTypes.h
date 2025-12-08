#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <map>
#include <iostream>

// --- Unreal Engine Compatibility Layer ---
// In a real Unreal project, you would not have these typedefs.
// Instead, you would use the actual UE4/5 types.
// This section exists to make the logic compile in standard C++ for verification
// while allowing the code style to mimic Unreal Engine conventions.

using int32 = int;
using float32 = float;
using FString = std::string;

// Mimic TArray
template<typename T>
using TArray = std::vector<T>;

// Mimic TSharedPtr (Using std::shared_ptr for this standalone simulation)
template<typename T>
using TSharedPtr = std::shared_ptr<T>;

// --- Enums ---
// In Unreal: UENUM(BlueprintType) enum class ECardType : uint8 { ... };

enum class ECardType {
    Attack,
    Skill,
    Power,
    Status,
    Curse
};

enum class ECardTarget {
    Self,
    Enemy,
    AllEnemies,
    None
};

// --- Forward Declarations ---
class Combatant;
class BattleManager;
class Card;
class StatusEffect;

// --- Context Structs ---
// In Unreal: USTRUCT(BlueprintType) struct FCombatContext { GENERATED_BODY() ... };

struct FCombatContext {
    Combatant* Source;
    Combatant* Target;
    BattleManager* Manager;
};

// --- Delegate / Event System Simulation ---
// In Unreal, we use DECLARE_DYNAMIC_MULTICAST_DELEGATE or similar macros.
// Here is a simple std::function based multicast delegate implementation.

template<typename... Args>
class MulticastDelegate {
public:
    using DelegateType = std::function<void(Args...)>;
    std::vector<DelegateType> Listeners;

    void Add(DelegateType Listener) {
        Listeners.push_back(Listener);
    }

    void Broadcast(Args... args) {
        for (auto& Listener : Listeners) {
            Listener(args...);
        }
    }
};
